#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "luaheaders.h"
#include "export.h"
#include "gameobject.h"


extern "C"
{

static int libgameobject_transform(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);
    float z = lua_tonumber(L, 4);
    object->pos += glm::vec3(x, y, z);
    return 0;
}

static int libgameobject_set_transform(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);
    float z = lua_tonumber(L, 4);
    object->pos = glm::vec3(x, y, z);
    return 0;
}

static int libgameobject_get_transform(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    lua_pushnumber(L, object->pos.x);
    lua_pushnumber(L, object->pos.y);
    lua_pushnumber(L, object->pos.z);
    return 3;
}

static int libgameobject_get_global_transform(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    lua_pushnumber(L, object->globalPos.x);
    lua_pushnumber(L, object->globalPos.y);
    lua_pushnumber(L, object->globalPos.z);
    return 3;
}

static int libgameobject_rotate(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);
    float z = lua_tonumber(L, 4);
    object->rot += glm::vec3(x, y, z);
    return 0;
}

static int libgameobject_set_rotation(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);
    float z = lua_tonumber(L, 4);
    object->rot = glm::vec3(x, y, z);
    return 0;
}

static int libgameobject_get_rotation(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    lua_pushnumber(L, (lua_Number)object->rot.x);
    lua_pushnumber(L, (lua_Number)object->rot.y);
    lua_pushnumber(L, (lua_Number)object->rot.z);
    return 3;
}

static int libgameobject_get_global_rotation(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    lua_pushnumber(L, object->globalRot.x);
    lua_pushnumber(L, object->globalRot.y);
    lua_pushnumber(L, object->globalRot.z);
    return 3;
}


static int libgameobject_scale(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);
    float z = lua_tonumber(L, 4);
    object->scale.x *= x;
    object->scale.y *= y;
    object->scale.z *= z;
    return 0;
}

static int libgameobject_set_scale(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);
    float z = lua_tonumber(L, 4);
    object->scale = glm::vec3(x, y, z);
    return 0;
}


static int libgameobject_set_parent(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    gameobject_t *parent = (gameobject_t*)lua_tointeger(L, 2);
    object->parent = (gameobject_t*)parent;
    return 0;
}

static int libgameobject_get_parent(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    lua_pushinteger(L, (lua_Integer)object->parent);
    return 1;
}


static int libgameobject_set_string(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    const char *name = lua_tostring(L, 2);
    const char *value = lua_tostring(L, 3);
    object->strings[name] = std::string(value);
    return 0;
}


static int libgameobject_get_string(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    const char *name = lua_tostring(L, 2);
    std::string value = object->strings[name];
    lua_pushstring(L, value.c_str());
    return 1;
}


static int libgameobject_set_integer(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    const char *name = lua_tostring(L, 2);
    lua_Integer value = lua_tointeger(L, 3);
    object->integers[name] = value;
    return 0;
}

static int libgameobject_get_integer(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    const char *name = lua_tostring(L, 2);
    lua_Integer value = object->integers[name];
    lua_pushinteger(L, value);
    return 1;
}


static int libgameobject_set_number(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    const char *name = lua_tostring(L, 2);
    lua_Number value = lua_tonumber(L, 3);
    object->numbers[name] = value;
    return 0;
}

static int libgameobject_get_number(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    const char *name = lua_tostring(L, 2);
    lua_Number value = object->numbers[name];
    lua_pushnumber(L, value);
    return 1;
}


static int libgameobject_rotate_vector(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);
    float z = lua_tonumber(L, 4);

    glm::vec3 output = gameobject_rotate_vector(object, glm::vec3(x, y, z));

    lua_pushnumber(L, output.x);
    lua_pushnumber(L, output.y);
    lua_pushnumber(L, output.z);
    return 3;
}

static int libgameobject_unrotate_vector(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);
    float z = lua_tonumber(L, 4);

    glm::vec3 output = gameobject_unrotate_vector(object, glm::vec3(x, y, z));

    lua_pushnumber(L, output.x);
    lua_pushnumber(L, output.y);
    lua_pushnumber(L, output.z);
    return 3;
}


static int libgameobject_look_at(lua_State *L)
{
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 1);
    float xt = lua_tonumber(L, 2);
    float yt = lua_tonumber(L, 3);
    float zt = lua_tonumber(L, 4);

    glm::vec3 target(xt, yt, zt);
    float dx = target.x - object->pos.x;
    float dy = target.y - object->pos.y;
    float dz = target.z - object->pos.z;
    float dist = sqrt(dx * dx + dy * dy + dz * dz);

    object->rot.y = asin(dx/dist);
    object->rot.x = asin(dy/dist);
    return 0;
}


int EXPORT luaopen_gameobject(lua_State *L)
{
    lua_register(L, "gameobject_transform", libgameobject_transform);
    lua_register(L, "gameobject_set_transform", libgameobject_set_transform);
    lua_register(L, "gameobject_get_transform", libgameobject_get_transform);
    lua_register(L, "gameobject_get_global_transform", libgameobject_get_global_transform);
    lua_register(L, "gameobject_rotate", libgameobject_rotate);
    lua_register(L, "gameobject_set_rotation", libgameobject_set_rotation);
    lua_register(L, "gameobject_get_rotation", libgameobject_get_rotation);
    lua_register(L, "gameobject_get_global_rotation", libgameobject_get_global_rotation);
    lua_register(L, "gameobject_scale", libgameobject_scale);
    lua_register(L, "gameobject_set_scale", libgameobject_set_scale);
    lua_register(L, "gameobject_set_parent", libgameobject_set_parent);
    lua_register(L, "gameobject_get_parent", libgameobject_get_parent);
    lua_register(L, "gameobject_set_string", libgameobject_set_string);
    lua_register(L, "gameobject_get_string", libgameobject_get_string);
    lua_register(L, "gameobject_set_integer", libgameobject_set_integer);
    lua_register(L, "gameobject_get_integer", libgameobject_get_integer);
    lua_register(L, "gameobject_set_number", libgameobject_set_number);
    lua_register(L, "gameobject_get_number", libgameobject_get_number);
    lua_register(L, "gameobject_rotate_vector", libgameobject_rotate_vector);
    lua_register(L, "gameobject_unrotate_vector", libgameobject_unrotate_vector);
    lua_register(L, "gameobject_look_at", libgameobject_look_at);
    return 0;
}

}

