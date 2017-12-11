typedef struct {
    GLfloat x, y, z;
} Vec3;

typedef struct {
    GLfloat data[4][4];
} Mat4;

extern void perspective(Mat4 * matrix,
    float l, float r, float b, float t, float n, float f);

extern void identity(Mat4 * matrix);

extern void scale(Mat4 * matrix, float sx, float sy, float sz);

extern void translation(Mat4 * matrix, float dx, float dy, float dz);

extern void rotationX(Mat4 * matrix, float angle);
extern void rotationY(Mat4 * matrix, float angle);
extern void rotationZ(Mat4 * matrix, float angle);

extern void mulm(Mat4 * out, Mat4 * a, Mat4 * b);