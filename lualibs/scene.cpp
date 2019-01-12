#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "luaheaders.h"
#include "export.h"
#include "scene.h"


extern "C"
{

static int libscene_add_gameobject(lua_State *L)
{
    scene_t *scene = (scene_t*)lua_tointeger(L, 1);
    gameobject_t *object = scene_add_gameobject(scene);
    lua_pushinteger(L, (lua_Integer)object);
    return 1;
}

static int libscene_add_camera(lua_State *L)
{
    scene_t *scene = (scene_t*)lua_tointeger(L, 1);
    camera_t *camera = scene_add_camera(scene);
    lua_pushinteger(L, (lua_Integer)camera);
    return 1;
}

static int libscene_add_light(lua_State *L)
{
    scene_t *scene = (scene_t*)lua_tointeger(L, 1);
    light_t *light = scene_add_light(scene);
    lua_pushinteger(L, (lua_Integer)light);
    return 1;
}

static int libscene_enable_shadow(lua_State *L)
{
    scene_t *scene = (scene_t*)lua_tointeger(L, 1);
    light_t *light = (light_t*)lua_tointeger(L, 2);
    scene_enable_shadow(scene, light);
    return 0;
}

static int libscene_add_script(lua_State *L)
{
    scene_t *scene = (scene_t*)lua_tointeger(L, 1);
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 2);
    const char *name = lua_tostring(L, 3);
    scene_add_script(scene, object, name);
    return 0;
}

static int libscene_set_camera(lua_State *L)
{
    scene_t *scene = (scene_t*)lua_tointeger(L, 1);
    camera_t *camera = (camera_t*)lua_tointeger(L, 2);
    scene->camera = camera;
    return 0;
}

static int libscene_set_model(lua_State *L)
{
    scene_t *scene = (scene_t*)lua_tointeger(L, 1);
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 2);
    const char *model = lua_tostring(L, 3);
    scene_set_model(scene, object, model);
    return 0;
}

static int libscene_set_texture(lua_State *L)
{
    scene_t *scene = (scene_t*)lua_tointeger(L, 1);
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 2);
    const char *texture = lua_tostring(L, 3);
    scene_set_texture(scene, object, texture);
    return 0;
}

static int libscene_set_normal(lua_State *L)
{
    scene_t *scene = (scene_t*)lua_tointeger(L, 1);
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 2);
    const char *texture = lua_tostring(L, 3);
    scene_set_normal(scene, object, texture);
    return 0;
}

static int libscene_set_pbr(lua_State *L)
{
    scene_t *scene = (scene_t*)lua_tointeger(L, 1);
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 2);
    const char *texture = lua_tostring(L, 3);
    scene_set_pbr(scene, object, texture);
    return 0;
}

static int libscene_send_event(lua_State *L)
{
    scene_t *scene = (scene_t*)lua_tointeger(L, 1);
    gameobject_t *object = (gameobject_t*)lua_tointeger(L, 2);
    const char *event = lua_tostring(L, 3);
    float value = lua_tonumber(L, 4);

    gameobject_on_event(object, scene, {std::string(event), value});
    return 0;
}


int EXPORT luaopen_scene(lua_State *L)
{
    lua_register(L, "scene_add_gameobject", libscene_add_gameobject);
    lua_register(L, "scene_add_camera", libscene_add_camera);
    lua_register(L, "scene_add_light", libscene_add_light);
    lua_register(L, "scene_enable_shadow", libscene_enable_shadow);
    lua_register(L, "scene_add_script", libscene_add_script);
    lua_register(L, "scene_set_camera", libscene_set_camera);
    lua_register(L, "scene_set_model", libscene_set_model);
    lua_register(L, "scene_set_texture", libscene_set_texture);
    lua_register(L, "scene_set_normal", libscene_set_normal);
    lua_register(L, "scene_set_pbr", libscene_set_pbr);
    lua_register(L, "scene_send_event", libscene_send_event);
    return 0;
}

}

