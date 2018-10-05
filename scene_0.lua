require("scene")
require("gameobject")
require("light")
require("camera")
require("physics")

function setup(scene, object)
    player = scene_add_gameobject(scene)
    scene_add_script(scene, player, "player.lua")
    physics_set_position(player, 0, 10, 5)

    --for x = 0, 10 do
    --    for y = 0, 10 do
    --      for i = 1, 2 do
    --        cube = scene_add_gameobject(scene)
    --        scene_set_model(scene, cube, "scifi_cube.dae")
    --        scene_set_texture(scene, cube, "default.png")
    --        scene_set_normal(scene, cube, "normal.png")
    --        gameobject_scale(cube, 1)
    --        gameobject_transform(cube, x*1, i, y*1)
    --        physics_init_box(scene, cube, 10, 0.5, 0.5, 0.5)
    --      end
    --    end
    --end

    sphere = scene_add_gameobject(scene)
    scene_add_script(scene, sphere, "test.lua")
    gameobject_transform(sphere, -5, 1, 0)

    plane = scene_add_gameobject(scene)
    scene_set_model(scene, plane, "plane.dae")
    gameobject_transform(plane, 0, 0, 0)
    gameobject_scale(plane, 50)
    physics_init_box(scene, plane, 0, 50, 0.01, 50)

    example = scene_add_gameobject(scene)
    scene_set_model(scene, example, "example.dae")
    gameobject_rotate(example, math.pi, 0, 0)
    gameobject_transform(example, 0, 0.5, 0)
    gameobject_scale(example, 0.5)

    sun = scene_add_light(scene)
    light_set_color(sun, 1.0, 1.0, 1.0)
    sun_cam = light_get_camera(sun)
    camera_set_fov(sun_cam, -1.0)
    sun_obj = camera_get_gameobject(sun_cam)
    gameobject_transform(sun_obj, 10, 10, 10)
    gameobject_rotate(sun_obj, -0.7853982, 0.7853982, 0)
end
