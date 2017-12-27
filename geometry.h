/* extern void matrixOfCamera(Mat4 * matrix, Vec3 eye, Vec3 center, Vec3 up); */

extern void matrixOfPerspective(GLfloat * matrix,
    GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f);

extern void matrixOfIdentity(GLfloat * matrix);

extern void matrixOfScale(GLfloat * matrix, GLfloat sx, GLfloat sy, GLfloat sz);

extern void matrixOfTranslation(GLfloat * matrix, GLfloat dx, GLfloat dy, GLfloat dz);

extern void matrixOfRotationX(GLfloat * matrix, GLfloat angle);
extern void matrixOfRotationY(GLfloat * matrix, GLfloat angle);
extern void matrixOfRotationZ(GLfloat * matrix, GLfloat angle);

extern void matrixMultiplymm(GLfloat * out, GLfloat * a, GLfloat * b);

extern void matrixTranspose(GLfloat * out, GLfloat * in);
extern int matrixInverse(GLfloat * out, GLfloat * in);
