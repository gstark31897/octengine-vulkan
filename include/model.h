#ifndef H_MODEL
#define H_MODEL

#include <array>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct vertex_t
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec4 weights;
    glm::ivec4 bones;
    int32_t boneCount = 0;
};

VkVertexInputBindingDescription vertex_get_binding_description();
VkVertexInputBindingDescription instance_get_binding_description();
std::array<VkVertexInputAttributeDescription, 9> vertex_get_attribute_descriptions();


struct scale_keyframe_t
{
    aiVector3D vec;
    float time;
};

struct rotation_keyframe_t
{
    aiQuaternion quat;
    float time;
};

struct position_keyframe_t
{
    aiVector3D vec;
    float time;
};

struct bone_t
{
    aiMatrix4x4 matrix;
    aiMatrix4x4 offset;
    std::string name;
    std::vector<scale_keyframe_t> scaleKeyframes;
    std::vector<rotation_keyframe_t> rotationKeyframes;
    std::vector<position_keyframe_t> positionKeyframes;
    int32_t pos = 0;
    std::vector<bone_t*> children;
};

struct model_instance_t {
    glm::vec3 pos;
    glm::vec3 rot;
    float scale;
    uint32_t textureIdx;
};


struct model_t
{
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    VkBuffer instanceBuffer;
    VkDeviceMemory instanceBufferMemory;
    VkBuffer uniformBuffer;
    VkDeviceMemory uniformBufferMemory;

    VkDescriptorSet descriptorSet;
    VkDescriptorSetLayout descriptorSetLayout;

    VkDescriptorSet instanceDescriptorSet;
    VkDescriptorSetLayout instanceDescriptorSetLayout;

    std::vector<vertex_t> vertices;
    std::vector<uint16_t> indices;
    std::vector<bone_t> bones;
    std::vector<model_instance_t> instances;

    aiMatrix4x4 globalInverseTransform;

    float time = 0.0f;
};

aiMatrix4x4 interpolate_scale(bone_t *bone, float time);
aiMatrix4x4 interpolate_rotation(bone_t *bone, float time);
aiMatrix4x4 interpolate_position(bone_t *bone, float time);

void model_load(model_t *model, std::string path);
void model_create_buffers(model_t *model, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue);
void model_update(model_t *model, float delta);
void model_render(model_t *model, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, VkPipeline graphicsPipeline, VkDescriptorSet descriptorSet);
void model_cleanup(model_t *model, VkDevice device);

#endif