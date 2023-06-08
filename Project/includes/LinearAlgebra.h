#ifndef LINEAR_ALGEBRA_H
#define LINEAR_ALGEBRA_H

/* TODO: Maybe make this a 3x4 matrix instead */
typedef double Mat4[4][4];

typedef struct Vec3 { double x, y, z; } Vec3;
typedef struct Pos3 { double x, y, z; } Pos3;
typedef struct Dim3 { double width, height, depth; } Dim3;
typedef struct Rot3 { double alpha, beta, gamma; } Rot3;

typedef struct Ray { Vec3 pos, dpos; } Ray;

typedef struct Interval { double left, right; /** NOTE: left <= right */ } Interval;

void Mat4_invert(Mat4 self);

Vec3 Mat4_mult_Vec3(const Mat4 self, Vec3 vec);

Vec3 Mat4_mult_Raytip(const Mat4 self, Vec3 raytip);

Ray Mat4_mult_Ray(const Mat4 self, Ray ray);

void Mat4_make_transformation(Mat4 self, Pos3 pos, Dim3 dim, Rot3 rot);

double Vec3_mag(Vec3 self);

double Vec3_dot(Vec3 self, Vec3 vec);

void Mat4_println(const Mat4 self);

void Ray_println(Ray self);

void Vec3_println(Vec3 self);

#endif
