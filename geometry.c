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
matrixMultiplymm(Mat4 * out, Mat4 * a, Mat4 * b) {
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

static void
matrixMultiplymd(Mat4 * out, Mat4 * a, GLfloat b) {
    Mat4 temp;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            temp.data[i][j] = b * a->data[i][j];
        }
    }
    *out = temp;
}

extern void 
matrixTranspose(Mat4 * out, Mat4 * in) {
    Mat4 temp;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            temp.data[j][i] = in->data[i][j];
        }
    }
    *out = temp;
}

static double
matrixDeterminant(Mat4 * m) {
    return
        m->data[0][3] * m->data[1][2] * m->data[2][1] * m->data[3][0] -
        m->data[0][2] * m->data[1][3] * m->data[2][1] * m->data[3][0] -
        m->data[0][3] * m->data[1][1] * m->data[2][2] * m->data[3][0] +
        m->data[0][1] * m->data[1][3] * m->data[2][2] * m->data[3][0] +
        m->data[0][2] * m->data[1][1] * m->data[2][3] * m->data[3][0] -
        m->data[0][1] * m->data[1][2] * m->data[2][3] * m->data[3][0] -
        m->data[0][3] * m->data[1][2] * m->data[2][0] * m->data[3][1] +
        m->data[0][2] * m->data[1][3] * m->data[2][0] * m->data[3][1] +
        m->data[0][3] * m->data[1][0] * m->data[2][2] * m->data[3][1] -
        m->data[0][0] * m->data[1][3] * m->data[2][2] * m->data[3][1] -
        m->data[0][2] * m->data[1][0] * m->data[2][3] * m->data[3][1] +
        m->data[0][0] * m->data[1][2] * m->data[2][3] * m->data[3][1] +
        m->data[0][3] * m->data[1][1] * m->data[2][0] * m->data[3][2] -
        m->data[0][1] * m->data[1][3] * m->data[2][0] * m->data[3][2] -
        m->data[0][3] * m->data[1][0] * m->data[2][1] * m->data[3][2] +
        m->data[0][0] * m->data[1][3] * m->data[2][1] * m->data[3][2] +
        m->data[0][1] * m->data[1][0] * m->data[2][3] * m->data[3][2] -
        m->data[0][0] * m->data[1][1] * m->data[2][3] * m->data[3][2] -
        m->data[0][2] * m->data[1][1] * m->data[2][0] * m->data[3][3] +
        m->data[0][1] * m->data[1][2] * m->data[2][0] * m->data[3][3] +
        m->data[0][2] * m->data[1][0] * m->data[2][1] * m->data[3][3] -
        m->data[0][0] * m->data[1][2] * m->data[2][1] * m->data[3][3] -
        m->data[0][1] * m->data[1][0] * m->data[2][2] * m->data[3][3] +
        m->data[0][0] * m->data[1][1] * m->data[2][2] * m->data[3][3]; 
}

extern int
matrixInverse(Mat4 * out, Mat4 * in) {
    GLfloat * m = &in->data[0][0];
    GLfloat inv[16];
    inv[ 0] =  m[ 5] * m[10] * m[15] - m[ 5] * m[11] * m[14] - m[ 9] * m[ 6] * m[15] 
            +  m[ 9] * m[ 7] * m[14] + m[13] * m[ 6] * m[11] - m[13] * m[ 7] * m[10];
    inv[ 1] = -m[ 1] * m[10] * m[15] + m[ 1] * m[11] * m[14] + m[ 9] * m[ 2] * m[15] 
            -  m[ 9] * m[ 3] * m[14] - m[13] * m[ 2] * m[11] + m[13] * m[ 3] * m[10];
    inv[ 2] =  m[ 1] * m[ 6] * m[15] - m[ 1] * m[ 7] * m[14] - m[ 5] * m[ 2] * m[15] 
            +  m[ 5] * m[ 3] * m[14] + m[13] * m[ 2] * m[ 7] - m[13] * m[ 3] * m[ 6];
    inv[ 3] = -m[ 1] * m[ 6] * m[11] + m[ 1] * m[ 7] * m[10] + m[ 5] * m[ 2] * m[11] 
            -  m[ 5] * m[ 3] * m[10] - m[ 9] * m[ 2] * m[ 7] + m[ 9] * m[ 3] * m[ 6];
    inv[ 4] = -m[ 4] * m[10] * m[15] + m[ 4] * m[11] * m[14] + m[ 8] * m[ 6] * m[15] 
            -  m[ 8] * m[ 7] * m[14] - m[12] * m[ 6] * m[11] + m[12] * m[ 7] * m[10];
    inv[ 5] =  m[ 0] * m[10] * m[15] - m[ 0] * m[11] * m[14] - m[ 8] * m[ 2] * m[15] 
            +  m[ 8] * m[ 3] * m[14] + m[12] * m[ 2] * m[11] - m[12] * m[ 3] * m[10];
    inv[ 6] = -m[ 0] * m[ 6] * m[15] + m[ 0] * m[ 7] * m[14] + m[ 4] * m[ 2] * m[15] 
            -  m[ 4] * m[ 3] * m[14] - m[12] * m[ 2] * m[ 7] + m[12] * m[ 3] * m[ 6];
    inv[ 7] =  m[ 0] * m[ 6] * m[11] - m[ 0] * m[ 7] * m[10] - m[ 4] * m[ 2] * m[11] 
            +  m[ 4] * m[ 3] * m[10] + m[ 8] * m[ 2] * m[ 7] - m[ 8] * m[ 3] * m[ 6];
    inv[ 8] =  m[ 4] * m[ 9] * m[15] - m[ 4] * m[11] * m[13] - m[ 8] * m[ 5] * m[15] 
            +  m[ 8] * m[ 7] * m[13] + m[12] * m[ 5] * m[11] - m[12] * m[ 7] * m[ 9];
    inv[ 9] = -m[ 0] * m[ 9] * m[15] + m[ 0] * m[11] * m[13] + m[ 8] * m[ 1] * m[15] 
            -  m[ 8] * m[ 3] * m[13] - m[12] * m[ 1] * m[11] + m[12] * m[ 3] * m[ 9];
    inv[10] =  m[ 0] * m[ 5] * m[15] - m[ 0] * m[ 7] * m[13] - m[ 4] * m[ 1] * m[15] 
            +  m[ 4] * m[ 3] * m[13] + m[12] * m[ 1] * m[ 7] - m[12] * m[ 3] * m[ 5];
    inv[11] = -m[ 0] * m[ 5] * m[11] + m[ 0] * m[ 7] * m[ 9] + m[ 4] * m[ 1] * m[11] 
            -  m[ 4] * m[ 3] * m[ 9] - m[ 8] * m[ 1] * m[ 7] + m[ 8] * m[ 3] * m[ 5];
    inv[12] = -m[ 4] * m[ 9] * m[14] + m[ 4] * m[10] * m[13] + m[ 8] * m[ 5] * m[14] 
            -  m[ 8] * m[ 6] * m[13] - m[12] * m[ 5] * m[10] + m[12] * m[ 6] * m[ 9];
    inv[13] =  m[ 0] * m[ 9] * m[14] - m[ 0] * m[10] * m[13] - m[ 8] * m[ 1] * m[14] 
            +  m[ 8] * m[ 2] * m[13] + m[12] * m[ 1] * m[10] - m[12] * m[ 2] * m[ 9];
    inv[14] = -m[ 0] * m[ 5] * m[14] + m[ 0] * m[ 6] * m[13] + m[ 4] * m[ 1] * m[14] 
            -  m[ 4] * m[ 2] * m[13] - m[12] * m[ 1] * m[ 6] + m[12] * m[ 2] * m[ 5];
    inv[15] =  m[ 0] * m[ 5] * m[10] - m[ 0] * m[ 6] * m[ 9] - m[ 4] * m[ 1] * m[10] 
            +  m[ 4] * m[ 2] * m[ 9] + m[ 8] * m[ 1] * m[ 6] - m[ 8] * m[ 2] * m[ 5];
    GLfloat d = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12]; 
    if (!d) return 1; 
    d = 1 / d;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            out->data[i][j] = inv[i * 4 + j] * d;
        }
    }
    return 0;
}
