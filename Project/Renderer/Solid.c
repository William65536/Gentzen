#include <stdlib.h>
#include <assert.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <setjmp.h>

#include "..\includes\Image.h"
#include "..\includes\LinearAlgebra.h"
#include "Solid.h"

#define T Interval
#include "..\includes\List.h"

/** TODO: Allow blocks */
static void try(bool expr, jmp_buf envbuf, int errorcode)
{
    if (!expr)
        longjmp(envbuf, errorcode);
}

typedef struct Luminosity {
    Vec3 normal;
    IntervalList **intervals;
} Luminosity;

/** TODO: Make this take in an array of length 3 */
typedef bool LuminosityCalculation(const Solid *self, Ray ray, Luminosity *scratch, Luminosity *left, Luminosity *right, jmp_buf envbuf);

struct Solid {
    union {
        struct { Mat4 to, from; } scene_transform;
        struct { Solid *left, *right; }; /** TODO: Make self support structure-sharing, probably by using a linear allocator */
    };
    LuminosityCalculation *calculate_luminosity;
};

/** TODO: Make deletion functions */

static Solid *Solid_primitive_new(Pos3 pos, Dim3 dim, Rot3 rot, LuminosityCalculation *primitive_luminosity)
{
    assert(primitive_luminosity != NULL);

    Solid *ret = malloc(sizeof *ret);

    if (ret == NULL)
        return NULL;

    Mat4_make_transformation(ret->scene_transform.to, pos, dim, rot);

    memcpy(ret->scene_transform.from, ret->scene_transform.to, sizeof ret->scene_transform.from);
    Mat4_invert(ret->scene_transform.from);

    ret->calculate_luminosity = primitive_luminosity;

    return ret;
}

#if 0
/** TODO: Allow for whole tranformations on composite structures */
static Solid *Solid_composite_new(Solid *left, Solid *right, LuminosityCalculation *composite_luminosity)
{
    assert(left != NULL);
    assert(right != NULL);
    assert(composite_luminosity);

    Solid *ret = malloc(sizeof *ret);

    if (ret == NULL)
        return NULL;

    ret->left = left;
    ret->right = right;
    ret->calculate_luminosity = composite_luminosity;

    return ret;
}
#endif

/** TODO: Fix the issues with rendering */
/** TODO: Make lighting distance-based */

static bool Ellipsoid_luminosity(const Solid *self, Ray ray, Luminosity *scratch, Luminosity *left, Luminosity *right, jmp_buf envbuf)
{
    (void) left, (void) right;

    assert(self != NULL);
    assert(self->scene_transform.from != NULL);
    assert(self->scene_transform.to != NULL);
    assert(scratch != NULL);
    assert(scratch->intervals != NULL && *scratch->intervals != NULL);
    assert(envbuf != NULL);

    IntervalList_clear(*scratch->intervals);

    const Ray tray = Mat4_mult_Ray(self->scene_transform.from, ray);

    const double a = tray.dpos.x * tray.dpos.x + tray.dpos.y * tray.dpos.y + tray.dpos.z * tray.dpos.z;
    const double b = 2.0 * (tray.pos.x * tray.dpos.x + tray.pos.y * tray.dpos.y + tray.pos.z * tray.dpos.z);
    const double c = tray.pos.x * tray.pos.x + tray.pos.y * tray.pos.y + tray.pos.z * tray.pos.z - 1.0;

    if (b * b - 4.0 * a * c < 0.0)
        return false;

    const Interval interval = {
        .left = (-b - sqrt(b * b - 4.0 * a * c)) / (2.0 * a),
        .right = (-b + sqrt(b * b - 4.0 * a * c)) / (2.0 * a)
    };

    /** TODO: Find further restrictions---reimplement the ones you've already made */

    const struct { Vec3 left; Vec3 right; } intersectionpoints = {
        .left = {
            .x = tray.pos.x + tray.dpos.x * interval.left,
            .y = tray.pos.y + tray.dpos.y * interval.left,
            .z = tray.pos.z + tray.dpos.z * interval.left
        },
        .right = {
            .x = tray.pos.x + tray.dpos.x * interval.right,
            .y = tray.pos.y + tray.dpos.y * interval.right,
            .z = tray.pos.z + tray.dpos.z * interval.right
        }
    };

    const Vec3 normal = intersectionpoints.left;

    const Vec3 tnormal = Mat4_mult_Raytip(self->scene_transform.to, normal);

    const Interval tinterval = {
        .left = Vec3_mag(Mat4_mult_Vec3(self->scene_transform.to, intersectionpoints.left)),
        .right = Vec3_mag(Mat4_mult_Vec3(self->scene_transform.to, intersectionpoints.right))
    };

    scratch->normal = tnormal;

    if (!IntervalList_push(scratch->intervals, tinterval))
        longjmp(envbuf, 1);

    return true;
}

#if 0

Luminosity Solid_cuboid_luminosity(const Solid *self, Ray ray)
{
    assert(self != NULL);
    assert(self->scene_transform.from != NULL);
    assert(self->scene_transform.to != NULL);

    const Ray tray = Mat4_mult_Ray(self->scene_transform.from, ray);

    Vec3 normal;

    double closestintersection = INFINITY;

    /* x == 0 && 0 <= y <= 1 && 0 <= z <= 1 */
    if (-tray.pos.x / tray.dpos.x >= 0.0) {
        const double a = tray.pos.y + tray.dpos.y * -tray.pos.x / tray.dpos.x;
        const double b = tray.pos.z + tray.dpos.z * -tray.pos.x / tray.dpos.x;

        if (0.0 <= a && a <= 1.0 && 0.0 <= b && b <= 1.0) {
            closestintersection = -tray.pos.x / tray.dpos.x;
            normal = (Vec3) { .x = -1.0, .y = 0.0, .z = 0.0 };
        }
    }

    /* x == 1 && 0 <= y <= 1 && 0 <= z <= 1 */
    if ((1.0 - tray.pos.x) / tray.dpos.x >= 0.0) {
        const double a = tray.pos.y + tray.dpos.y * (1.0 - tray.pos.x) / tray.dpos.x;
        const double b = tray.pos.z + tray.dpos.z * (1.0 - tray.pos.x) / tray.dpos.x;

        const double currentintersection = (1.0 - tray.pos.x) / tray.dpos.x;

        if (0.0 <= a && a <= 1.0 && 0.0 <= b && b <= 1.0 && (isinf(closestintersection) || currentintersection < closestintersection)) {
            closestintersection = currentintersection;
            normal = (Vec3) { .x = 1.0, .y = 0.0, .z = 0.0 };
        }
    }

    /*  0 <= x <= 1 && y == 0 && 0 <= z <= 1 */
    if (-tray.pos.y / tray.dpos.y >= 0.0) {
        const double a = tray.pos.x + tray.dpos.x * -tray.pos.y / tray.dpos.y;
        const double b = tray.pos.z + tray.dpos.z * -tray.pos.y / tray.dpos.y;

        const double currentintersection = -tray.pos.y / tray.dpos.y;

        if (0.0 <= a && a <= 1.0 && 0.0 <= b && b <= 1.0 && (isinf(closestintersection) || currentintersection < closestintersection)) {
            closestintersection = currentintersection;
            normal = (Vec3) { .x = 0.0, .y = -1.0, .z = 0.0 };
        }
    }

    /*  0 <= x <= 1 && y == 1 && 0 <= z <= 1 */
    if ((1.0 - tray.pos.y) / tray.dpos.y >= 0.0) {
        const double a = tray.pos.x + tray.dpos.x * (1.0 - tray.pos.y) / tray.dpos.y;
        const double b = tray.pos.z + tray.dpos.z * (1.0 - tray.pos.y) / tray.dpos.y;

        const double currentintersection = (1.0 - tray.pos.y) / tray.dpos.y;

        if (0.0 <= a && a <= 1.0 && 0.0 <= b && b <= 1.0 && (isinf(closestintersection) || currentintersection < closestintersection)) {
            closestintersection = currentintersection;
            normal = (Vec3) { .x = 0.0, .y = 1.0, .z = 0.0 };
        }
    }

    /*  0 <= x <= 1 && 0 <= y <= 1 && z == 0 */
    if (-tray.pos.z / tray.dpos.z >= 0.0) {
        const double a = tray.pos.x + tray.dpos.x * -tray.pos.z / tray.dpos.z;
        const double b = tray.pos.y + tray.dpos.y * -tray.pos.z / tray.dpos.z;

        const double currentintersection = -tray.pos.z / tray.dpos.z;

        if (0.0 <= a && a <= 1.0 && 0.0 <= b && b <= 1.0 && (isinf(closestintersection) || currentintersection < closestintersection)) {
            closestintersection = currentintersection;
            normal = (Vec3) { .x = 0.0, .y = 0.0, .z = -1.0 };
        }
    }

    /*  0 <= x <= 1 && 0 <= y <= 1 && z == 1 */
    if ((1.0 - tray.pos.z) / tray.dpos.z >= 0.0) {
        const double a = tray.pos.x + tray.dpos.x * (1.0 - tray.pos.z) / tray.dpos.z;
        const double b = tray.pos.y + tray.dpos.y * (1.0 - tray.pos.z) / tray.dpos.z;

        const double currentintersection = (1.0 - tray.pos.z) / tray.dpos.z;

        if (0.0 <= a && a <= 1.0 && 0.0 <= b && b <= 1.0 && (isinf(closestintersection) || currentintersection < closestintersection)) {
            closestintersection = currentintersection;
            normal = (Vec3) { .x = 0.0, .y = 0.0, .z = 1.0 };
        }
    }

    // compute_luminosity: /* Make the luminosity calculuation be distance-based (inverse square law) */

    if (isinf(closestintersection))
        return (Luminosity) { .distance = INFINITY };

    const Vec3 tnormal = Mat4_mult_Raytip(self->scene_transform.to, normal);

    const double angle = acos(Vec3_dot(ray.dpos, tnormal) / (Vec3_mag(ray.dpos) * Vec3_mag(tnormal)));

    const Vec3 intersectionpoint = {
        .x = tray.pos.x + tray.dpos.x * closestintersection,
        .y = tray.pos.y + tray.dpos.y * closestintersection,
        .z = tray.pos.z + tray.dpos.z * closestintersection
    };

    return (Luminosity) { .distance = Vec3_mag(Mat4_mult_Raytip(self->scene_transform.to, intersectionpoint)), .angle = angle / M_PI };
}

Luminosity Solid_cylinder_luminosity(const Solid *self, Ray ray)
{
    assert(self != NULL);

    const Ray tray = Mat4_mult_Ray(self->scene_transform.from, ray);

    Vec3 normal;

    double closestintersection = INFINITY;

    /* z == 0 && x * x + y * y <= 1 */
    if (-tray.pos.z / tray.dpos.z >= 0.0) {
        const double a = tray.pos.x + tray.dpos.x * -tray.pos.z / tray.dpos.z;
        const double b = tray.pos.y + tray.dpos.y * -tray.pos.z / tray.dpos.z;

        if (a * a + b * b <= 1.0) {
            closestintersection = -tray.pos.z / tray.dpos.z;
            normal = (Vec3) { .x = 0.0, .y = 0.0, .z = -1.0 };
        }
    }

    /* z == 1 && x * x + y * y <= 1 */
    if ((1.0 - tray.pos.z) / tray.dpos.z >= 0.0) {
        const double a = tray.pos.x + tray.dpos.x * (1.0 - tray.pos.z) / tray.dpos.z;
        const double b = tray.pos.y + tray.dpos.y * (1.0 - tray.pos.z) / tray.dpos.z;

        const double currentintersection = (1.0 - tray.pos.z) / tray.dpos.z;

        if (a * a + b * b <= 1.0 && (isinf(closestintersection) || currentintersection < closestintersection)) {
            closestintersection = currentintersection;
            normal = (Vec3) { .x = 0.0, .y = 0.0, .z = 1.0 };
        }
    }

    /* 0 < z < 1 && x * x + y * y == 1 */
    {
        const double a = tray.dpos.x * tray.dpos.x + tray.dpos.y * tray.dpos.y;
        const double b = 2.0 * (tray.pos.x * tray.dpos.x + tray.pos.y * tray.dpos.y);
        const double c = tray.pos.x * tray.pos.x + tray.pos.y * tray.pos.y - 1.0;

        if (b * b - 4.0 * a * c >= 0.0) {
            const double currentintersection = (-b - sqrt(b * b - 4.0 * a * c)) / (2.0 * a); /** TODO: Ensure that self is necessarily the closer of the roots, I think it is */

            if (currentintersection >= 0.0) {
                const double z = tray.pos.z + tray.dpos.z * currentintersection;

                if (0.0 < z && z < 1.0 && (isinf(closestintersection) || currentintersection < closestintersection)) {
                    closestintersection = currentintersection;
                    normal = (Vec3) {
                        .x = tray.pos.x + tray.dpos.x * closestintersection,
                        .y = tray.pos.y + tray.dpos.y * closestintersection,
                        .z = 0.0
                    };
                }
            }
        }
    }

    if (isinf(closestintersection))
        return (Luminosity) { .distance = INFINITY };

    const Vec3 tnormal = Mat4_mult_Raytip(self->scene_transform.to, normal);

    const double angle = acos(Vec3_dot(ray.dpos, tnormal) / (Vec3_mag(ray.dpos) * Vec3_mag(tnormal)));

    const Vec3 intersectionpoint = {
        .x = tray.pos.x + tray.dpos.x * closestintersection,
        .y = tray.pos.y + tray.dpos.y * closestintersection,
        .z = tray.pos.z + tray.dpos.z * closestintersection
    };

    return (Luminosity) { .distance = Vec3_mag(Mat4_mult_Raytip(self->scene_transform.to, intersectionpoint)), .angle = angle / M_PI };
}

/** TODO: Double-check self, I think the color is off */
Luminosity Solid_cone_luminosity(const Solid *self, Ray ray)
{
    assert(self != NULL);
    assert(self->scene_transform.from != NULL);
    assert(self->scene_transform.to != NULL);

    const Ray tray = Mat4_mult_Ray(self->scene_transform.from, ray);

    Vec3 normal;

    double closestintersection = INFINITY;

    /* z == 0 */
    if (tray.pos.z / tray.dpos.z >= 0)
        return (Luminosity) { .distance = INFINITY };

    /* z == 1 && x * x + y * y == 1 */
    if ((1.0 - tray.pos.z) / tray.dpos.z >= 0) {
        const double a = tray.pos.x + tray.dpos.x * (1.0 - tray.pos.z) / tray.dpos.z;
        const double b = tray.pos.y + tray.dpos.y * (1.0 - tray.pos.z) / tray.dpos.z;

        if (a * a + b * b <= 1.0) {
            closestintersection = (1.0 - tray.pos.z) / tray.dpos.z;
            normal = (Vec3) { .x = 0.0, .y = 0.0, .z = 1.0 };
        }
    }

    /* 0 < z < 1 && x * x + y * y == z * z */
    {
        const double a = tray.dpos.x * tray.dpos.x + tray.dpos.y * tray.dpos.y - tray.dpos.z * tray.dpos.z;
        const double b = 2.0 * (tray.pos.x * tray.dpos.x + tray.pos.y * tray.dpos.y - tray.pos.z * tray.dpos.z);
        const double c = tray.pos.x * tray.pos.x + tray.pos.y * tray.pos.y - tray.pos.z * tray.pos.z;

        if (b * b - 4.0 * a * c >= 0) {
            const double t = (-b - sqrt(b * b - 4.0 * a * c)) / (2.0 * a);

            if (t >= 0.0 && 0.0 < tray.pos.z + tray.dpos.z * t && tray.pos.z + tray.dpos.z * t < 1.0 && (isinf(closestintersection) || t < closestintersection)) {
                closestintersection = t;
                normal = (Vec3) {
                    .x = M_SQRT2 / 2.0 * (tray.pos.x + tray.dpos.x * closestintersection),
                    .y = M_SQRT2 / 2.0 * (tray.pos.y + tray.dpos.y * closestintersection),
                    .z = -M_SQRT2 / 2.0
                };
            }
        }
    }

    if (isinf(closestintersection))
        return (Luminosity) { .distance = INFINITY};

    const Vec3 tnormal = Mat4_mult_Raytip(self->scene_transform.to, normal);

    const double angle = acos(Vec3_dot(ray.dpos, tnormal) / (Vec3_mag(ray.dpos) * Vec3_mag(tnormal)));

    const Vec3 intersectionpoint = {
        .x = tray.pos.x + tray.dpos.x * closestintersection,
        .y = tray.pos.y + tray.dpos.y * closestintersection,
        .z = tray.pos.z + tray.dpos.z * closestintersection
    };

    return (Luminosity) { .distance = Vec3_mag(Mat4_mult_Raytip(self->scene_transform.to, intersectionpoint)), .angle = angle / M_PI };
}

#endif

/** TODO: Allow for whole tranformations on composite structures */
static bool Union_luminosity(const Solid *self, Ray ray, Luminosity *scratch, Luminosity *left, Luminosity *right, jmp_buf envbuf)
{
    assert(self != NULL);
    assert(self->left != NULL);
    assert(self->right != NULL);
    assert(self->left->calculate_luminosity != NULL);
    assert(self->right->calculate_luminosity != NULL);
    assert(scratch != NULL);
    assert(scratch->intervals != NULL && *scratch->intervals != NULL);
    assert(left != NULL);
    assert(left->intervals != NULL && *left->intervals != NULL);
    assert(right != NULL);
    assert(right->intervals != NULL && *right->intervals != NULL);

    IntervalList_clear(*scratch->intervals);

    const bool isleft = self->left->calculate_luminosity(self->left, ray, left, NULL, NULL, envbuf);
    const bool isright = self->right->calculate_luminosity(self->right, ray, right, NULL, NULL, envbuf);

    if (!isleft && !isright)
        return false;

    /** TODO: Implement these as a pointer exchanges later on */
    if (!isleft) {
        try(IntervalList_copy(*right->intervals, scratch->intervals), envbuf, 1);
        return true;
    }

    if (!isright) {
        try(IntervalList_copy(*left->intervals, scratch->intervals), envbuf, 1);
        return true;
    }

    assert(IntervalList_size(*left->intervals) > 0);
    assert(IntervalList_size(*right->intervals) > 0);

    typedef struct { const Interval *itr, *end; } IntervalListView;

    const IntervalListView leftview = {
        .itr = IntervalList_begin(*left->intervals),
        .end = IntervalList_end(*left->intervals)
    };

    const IntervalListView rightview = {
        .itr = IntervalList_begin(*right->intervals),
        .end = IntervalList_end(*right->intervals)
    };

    IntervalListView views[2];

    if (leftview.itr->left < rightview.itr->right) {
        views[0] = (IntervalListView) { .itr = leftview.itr, .end = leftview.end };
        views[1] = (IntervalListView) { .itr = rightview.itr, .end = rightview.end };
    } else {
        views[0] = (IntervalListView) { .itr = rightview.itr, .end = rightview.end };
        views[1] = (IntervalListView) { .itr = leftview.itr, .end = leftview.end };
    }


    while (true) {
        Interval temp;

        temp.left = views[0].itr->left;

        for (; views[1].itr < views[1].end; views[1].itr++) {
            if (views[1].itr->left > views[0].itr->right) {
                temp.right = views[0].itr->right;

                IntervalListView tempview = views[0];
                views[0] = views[1];
                views[1] = tempview;
                break;
            }

            if (views[1].itr->right > views[0].)
        }
    }

    return true;
}

#if 0
Luminosity Solid_intersection_luminosity(const Solid *self, Ray ray)
{
    assert(self != NULL);
    assert(self->left != NULL);
    assert(self->right != NULL);
    assert(self->left->luminosity != NULL);
    assert(self->right->luminosity != NULL);

    const Luminosity leftluminosity = self->left->luminosity(self->left, ray);
    const Luminosity rightluminosity = self->right->luminosity(self->right, ray);

    if (isinf(leftluminosity.range.near) || isinf(rightluminosity.range.near))
        return (Luminosity) { .range = { .near = INFINITY } };

    const double near = fmax(leftluminosity.range.near, rightluminosity.range.near);
    const double far = fmax(leftluminosity.range.far, rightluminosity.range.far);

    return (Luminosity) { .range = { .near = near, .far = far }, .angle = near == leftluminosity.range.near ? leftluminosity.angle : rightluminosity.angle };
}

Luminosity Solid_difference_luminosity(const Solid *self, Ray ray)
{
    assert(self != NULL);
    assert(self->left != NULL);
    assert(self->right != NULL);
    assert(self->left->luminosity != NULL);
    assert(self->right->luminosity != NULL);

    const Luminosity leftluminosity = self->left->luminosity(self->left, ray);
    const Luminosity rightluminosity = self->right->luminosity(self->right, ray);

    if (isinf(leftluminosity.range.near))
        return (Luminosity) { .range = { .near = INFINITY } };

    if (isinf(rightluminosity.range.near))
        return leftluminosity;

    return ;
}
#endif

Ellipsoid *Ellipsoid_new(Pos3 pos, Dim3 dim, Rot3 rot)
{
    return Solid_primitive_new(pos, dim, rot, Ellipsoid_luminosity);
}

// Union *Union_new(Solid *left, Solid *right)
// {
//     assert(left != NULL);
//     assert(right != NULL);

//     return Solid_composite_new(left, right, Union_luminosity);
// }

/** TODO: Use `longjmp` instead of an errorcode */
bool Solid_render(const Solid *self, Image *image, Color backgroundcolor)
{
    assert(self != NULL);
    assert(image != NULL);
    assert(self->calculate_luminosity);

    /** TODO: Add clipping */

    /** TODO: Consolidate these into a `Camera` object */

    const double focallength = 1700.0; /** TODO: Test to find the best focal length */

    const Vec3 orig = { .x = 0.0, .y = 0.0, .z = -focallength };

    jmp_buf envbuf;

    Luminosity luminosities[3];
    IntervalList *interval_lists[3];

    {
        {
            interval_lists[0] = IntervalList_new(10);

            if (interval_lists[0] == NULL)
                goto failure;

            luminosities[0] = (Luminosity) { .intervals = &interval_lists[0] };
        }

        {
            interval_lists[1] = IntervalList_new(10);

            if (interval_lists[1] == NULL)
                goto delete_scratch;

            luminosities[1] = (Luminosity) { .intervals = &interval_lists[1] };

        }

        {

            interval_lists[2] = IntervalList_new(10);

            if (interval_lists[2] == NULL)
                goto delete_left;

            luminosities[2] = (Luminosity) { .intervals = &interval_lists[2] };
        }
    }

    for (uint32_t x = 0; x < Image_width(image); x++) {
        for (uint32_t y = 0; y < Image_height(image); y++) {
            Ray ray = {
                .pos = orig,
                .dpos = (Vec3) { .x = (double) x - Image_width(image) / 2, .y = Image_height(image) / 2 - (double) y, .z = focallength }
            };

            if (setjmp(envbuf) != 0)
                goto delete_right;

            if (self->calculate_luminosity(self, ray, &luminosities[0], &luminosities[1], &luminosities[2], envbuf)) {
                const double angle = acos(Vec3_dot(ray.dpos, luminosities[0].normal) / (Vec3_mag(ray.dpos) * Vec3_mag(luminosities[0].normal)));

                const Color color = {
                    .r = ((angle / M_PI - 0.5) * 2.0 + 2.0) * backgroundcolor.r / 3.0,
                    .g = ((angle / M_PI - 0.5) * 2.0 + 2.0) * backgroundcolor.g / 3.0,
                    .b = ((angle / M_PI - 0.5) * 2.0 + 2.0) * backgroundcolor.b / 3.0
                };

                Image_point(image, x, y, color);
            }
        }
    }

    IntervalList_delete(luminosities[2].intervals);
    IntervalList_delete(luminosities[1].intervals);
    IntervalList_delete(luminosities[0].intervals);

    return true;

    delete_right:
    IntervalList_delete(luminosities[2].intervals);

    delete_left:
    IntervalList_delete(luminosities[1].intervals);

    delete_scratch:
    IntervalList_delete(luminosities[0].intervals);

    failure:
    return false;
}

size_t Interval_union(Interval a, Interval b, IntervalList **intervals, jmp_buf envbuf)
{
    assert(a.left <= a.right);
    assert(b.left <= b.right);
    assert(intervals != NULL && *intervals != NULL);
    assert(envbuf != NULL);

    if (a.left <= b.left && b.right <= a.right) {
        try(IntervalList_push(intervals, a), envbuf, 1);
        return 1;
    }

    if (b.left <= a.left && a.right <= b.right) {
        try(IntervalList_push(intervals, b), envbuf, 1);
        return 1;
    }

    if (b.left <= a.right) {
        try(IntervalList_push(intervals, (Interval) { .left = a.left, .right = b.right }), envbuf, 1);
        return 1;
    }

    if (a.left <= b.right) {
        try(IntervalList_push(intervals, (Interval) { .left = b.left, .right = a.right }), envbuf, 1);
        return 1;
    }

    if (a.right < b.left) {
        try(IntervalList_push(intervals, a), envbuf, 1);
        try(IntervalList_push(intervals, b), envbuf, 1);
        return 2;
    }

    try(IntervalList_push(intervals, b), envbuf, 1);
    try(IntervalList_push(intervals, a), envbuf, 1);

    return 2;
}

size_t Interval_intersection(Interval a, Interval b, IntervalList **intervals, jmp_buf envbuf)
{
    assert(a.left <= a.right);
    assert(b.left <= b.right);
    assert(intervals != NULL && *intervals != NULL);
    assert(envbuf != NULL);

    if (a.left <= b.left && b.right <= a.right) {
        try(IntervalList_push(intervals, a), envbuf, 1);
        return 1;
    }

    if (b.left <= a.left && a.right <= b.right) {
        try(IntervalList_push(intervals, b), envbuf, 1);
        return 1;
    }

    if (b.left <= a.right) {
        try(IntervalList_push(intervals, (Interval) { .left = b.left, .right = a.right }), envbuf, 1);
        return 1;
    }

    if (a.left <= b.right) {
        try(IntervalList_push(intervals, (Interval) { .left = a.left, .right = b.right }), envbuf, 1);
        return 1;
    }

    return 0;
}

size_t Interval_difference(Interval a, Interval b, IntervalList **intervals, jmp_buf envbuf)
{
    assert(a.left <= a.right);
    assert(b.left <= b.right);
    assert(intervals != NULL && *intervals != NULL);
    assert(envbuf != NULL);

    if (a.left <= b.left && b.right <= a.right) {
        if (b.left == b.right) {
            try(IntervalList_push(intervals, a), envbuf, 1);
            return 1;
        }

        try(IntervalList_push(intervals, (Interval) { .left = a.left, .right = b.left }), envbuf, 1);
        try(IntervalList_push(intervals, (Interval) { .left = b.right, .right = a.right }), envbuf, 1);

        return 2;
    }

    if (b.left <= a.left && a.right <= b.right) {
        if (a.left == a.right) {
            try(IntervalList_push(intervals, b), envbuf, 1);
            return 1;
        }

        try(IntervalList_push(intervals, (Interval) { .left = b.left, .right = a.left }), envbuf, 1);
        try(IntervalList_push(intervals, (Interval) { .left = a.right, .right = b.right }), envbuf, 1);

        return 2;
    }

    if (b.left <= a.right) {
        try(IntervalList_push(intervals, (Interval) { .left = a.left, .right = b.left }), envbuf, 1);
        return 1;
    }

    if (a.left <= b.right) {
        try(IntervalList_push(intervals, (Interval) { .left = b.left, .right = a.left }), envbuf, 1);
        return 1;
    }

    return 0;
}
