typedef struct {
    GLfloat x, y, z;
} Vec3;

typedef struct {
    GLfloat data[4][4];
} Mat4;

extern void matrixOfPerspective(Mat4 * matrix,
    float l, float r, float b, float t, float n, float f);

extern void matrixOfIdentity(Mat4 * matrix);

extern void matrixOfScale(Mat4 * matrix, float sx, float sy, float sz);

extern void matrixOfTranslation(Mat4 * matrix, float dx, float dy, float dz);

extern void matrixOfRotationX(Mat4 * matrix, float angle);
extern void matrixOfRotationY(Mat4 * matrix, float angle);
extern void matrixOfRotationZ(Mat4 * matrix, float angle);

extern void mulm(Mat4 * out, Mat4 * a, Mat4 * b);
