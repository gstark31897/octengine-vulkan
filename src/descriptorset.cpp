#include <vulkan/vulkan.h>

#include "descriptorset.h"
#include "buffer.h"
#include "util.h"

#include <iostream>
#include <array>


void descriptor_set_create_layout(descriptor_set_t *descriptorSet)
{
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    for (auto it = descriptorSet->buffers.begin(); it != descriptorSet->buffers.end(); ++it)
    {
        VkDescriptorSetLayoutBinding binding = {};
        binding.binding = it->binding;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        binding.descriptorCount = 1;
        binding.stageFlags = it->vertex ? VK_SHADER_STAGE_VERTEX_BIT : VK_SHADER_STAGE_FRAGMENT_BIT;
        binding.pImmutableSamplers = nullptr;
        bindings.push_back(binding);
    }

    for (auto it = descriptorSet->textures.begin(); it != descriptorSet->textures.end(); ++it)
    {
        size_t bindIdx = it->binding;
        if (!it->texture->combined)
        {
            VkDescriptorSetLayoutBinding binding = {};
            binding.binding = bindIdx;
            binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
            binding.descriptorCount = 1;
            binding.stageFlags = it->vertex ? VK_SHADER_STAGE_VERTEX_BIT : VK_SHADER_STAGE_FRAGMENT_BIT;
            binding.pImmutableSamplers = nullptr;
            bindings.push_back(binding);
            bindIdx++;
        }

        VkDescriptorSetLayoutBinding binding = {};
        binding.binding = bindIdx;
        binding.descriptorType = it->texture->combined ? VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER : VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        binding.descriptorCount = it->texture->combined ? 1 : 80;
        binding.stageFlags = it->vertex ? VK_SHADER_STAGE_VERTEX_BIT : VK_SHADER_STAGE_FRAGMENT_BIT;
        binding.pImmutableSamplers = nullptr;
        bindings.push_back(binding);
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = bindings.size();
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(descriptorSet->device, &layoutInfo, nullptr, &descriptorSet->descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void descriptor_set_create_pool(descriptor_set_t *descriptorSet)
{
    size_t poolSize = descriptorSet->buffers.size() + descriptorSet->textures.size();
    for (auto it = descriptorSet->textures.begin(); it != descriptorSet->textures.end(); ++it)
    {
        if (!it->texture->combined)
            ++poolSize;
    }

    std::vector<VkDescriptorPoolSize> poolSizes(poolSize);
    for (auto it = descriptorSet->buffers.begin(); it != descriptorSet->buffers.end(); ++it)
    {
        poolSizes.at(it->binding).type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes.at(it->binding).descriptorCount = 1;
    }

    for (auto it = descriptorSet->textures.begin(); it != descriptorSet->textures.end(); ++it)
    {
        size_t bindIdx = it->binding;
        if (!it->texture->combined)
        {
            poolSizes.at(bindIdx).type = VK_DESCRIPTOR_TYPE_SAMPLER;
            poolSizes.at(bindIdx).descriptorCount = 1;
            ++bindIdx;
        }

        poolSizes.at(bindIdx).type = it->texture->combined ? VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER : VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        poolSizes.at(bindIdx).descriptorCount = it->texture->combined ? 1 : 80;
    }

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = 1;

    if (vkCreateDescriptorPool(descriptorSet->device, &poolInfo, nullptr, &descriptorSet->descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void descriptor_set_create_descriptor_set(descriptor_set_t *descriptorSet)
{
    VkDescriptorSetLayout layouts[] = {descriptorSet->descriptorSetLayout};
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorSet->descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = layouts;

    if (vkAllocateDescriptorSets(descriptorSet->device, &allocInfo, &descriptorSet->descriptorSet) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor set!");
    }

    std::vector<VkWriteDescriptorSet> descriptorWrites;
    std::vector<VkDescriptorBufferInfo*> bufferInfos;
    std::vector<VkDescriptorImageInfo*> samplerInfos;
    std::vector<std::vector<VkDescriptorImageInfo>*> imageInfos;
    for (auto it = descriptorSet->buffers.begin(); it != descriptorSet->buffers.end(); ++it)
    {
        VkDescriptorBufferInfo *bufferInfo = new VkDescriptorBufferInfo();
        bufferInfo->buffer = it->uniformBuffer;
        bufferInfo->offset = 0;
        bufferInfo->range = it->size;
        bufferInfos.push_back(bufferInfo);

        VkWriteDescriptorSet write;
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = descriptorSet->descriptorSet;
        write.dstBinding = it->binding;
        write.dstArrayElement = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.descriptorCount = 1;
        write.pBufferInfo = bufferInfo;
        write.pNext = NULL;
        descriptorWrites.push_back(write);
    }

    for (auto it = descriptorSet->textures.begin(); it != descriptorSet->textures.end(); ++it)
    {
        std::vector<VkDescriptorImageInfo> *imageInfo = new std::vector<VkDescriptorImageInfo>();
        size_t binding = it->binding;
        if (!it->texture->combined)
        {
            {
                VkDescriptorImageInfo *info = new VkDescriptorImageInfo();
                info->imageLayout = it->shadow ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                info->sampler = it->texture->sampler;
                samplerInfos.push_back(info);

                VkWriteDescriptorSet write;
                write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.dstSet = descriptorSet->descriptorSet;
                write.dstBinding = binding;
                write.dstArrayElement = 0;
                write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
                write.descriptorCount = 1;
                write.pImageInfo = info;
                write.pNext = NULL;
                descriptorWrites.push_back(write);

                binding++;
            }

            for (auto it2 = it->texture->data.begin(); it2 != it->texture->data.end(); ++it2)
            {
                VkDescriptorImageInfo info;
                info.imageLayout = it->shadow ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                info.imageView = it2->image.view;
                info.sampler = it->texture->combined ? it->texture->sampler : VK_NULL_HANDLE;
                imageInfo->push_back(info);
            }

            for (size_t i = it->texture->data.size(); !it->texture->combined && i < 80; ++i)
            {
                VkDescriptorImageInfo info;
                info.imageLayout = it->shadow ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                info.imageView = it->texture->data.back().image.view;
                imageInfo->push_back(info);
            }
        }
        else
        {
            VkDescriptorImageInfo info;
            info.imageLayout = it->shadow ?  VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            info.imageView = it->texture->data[0].image.view;
            info.sampler = it->texture->sampler;
            imageInfo->push_back(info);
        }
        imageInfos.push_back(imageInfo);

        VkWriteDescriptorSet write;
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = descriptorSet->descriptorSet;
        write.dstBinding = binding;
        write.dstArrayElement = 0;
        write.descriptorType = it->texture->combined ? VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER : VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        write.descriptorCount = imageInfo->size();
        write.pImageInfo = imageInfo->data();
        write.pNext = NULL;
        descriptorWrites.push_back(write);
    }

    vkUpdateDescriptorSets(descriptorSet->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

    for (auto it = bufferInfos.begin(); it != bufferInfos.end(); ++it)
        delete *it;
    for (auto it = imageInfos.begin(); it != imageInfos.end(); ++it)
        delete *it;
    for (auto it = samplerInfos.begin(); it != samplerInfos.end(); ++it)
        delete *it;
}

void descriptor_set_setup(descriptor_set_t *descriptorSet, VkDevice device, VkPhysicalDevice physicalDevice)
{
    descriptorSet->device = device;
    descriptorSet->physicalDevice = physicalDevice;
}

void descriptor_set_add_buffer(descriptor_set_t *descriptorSet, size_t size, uint32_t binding, bool vertex)
{
    descriptor_buffer_t newBuffer;
    newBuffer.size = size;
    newBuffer.binding = binding;
    newBuffer.vertex = vertex;
    create_buffer(descriptorSet->device, descriptorSet->physicalDevice, newBuffer.size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &newBuffer.uniformBuffer, &newBuffer.uniformBufferMemory);
    descriptorSet->buffers.push_back(newBuffer);
}

void descriptor_set_add_texture(descriptor_set_t *descriptorSet, texture_t *texture, uint32_t binding, bool vertex)
{
    descriptor_texture_t newTexture;
    newTexture.texture = texture;
    newTexture.binding = binding;
    newTexture.vertex = vertex;
    descriptorSet->textures.push_back(newTexture);
}

void descriptor_set_add_image(descriptor_set_t *descriptorSet, image_t *image, uint32_t binding, bool vertex, bool repeat, bool shadow)
{
    texture_t *texture = new texture_t();
    texture_data_t textureData;
    if (image)
    {
        textureData.image = *image;
        textureData.width = image->width;
        textureData.height = image->height;

        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        
        samplerInfo.addressModeU = repeat ? VK_SAMPLER_ADDRESS_MODE_REPEAT : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerInfo.addressModeV = samplerInfo.addressModeU;
        samplerInfo.addressModeW = samplerInfo.addressModeU;
        samplerInfo.anisotropyEnable = shadow ? VK_FALSE : VK_TRUE;
        samplerInfo.maxAnisotropy = shadow ? 1 : 16;
        samplerInfo.borderColor = shadow ? VK_BORDER_COLOR_INT_OPAQUE_WHITE : VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = shadow ? VK_TRUE : VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        if (vkCreateSampler(descriptorSet->device, &samplerInfo, nullptr, &texture->sampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }
    texture->data.push_back(textureData);
    texture->cube = image->samples > 1;
    texture->combined = true;

    descriptor_texture_t newTexture;
    newTexture.texture = texture;
    newTexture.binding = binding;
    newTexture.vertex = vertex;
    newTexture.shadow = repeat;
    newTexture.shadow = shadow;
    newTexture.fromImage = true;

    descriptorSet->textures.push_back(newTexture);
}

void descriptor_set_update_buffer(descriptor_set_t *descriptorSet, void *data, uint32_t binding)
{
    for (auto it = descriptorSet->buffers.begin(); it != descriptorSet->buffers.end(); ++it)
    {
        if (it->binding == binding)
        {
            void *target;
            vkMapMemory(descriptorSet->device, it->uniformBufferMemory, 0, it->size, 0, &target);
            memcpy(target, data, it->size);
            vkUnmapMemory(descriptorSet->device, it->uniformBufferMemory);
        }
    }
}

void descriptor_set_update_texture(descriptor_set_t *descriptorSet, texture_t *texture, uint32_t binding)
{
    std::vector<VkDescriptorImageInfo> imageInfos;
    for (auto it = texture->data.begin(); it != texture->data.end(); ++it)
    {
        VkDescriptorImageInfo imageInfo;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = it->image.view;
        imageInfo.sampler = texture->combined ? texture->sampler : VK_NULL_HANDLE;
        imageInfos.push_back(imageInfo);
    }

    VkWriteDescriptorSet write;
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = descriptorSet->descriptorSet;
    write.dstBinding = texture->combined ? binding : binding + 1;
    write.dstArrayElement = 0;
    write.descriptorType = texture->combined ? VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER : VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    write.descriptorCount = imageInfos.size();
    write.pImageInfo = imageInfos.data();
    write.pNext = NULL;

    vkUpdateDescriptorSets(descriptorSet->device, 1, &write, 0, nullptr);
}

void descriptor_set_update_image(descriptor_set_t *descriptorSet, image_t *image, uint32_t binding)
{
    for (auto it = descriptorSet->textures.begin(); it != descriptorSet->textures.end(); ++it)
    {
        if (it->binding == binding)
        {
            vkDestroySampler(descriptorSet->device, it->texture->sampler, nullptr);

            it->texture->data[0].image = *image;
            it->texture->data[0].width = image->width;
            it->texture->data[0].height = image->height;

            VkSamplerCreateInfo samplerInfo = {};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter = VK_FILTER_LINEAR;
            samplerInfo.minFilter = VK_FILTER_LINEAR;
            
            samplerInfo.addressModeU = it->repeat ? VK_SAMPLER_ADDRESS_MODE_REPEAT : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerInfo.addressModeV = samplerInfo.addressModeU;
            samplerInfo.addressModeW = samplerInfo.addressModeU;
            samplerInfo.anisotropyEnable = it->shadow ? VK_FALSE : VK_TRUE;
            samplerInfo.maxAnisotropy = it->shadow ? 1 : 16;
            samplerInfo.borderColor = it->shadow ? VK_BORDER_COLOR_INT_OPAQUE_WHITE : VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            samplerInfo.unnormalizedCoordinates = VK_FALSE;
            samplerInfo.compareEnable = it->shadow ? VK_TRUE : VK_FALSE;
            samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            samplerInfo.mipLodBias = 0.0f;
            samplerInfo.minLod = 0.0f;
            samplerInfo.maxLod = 0.0f;

            if (vkCreateSampler(descriptorSet->device, &samplerInfo, nullptr, &it->texture->sampler) != VK_SUCCESS) {
                throw std::runtime_error("failed to create texture sampler!");
            }

            VkDescriptorImageInfo imageInfo;
            imageInfo.imageLayout = it->shadow ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = it->texture->data[0].image.view;
            imageInfo.sampler = it->texture->sampler;

            VkWriteDescriptorSet write;
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = descriptorSet->descriptorSet;
            write.dstBinding = binding;
            write.dstArrayElement = 0;
            write.descriptorType = it->texture->combined ? VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER : VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            write.descriptorCount = 1;
            write.pImageInfo = &imageInfo;
            write.pNext = NULL;

            vkUpdateDescriptorSets(descriptorSet->device, 1, &write, 0, nullptr);
        }
    }
}


void descriptor_set_create(descriptor_set_t *descriptorSet)
{
    descriptor_set_create_pool(descriptorSet);
    descriptor_set_create_layout(descriptorSet);
    descriptor_set_create_descriptor_set(descriptorSet);
}

void descriptor_set_cleanup(descriptor_set_t *descriptorSet)
{
    vkDestroyDescriptorSetLayout(descriptorSet->device, descriptorSet->descriptorSetLayout, nullptr);
    vkDestroyDescriptorPool(descriptorSet->device, descriptorSet->descriptorPool, nullptr);
    descriptorSet->descriptorSetLayout = nullptr;
    descriptorSet->descriptorPool = nullptr;

    for (auto it = descriptorSet->buffers.begin(); it != descriptorSet->buffers.end(); ++it)
    {
        vkDestroyBuffer(descriptorSet->device, it->uniformBuffer, nullptr);
        vkFreeMemory(descriptorSet->device, it->uniformBufferMemory, nullptr);
    }
    descriptorSet->buffers.clear();

    for (auto it = descriptorSet->textures.begin(); it != descriptorSet->textures.end(); ++it)
    {
        if (it->fromImage)
        {
            vkDestroySampler(descriptorSet->device, it->texture->sampler, nullptr);
            delete it->texture;
        }
    }
    descriptorSet->textures.clear();
}

