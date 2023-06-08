#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "LinearAlgebra.h"

void Mat4_invert(Mat4 self)
{
    assert(self != NULL);

    /** CITE: Inversion formula taken from Wolfram Alpha */

    const double denom =
        self[0][0] *
            (self[1][1] * (self[2][2] * self[3][3] - self[2][3] * self[3][2]) 
           + self[1][2] * (self[2][3] * self[3][1] - self[2][1] * self[3][3])
           + self[1][3] * (self[2][1] * self[3][2] - self[2][2] * self[3][1]))
      + self[0][1] *
            (self[1][0] * (self[2][3] * self[3][2] - self[2][2] * self[3][3])
           + self[1][2] * (self[2][0] * self[3][3] - self[2][3] * self[3][0])
           + self[1][3] * (self[2][2] * self[3][0] - self[2][0] * self[3][2]))
      + self[0][2] *
            (self[1][0] * (self[2][1] * self[3][3] - self[2][3] * self[3][1])
           + self[1][1] * (self[2][3] * self[3][0] - self[2][0] * self[3][3])
           + self[1][3] * (self[2][0] * self[3][1] - self[2][1] * self[3][0]))
      + self[0][3] *
            (self[1][0] * (self[2][2] * self[3][1] - self[2][1] * self[3][2])
           + self[1][1] * (self[2][0] * self[3][2] - self[2][2] * self[3][0])
           + self[1][2] * (self[2][1] * self[3][0] - self[2][0] * self[3][1]));

    assert(denom != 0.0);

    const Mat4 temp = {
        {
            (self[1][1] * (self[2][2] * self[3][3] - self[2][3] * self[3][2]) + self[1][2] * (self[2][3] * self[3][1] - self[2][1] * self[3][3]) + self[1][3] * (self[2][1] * self[3][2] - self[2][2] * self[3][1])) / denom,
            (self[0][1] * (self[2][3] * self[3][2] - self[2][2] * self[3][3]) + self[0][2] * (self[2][1] * self[3][3] - self[2][3] * self[3][1]) + self[0][3] * (self[2][2] * self[3][1] - self[2][1] * self[3][2])) / denom,
            (self[0][1] * (self[1][2] * self[3][3] - self[1][3] * self[3][2]) + self[0][2] * (self[1][3] * self[3][1] - self[1][1] * self[3][3]) + self[0][3] * (self[1][1] * self[3][2] - self[1][2] * self[3][1])) / denom,
            (self[0][1] * (self[1][3] * self[2][2] - self[1][2] * self[2][3]) + self[0][2] * (self[1][1] * self[2][3] - self[1][3] * self[2][1]) + self[0][3] * (self[1][2] * self[2][1] - self[1][1] * self[2][2])) / denom
        },
        {
            (self[1][0] * (self[2][3] * self[3][2] - self[2][2] * self[3][3]) + self[1][2] * (self[2][0] * self[3][3] - self[2][3] * self[3][0]) + self[1][3] * (self[2][2] * self[3][0] - self[2][0] * self[3][2])) / denom,
            (self[0][0] * (self[2][2] * self[3][3] - self[2][3] * self[3][2]) + self[0][2] * (self[2][3] * self[3][0] - self[2][0] * self[3][3]) + self[0][3] * (self[2][0] * self[3][2] - self[2][2] * self[3][0])) / denom,
            (self[0][0] * (self[1][3] * self[3][2] - self[1][2] * self[3][3]) + self[0][2] * (self[1][0] * self[3][3] - self[1][3] * self[3][0]) + self[0][3] * (self[1][2] * self[3][0] - self[1][0] * self[3][2])) / denom,
            (self[0][0] * (self[1][2] * self[2][3] - self[1][3] * self[2][2]) + self[0][2] * (self[1][3] * self[2][0] - self[1][0] * self[2][3]) + self[0][3] * (self[1][0] * self[2][2] - self[1][2] * self[2][0])) / denom
        },
        {
            (self[1][0] * (self[2][1] * self[3][3] - self[2][3] * self[3][1]) + self[1][1] * (self[2][3] * self[3][0] - self[2][0] * self[3][3]) + self[1][3] * (self[2][0] * self[3][1] - self[2][1] * self[3][0])) / denom,
            (self[0][0] * (self[2][3] * self[3][1] - self[2][1] * self[3][3]) + self[0][1] * (self[2][0] * self[3][3] - self[2][3] * self[3][0]) + self[0][3] * (self[2][1] * self[3][0] - self[2][0] * self[3][1])) / denom,
            (self[0][0] * (self[1][1] * self[3][3] - self[1][3] * self[3][1]) + self[0][1] * (self[1][3] * self[3][0] - self[1][0] * self[3][3]) + self[0][3] * (self[1][0] * self[3][1] - self[1][1] * self[3][0])) / denom,
            (self[0][0] * (self[1][3] * self[2][1] - self[1][1] * self[2][3]) + self[0][1] * (self[1][0] * self[2][3] - self[1][3] * self[2][0]) + self[0][3] * (self[1][1] * self[2][0] - self[1][0] * self[2][1])) / denom
        },
        {
            (self[1][0] * (self[2][2] * self[3][1] - self[2][1] * self[3][2]) + self[1][1] * (self[2][0] * self[3][2] - self[2][2] * self[3][0]) + self[1][2] * (self[2][1] * self[3][0] - self[2][0] * self[3][1])) / denom,
            (self[0][0] * (self[2][1] * self[3][2] - self[2][2] * self[3][1]) + self[0][1] * (self[2][2] * self[3][0] - self[2][0] * self[3][2]) + self[0][2] * (self[2][0] * self[3][1] - self[2][1] * self[3][0])) / denom,
            (self[0][0] * (self[1][2] * self[3][1] - self[1][1] * self[3][2]) + self[0][1] * (self[1][0] * self[3][2] - self[1][2] * self[3][0]) + self[0][2] * (self[1][1] * self[3][0] - self[1][0] * self[3][1])) / denom,
            (self[0][0] * (self[1][1] * self[2][2] - self[1][2] * self[2][1]) + self[0][1] * (self[1][2] * self[2][0] - self[1][0] * self[2][2]) + self[0][2] * (self[1][0] * self[2][1] - self[1][1] * self[2][0])) / denom
        }
    };

    memcpy(self, temp, sizeof temp);
}

Vec3 Mat4_mult_Vec3(const Mat4 self, Vec3 vec)
{
    assert(self != NULL);

    return (Vec3) {
        .x = self[0][0] * vec.x + self[0][1] * vec.y + self[0][2] * vec.z + self[0][3],
        .y = self[1][0] * vec.x + self[1][1] * vec.y + self[1][2] * vec.z + self[1][3],
        .z = self[2][0] * vec.x + self[2][1] * vec.y + self[2][2] * vec.z + self[2][3]
    };
}

Vec3 Mat4_mult_Raytip(const Mat4 self, Vec3 raytip)
{
    assert(self != NULL);

    return (Vec3) {
        .x = self[0][0] * raytip.x + self[0][1] * raytip.y + self[0][2] * raytip.z,
        .y = self[1][0] * raytip.x + self[1][1] * raytip.y + self[1][2] * raytip.z,
        .z = self[2][0] * raytip.x + self[2][1] * raytip.y + self[2][2] * raytip.z
    };
}

/** TODO: Maybe `ray` should be modified to prevent needless copying */
Ray Mat4_mult_Ray(const Mat4 self, Ray ray)
{
    assert(self != NULL);

    return (Ray) {
        .pos = Mat4_mult_Vec3(self, ray.pos),
        .dpos = Mat4_mult_Raytip(self, ray.dpos)
    };
}

/** TODO: Add reflections and shearing and the like */
void Mat4_make_transformation(Mat4 self, Pos3 pos, Dim3 dim, Rot3 rot)
{
    assert(self != NULL);

    self[0][0] = dim.width * cos(rot.beta) * cos(rot.gamma);
    self[0][1] = dim.height * (sin(rot.alpha) * sin(rot.beta) * cos(rot.gamma) - cos(rot.alpha) * sin(rot.gamma));
    self[0][2] = dim.depth * (cos(rot.alpha) * sin(rot.beta) * cos(rot.gamma) - sin(rot.alpha) * sin(rot.gamma));
    self[0][3] = pos.x;

    self[1][0] = dim.width * cos(rot.beta) * sin(rot.gamma);
    self[1][1] = dim.height * (sin(rot.alpha) * sin(rot.beta) * sin(rot.gamma) + cos(rot.alpha) * cos(rot.gamma));
    self[1][2] = dim.depth * (cos(rot.alpha) * sin(rot.beta) * sin(rot.gamma) - sin(rot.alpha) * cos(rot.gamma));
    self[1][3] = pos.y;

    self[2][0] = -dim.width * sin(rot.beta);
    self[2][1] = dim.height * sin(rot.alpha) * cos(rot.beta);
    self[2][2] = dim.depth * cos(rot.alpha) * cos(rot.beta);
    self[2][3] = pos.z;

    self[3][0] = 0.0;
    self[3][1] = 0.0;
    self[3][2] = 0.0;
    self[3][3] = 1.0;
}

double Vec3_mag(Vec3 self)
{
    return sqrt(self.x * self.x + self.y * self.y + self.z * self.z);
}

double Vec3_dot(Vec3 self, Vec3 vec)
{
    return self.x * vec.x + self.y * vec.y + self.z * vec.z;
}

/** NOTE: For debugging purposes */
void Mat4_println(const Mat4 self)
{
    assert(self != NULL);

    printf("[ %10g %10g %10g %10g ]\n", self[0][0], self[0][1], self[0][2], self[0][3]);
    printf("[ %10g %10g %10g %10g ]\n", self[1][0], self[1][1], self[1][2], self[1][3]);
    printf("[ %10g %10g %10g %10g ]\n", self[2][0], self[2][1], self[2][2], self[2][3]);
    printf("[ %10g %10g %10g %10g ]\n", self[3][0], self[3][1], self[3][2], self[3][3]);

    puts("");
}

/** NOTE: For debugging purposes */
void Ray_println(Ray self)
{
    printf("[ <%g %g %g> <%g %g %g> ]\n", self.pos.x, self.pos.y, self.pos.z, self.dpos.x, self.dpos.y, self.dpos.z);
}

/** NOTE: For debugging purposes */
void Vec3_println(Vec3 self)
{
    printf("<%g %g %g>\n", self.x, self.y, self.z);
}
