#include <math.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "utils.h"
#include "geometry.h"
#include "mesh.h"

static struct Rect {
    int width;
    int height;
} window = {
    .width = 640,
    .height = 480
};

static struct Mesh africanHead;
static GLuint texture;

static struct {
    GLuint id;
    GLuint modelViewInverseTransposeLocation;
    GLuint modelViewLocation;
    GLuint projectionLocation;
    GLuint textureLocation;
    GLuint lightPosLocation;
} solidShader;

#define PI (atan(1.) * 4.)

static GLuint
compileShaderFile(const char * shaderFilePath, GLenum shaderType) {
    GLchar * shaderSource = loadFile(shaderFilePath);
    GLuint shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, (const GLchar * const *)&shaderSource, NULL);
    glCompileShader(shaderId);
    return shaderId;
}

static GLint
compileShader(const char * vertexShaderPath, const char * fragmentShaderPath) {
    GLuint vertexShader = compileShaderFile(vertexShaderPath, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShaderFile(fragmentShaderPath, GL_FRAGMENT_SHADER);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    return shaderProgram;
}

static void
debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
        GLsizei length, const GLchar * message, const void * userParam) {
    (void)source, (void)type, (void)id, (void)length, (void)userParam;
    if (GL_DEBUG_SEVERITY_HIGH != severity) return;
    printf("opengl log: %s\n", message);
}

static GLfloat modelView[16];
static GLfloat projection[16];

static void
initOpengl(void) {
    glEnable(GL_DEPTH_TEST);
    glDebugMessageCallback(debugCallback, NULL);
    glClearColor(0, 0, 0, 1);
    solidShader.id = compileShader("solid.vert", "solid.frag");
    solidShader.modelViewInverseTransposeLocation = 
        glGetUniformLocation(solidShader.id, "modelViewInverseTranspose");
    solidShader.modelViewLocation = glGetUniformLocation(solidShader.id, "modelView");
    solidShader.projectionLocation = glGetUniformLocation(solidShader.id, "projection");
    solidShader.textureLocation = glGetUniformLocation(solidShader.id, "texture");
    solidShader.lightPosLocation = glGetUniformLocation(solidShader.id, "lightPos");
    matrixOfPerspective(projection, -1, 1, -1, 1, 1, 100);
    africanHead = createMeshFromObj("african_head.obj");
    int width, height, channels;
    GLchar * data = (GLchar *)stbi_load("african_head_diffuse.tga", &width, &height, &channels, 3);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

static void
loadIdentity() {
    matrixOfIdentity(modelView);
}

static void
translate(GLfloat dx, GLfloat dy, GLfloat dz) {
    GLfloat t[16];
    matrixOfTranslation(t, dx, dy, dz);
    matrixMultiplymm(modelView, modelView, t);
}

#if 0

static void
rotateX(GLfloat angle) {
    GLfloat t[16];
    matrixOfRotationX(t, angle);
    matrixMultiplymm(modelView, modelView, t);
}

static void
rotateY(GLfloat angle) {
    GLfloat t[16];
    matrixOfRotationY(t, angle);
    matrixMultiplymm(modelView, modelView, t);
}

static void
rotateZ(GLfloat angle) {
    GLfloat t[16];
    matrixOfRotationZ(t, angle);
    matrixMultiplymm(modelView, modelView, t);
}

static void
scale(GLfloat sx, GLfloat sy, GLfloat sz) {
    GLfloat t[16];
    matrixOfScale(t, sx, sy, sz);
    matrixMultiplymm(modelView, modelView, t);
}

#endif

static GLfloat angle = 0;

static void
display(void) {
    angle += 0.01;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    loadIdentity();
    translate(0, 0, -1.8);
    glUseProgram(solidShader.id);
    glUniformMatrix4fv(solidShader.projectionLocation, 1, GL_TRUE, projection);
    glUniformMatrix4fv(solidShader.modelViewLocation, 1, GL_TRUE, modelView);
    GLfloat modelViewInverseTranspose[16];
    matrixInverse(modelViewInverseTranspose, modelView);
    matrixTranspose(modelViewInverseTranspose, modelViewInverseTranspose);
    glUniformMatrix4fv(solidShader.modelViewInverseTransposeLocation, 1, GL_TRUE, 
        modelViewInverseTranspose);
    glUniform3f(solidShader.lightPosLocation, 2 * cos(angle), 0, -1.8 + 2 * sin(angle));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(solidShader.textureLocation, 0);
    drawMesh(africanHead);
    glutSwapBuffers();
}

static void 
reshape(int width, int height) {
    window.width = width;
    window.height = height;
    glViewport(0, 0, width, height);
}

int main(int argc, char * argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitContextVersion(3, 3);
    glutInitContextFlags(GLUT_CORE_PROFILE | GLUT_DEBUG);
    glutInitWindowSize(window.width, window.height);
    glutCreateWindow("Shaders");
    glewExperimental = GL_TRUE;
    glewInit();
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(reshape);
    initOpengl();
    glutMainLoop();
}
