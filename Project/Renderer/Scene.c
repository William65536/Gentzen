#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "..\includes\LinearAlgebra.h"
#include "Solid.h"
#define T Solid
#include "..\includes\List.h"
typedef struct Light Light;
#define T Light
#include "..\includes\List.h"

#define T int
#include "..\includes\List.h"

typedef struct Camera Camera;
struct Camera {
    struct { Mat4 to, from; } scene_transform;
};

/** TODO: Allow for the use of custom allocators */

Camera *Camera_new(Pos3 pos, Dim3 dim, Rot3 rot)
{
    Camera *ret = malloc(sizeof *ret);

    if (ret == NULL)
        return NULL;

    Mat4_make_transformation(ret->scene_transform.to, pos, dim, rot);

    memcpy(ret->scene_transform.from, ret->scene_transform.to, sizeof ret->scene_transform.from);
    Mat4_invert(ret->scene_transform.from);

    return ret;
}

void Camera_delete(Camera **self)
{
    assert(self != NULL && *self != NULL);

    free(*self);
    *self = NULL;
}

struct Light {
    struct { Mat4 to, from; } scene_transform;
};

Light *Light_new(Pos3 pos, Dim3 dim, Rot3 rot)
{
    Light *ret = malloc(sizeof *ret);

    if (ret == NULL)
        return NULL;

    Mat4_make_transformation(ret->scene_transform.to, pos, dim, rot);

    memcpy(ret->scene_transform.from, ret->scene_transform.to, sizeof ret->scene_transform.from);
    Mat4_invert(ret->scene_transform.from);

    return ret;
}

void Light_delete(Light **self)
{
    assert(self != NULL && *self != NULL);

    free(*self);
    *self = NULL;
}

typedef struct Scene Scene;
struct Scene {
    Camera *camera;
    SolidList *solids;
    LightList *lights;
};

Scene *Scene_new(Camera *camera, SolidList *solids, LightList *lights)
{
    assert(camera != NULL);
    assert(solids != NULL);
    assert(lights != NULL);

    Scene *ret = malloc(sizeof *ret);

    if (ret == NULL)
        return NULL;

    ret->camera = camera;
    ret->solids = solids;
    ret->lights = lights;

    return ret;
}

void Scene_delete(Scene **self)
{
    assert(self != NULL && *self != NULL);

    Camera_delete(&(*self)->camera);
    SolidList_delete(&(*self)->solids);
    LightList_delete(&(*self)->lights);

    free(*self);
    *self = NULL;
}

#undef T
