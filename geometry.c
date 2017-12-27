#include <math.h>
#include <string.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "geometry.h"

extern void
matrixOfPerspective(GLfloat * matrix, 
        GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f) {
    GLfloat temp[] = {
        2 * n / (r - l), 0, (r + l) / (r - l), 0,
        0, 2 * n / (t - b), (t + b) / (t - b), 0,
        0, 0, -(f + n) / (f - n), -2 * f * n / (f - n),
        0, 0, -1, 0,
    };
    memcpy(matrix, temp, sizeof(temp));
}

extern void
matrixOfIdentity(GLfloat * matrix) {
    GLfloat temp[] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };
    memcpy(matrix, temp, sizeof(temp));
}

extern void
matrixOfScale(GLfloat * matrix, GLfloat sx, GLfloat sy, GLfloat sz) {
    GLfloat temp[] = {
        sx, 0, 0, 0,
        0, sy, 0, 0,
        0, 0, sz, 0,
        0, 0, 0, 1,
    };
    memcpy(matrix, temp, sizeof(temp));
}

extern void
matrixOfTranslation(GLfloat * matrix, GLfloat dx, GLfloat dy, GLfloat dz) {
    GLfloat temp[] = {
        1, 0, 0, dx,
        0, 1, 0, dy,
        0, 0, 1, dz,
        0, 0, 0, 1,
    };
    memcpy(matrix, temp, sizeof(temp));
}

extern void
matrixOfRotationX(GLfloat * matrix, GLfloat angle) {
    GLfloat temp[] = {
        1, 0, 0, 0,
        0, cos(angle), -sin(angle), 0,
        0, sin(angle), cos(angle), 0,
        0, 0, 0, 1,
    };
    memcpy(matrix, temp, sizeof(temp));
}

extern void
matrixOfRotationY(GLfloat * matrix, GLfloat angle) {
    GLfloat temp[] = {
        cos(angle), 0, sin(angle), 0,
        0, 1, 0, 0,
        -sin(angle), 0, cos(angle), 0,
        0, 0, 0, 1,
    };
    memcpy(matrix, temp, sizeof(temp));
}

extern void
matrixOfRotationZ(GLfloat * matrix, GLfloat angle) {
    GLfloat temp[] = {
        cos(angle), -sin(angle), 0, 0,
        sin(angle), cos(angle), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };
    memcpy(matrix, temp, sizeof(temp));
}

extern void
matrixMultiplymm(GLfloat * out, GLfloat * a, GLfloat * b) {
    GLfloat temp[16];
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            temp[i * 4 + j] = 0;
            for (int k = 0; k < 4; ++k) {
                temp[i * 4 + j] += a[i * 4 + k] * b[k * 4 + j];
            }
        }
    }
    memcpy(out, temp, sizeof(temp));
}

extern void 
matrixTranspose(GLfloat * out, GLfloat * in) {
    GLfloat temp[16];
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            temp[j * 4 + i] = in[i * 4 + j];
        }
    }
    memcpy(out, temp, sizeof(temp));
}

static void
matrixMultiplymd(GLfloat * out, GLfloat * a, GLfloat b) {
    GLfloat temp[16];
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            temp[i * 4 + j] = a[i * 4 + j] * b;
        }
    }
    memcpy(out, temp, sizeof(temp));
}

extern int
matrixInverse(GLfloat * out, GLfloat * in) {
    GLfloat inv[16];
    inv[ 0] =  in[ 5] * in[10] * in[15] - in[ 5] * in[11] * in[14] - in[ 9] * in[ 6] * in[15] 
            +  in[ 9] * in[ 7] * in[14] + in[13] * in[ 6] * in[11] - in[13] * in[ 7] * in[10];
    inv[ 1] = -in[ 1] * in[10] * in[15] + in[ 1] * in[11] * in[14] + in[ 9] * in[ 2] * in[15] 
            -  in[ 9] * in[ 3] * in[14] - in[13] * in[ 2] * in[11] + in[13] * in[ 3] * in[10];
    inv[ 2] =  in[ 1] * in[ 6] * in[15] - in[ 1] * in[ 7] * in[14] - in[ 5] * in[ 2] * in[15] 
            +  in[ 5] * in[ 3] * in[14] + in[13] * in[ 2] * in[ 7] - in[13] * in[ 3] * in[ 6];
    inv[ 3] = -in[ 1] * in[ 6] * in[11] + in[ 1] * in[ 7] * in[10] + in[ 5] * in[ 2] * in[11] 
            -  in[ 5] * in[ 3] * in[10] - in[ 9] * in[ 2] * in[ 7] + in[ 9] * in[ 3] * in[ 6];
    inv[ 4] = -in[ 4] * in[10] * in[15] + in[ 4] * in[11] * in[14] + in[ 8] * in[ 6] * in[15] 
            -  in[ 8] * in[ 7] * in[14] - in[12] * in[ 6] * in[11] + in[12] * in[ 7] * in[10];
    inv[ 5] =  in[ 0] * in[10] * in[15] - in[ 0] * in[11] * in[14] - in[ 8] * in[ 2] * in[15] 
            +  in[ 8] * in[ 3] * in[14] + in[12] * in[ 2] * in[11] - in[12] * in[ 3] * in[10];
    inv[ 6] = -in[ 0] * in[ 6] * in[15] + in[ 0] * in[ 7] * in[14] + in[ 4] * in[ 2] * in[15] 
            -  in[ 4] * in[ 3] * in[14] - in[12] * in[ 2] * in[ 7] + in[12] * in[ 3] * in[ 6];
    inv[ 7] =  in[ 0] * in[ 6] * in[11] - in[ 0] * in[ 7] * in[10] - in[ 4] * in[ 2] * in[11] 
            +  in[ 4] * in[ 3] * in[10] + in[ 8] * in[ 2] * in[ 7] - in[ 8] * in[ 3] * in[ 6];
    inv[ 8] =  in[ 4] * in[ 9] * in[15] - in[ 4] * in[11] * in[13] - in[ 8] * in[ 5] * in[15] 
            +  in[ 8] * in[ 7] * in[13] + in[12] * in[ 5] * in[11] - in[12] * in[ 7] * in[ 9];
    inv[ 9] = -in[ 0] * in[ 9] * in[15] + in[ 0] * in[11] * in[13] + in[ 8] * in[ 1] * in[15] 
            -  in[ 8] * in[ 3] * in[13] - in[12] * in[ 1] * in[11] + in[12] * in[ 3] * in[ 9];
    inv[10] =  in[ 0] * in[ 5] * in[15] - in[ 0] * in[ 7] * in[13] - in[ 4] * in[ 1] * in[15] 
            +  in[ 4] * in[ 3] * in[13] + in[12] * in[ 1] * in[ 7] - in[12] * in[ 3] * in[ 5];
    inv[11] = -in[ 0] * in[ 5] * in[11] + in[ 0] * in[ 7] * in[ 9] + in[ 4] * in[ 1] * in[11] 
            -  in[ 4] * in[ 3] * in[ 9] - in[ 8] * in[ 1] * in[ 7] + in[ 8] * in[ 3] * in[ 5];
    inv[12] = -in[ 4] * in[ 9] * in[14] + in[ 4] * in[10] * in[13] + in[ 8] * in[ 5] * in[14] 
            -  in[ 8] * in[ 6] * in[13] - in[12] * in[ 5] * in[10] + in[12] * in[ 6] * in[ 9];
    inv[13] =  in[ 0] * in[ 9] * in[14] - in[ 0] * in[10] * in[13] - in[ 8] * in[ 1] * in[14] 
            +  in[ 8] * in[ 2] * in[13] + in[12] * in[ 1] * in[10] - in[12] * in[ 2] * in[ 9];
    inv[14] = -in[ 0] * in[ 5] * in[14] + in[ 0] * in[ 6] * in[13] + in[ 4] * in[ 1] * in[14] 
            -  in[ 4] * in[ 2] * in[13] - in[12] * in[ 1] * in[ 6] + in[12] * in[ 2] * in[ 5];
    inv[15] =  in[ 0] * in[ 5] * in[10] - in[ 0] * in[ 6] * in[ 9] - in[ 4] * in[ 1] * in[10] 
            +  in[ 4] * in[ 2] * in[ 9] + in[ 8] * in[ 1] * in[ 6] - in[ 8] * in[ 2] * in[ 5];
    GLfloat d = in[0] * inv[0] + in[1] * inv[4] + in[2] * inv[8] + in[3] * inv[12]; 
    if (!d) return 1; 
    matrixMultiplymd(out, inv, 1 / d);
    return 0;
}
