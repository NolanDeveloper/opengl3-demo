#include <math.h>
#include <string.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "geometry.h"

extern void
matrixOfPerspective(Mat4 * matrix, float l, float r, float b, float t, float n, float f) {
    *matrix = (Mat4) {
        2 * n / (r - l), 0, (r + l) / (r - l), 0,
        0, 2 * n / (t - b), (t + b) / (t - b), 0,
        0, 0, -(f + n) / (f - n), -2 * f * n / (f - n),
        0, 0, -1, 0,
    };
}

extern void
matrixOfIdentity(Mat4 * matrix) {
    *matrix = (Mat4) {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };
}

extern void
matrixOfScale(Mat4 * matrix, float sx, float sy, float sz) {
    *matrix = (Mat4) {
        sx, 0, 0, 0,
        0, sy, 0, 0,
        0, 0, sz, 0,
        0, 0, 0, 1,
    };
}

extern void
matrixOfTranslation(Mat4 * matrix, float dx, float dy, float dz) {
    *matrix = (Mat4) {
        1, 0, 0, dx,
        0, 1, 0, dy,
        0, 0, 1, dz,
        0, 0, 0, 1,
    };
}

extern void
matrixOfRotationX(Mat4 * matrix, float angle) {
    *matrix = (Mat4) {
        1, 0, 0, 0,
        0, cos(angle), -sin(angle), 0,
        0, sin(angle), cos(angle), 0,
        0, 0, 0, 1,
    };
}

extern void
matrixOfRotationY(Mat4 * matrix, float angle) {
    *matrix = (Mat4) {
        cos(angle), 0, sin(angle), 0,
        0, 1, 0, 0,
        -sin(angle), 0, cos(angle), 0,
        0, 0, 0, 1,
    };
}

extern void
matrixOfRotationZ(Mat4 * matrix, float angle) {
    *matrix = (Mat4) {
        cos(angle), -sin(angle), 0, 0,
        sin(angle), cos(angle), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };
}

extern void
mulm(Mat4 * out, Mat4 * a, Mat4 * b) {
    Mat4 temp;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            temp.data[i][j] = 0;
            for (int k = 0; k < 4; ++k) {
                temp.data[i][j] += a->data[i][k] * b->data[k][j];
            }
        }
    }
    *out = temp;
}
