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
static struct Mesh monkey;
static struct Mesh sphere;

static struct {
    GLuint id;
    GLuint modelViewInverseTransposeLocation;
    GLuint modelViewLocation;
    GLuint projectionLocation;
    GLuint textureLocation;
    GLuint materialAmbientLocation;
    GLuint materialDiffuseLocation;
    GLuint materialSpecularLocation;
    GLuint materialShininessLocation;
    GLuint lightPosLocation;
} solidShader;

#define PI (atan(1.) * 4.)

static GLuint
compileShaderFile(const char * shaderFilePath, GLenum shaderType) {
    GLchar * shaderSource = loadFile(shaderFilePath);
    GLuint shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, (const GLchar * const *)&shaderSource, NULL);
    glCompileShader(shaderId);
    free(shaderSource);
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

static GLuint
loadTexture(char * filePath) {
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    int width, height, channels;
    GLchar * data = (GLchar *)stbi_load(filePath, &width, &height, &channels, 3);
    if (!data) exit(1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    return texture;
}

static struct Mesh
loadModel(char * objFilePath, char * textureFilePath) {
    return createMeshFromObj(objFilePath, loadTexture(textureFilePath));
}

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
    solidShader.materialAmbientLocation = glGetUniformLocation(solidShader.id, "material.ambient");
    solidShader.materialDiffuseLocation = glGetUniformLocation(solidShader.id, "material.diffuse");
    solidShader.materialSpecularLocation = 
        glGetUniformLocation(solidShader.id, "material.specular");
    solidShader.materialShininessLocation = 
        glGetUniformLocation(solidShader.id, "material.shininess");
    solidShader.lightPosLocation = glGetUniformLocation(solidShader.id, "lightPos");
    matrixOfPerspective(projection, -1, 1, -1, 1, 1, 100);
    africanHead = loadModel("african_head.obj", "african_head_diffuse.tga");
    monkey = loadModel("monkey.obj", "monkey_diffuse.png");
    sphere = loadModel("sphere.obj", "sphere_diffuse.png");
    for (int i = 0; i < 3; ++i) {
        africanHead.material.ambient[i] = 0;
        monkey.material.diffuse[i] = 0;
        sphere.material.specular[i] = 0;
    }
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

#endif

static void
scale(GLfloat sx, GLfloat sy, GLfloat sz) {
    GLfloat t[16];
    matrixOfScale(t, sx, sy, sz);
    matrixMultiplymm(modelView, modelView, t);
}

static void
drawModel(struct Mesh mesh) {
    glUniformMatrix4fv(solidShader.modelViewLocation, 1, GL_TRUE, modelView);
    GLfloat modelViewInverseTranspose[16];
    matrixInverse(modelViewInverseTranspose, modelView);
    matrixTranspose(modelViewInverseTranspose, modelViewInverseTranspose);
    glUniformMatrix4fv(solidShader.modelViewInverseTransposeLocation, 1, GL_TRUE, 
        modelViewInverseTranspose);
    glUniform3fv(solidShader.materialAmbientLocation, 1, mesh.material.ambient);
    glUniform3fv(solidShader.materialDiffuseLocation, 1, mesh.material.diffuse);
    glUniform3fv(solidShader.materialSpecularLocation, 1, mesh.material.specular);
    glUniform1f(solidShader.materialShininessLocation, mesh.material.shininess);
    drawMesh(mesh);
}

static GLfloat angle = 0;

static void
display(void) {
    angle += 0.01;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(solidShader.id);
    glUniform3f(solidShader.lightPosLocation, 0, 0, 2 * cos(angle));
    glUniformMatrix4fv(solidShader.projectionLocation, 1, GL_TRUE, projection);
    glUniform1i(solidShader.textureLocation, 0);
    // Draw african head
    loadIdentity();
    translate(-1, 1, -2);
    drawModel(africanHead);
    // Draw monkey
    loadIdentity();
    translate(1, 1, -2);
    scale(0.8, 0.8, 0.8);
    drawModel(monkey);
    // Draw sphere
    loadIdentity();
    translate(0, -0.8, -2);
    drawModel(sphere);
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
