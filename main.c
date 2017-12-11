#include <math.h>
#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "geometry.h"
#include "mesh.h"

static struct Rect {
    int width;
    int height;
} window = { .width = 640, .height = 480 };

static struct Mesh icosahedron;
static struct Mesh cube;

static struct {
    GLuint id;
    GLuint modelViewLocation;
    GLuint projectionLocation;
} solidShader;

static struct {
    GLuint id;
    GLuint modelViewLocation;
    GLuint projectionLocation;
    GLuint lineGapLocation;
    GLuint lineWidthLocation;
} hatchShader;

static GLuint modelViewLocation;
static GLuint projectionLocation;
static GLuint lineGapLocation;
static GLuint lineWidthLocation;

static const double PI = atan(1.) * 4.;

static char *
loadFile(const char * filepath) {
    FILE * f = fopen(filepath, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char * string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);
    string[fsize] = 0;
    return string;
}

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
    GLuint vertexShader =
        compileShaderFile(vertexShaderPath, GL_VERTEX_SHADER);
    GLuint fragmentShader =
        compileShaderFile(fragmentShaderPath, GL_FRAGMENT_SHADER);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    return shaderProgram;
}

static void
debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
        GLsizei length, const GLchar * message, const void * userParam) {
    // if (GL_DEBUG_SEVERITY_HIGH != severity) return;
    printf("opengl log: %s\n", message);
}

static float angle = 0;
static Mat4 modelView;
static Mat4 projection;

static double randomDouble() { return (double)rand() / RAND_MAX; }

static struct Mesh
createIcosahedron(float size) {
    GLfloat R = (size * sqrt(2.0 * (5.0 + sqrt(5.0)))) / 4;
    GLfloat r = (size * sqrt(3.0) * (3.0 + sqrt(5.0))) / 12;
    Vec3 positions[12];
    Vec3 colors[12];
    GLuint indices[60];
    for (int i = 0; i < 5; ++i) {
        positions[2 * i] = (Vec3) {
            r * cos(2 * PI / 5 * i),
            R / 2,
            r * sin(2 * PI / 5 * i),
        };
        positions[2 * i + 1] = (Vec3) {
            r * cos(2 * PI / 5 * i + 2 * PI / 10),
            -R / 2,
            r * sin(2 * PI / 5 * i + 2 * PI / 10),
        };
        colors[2 * i] = (Vec3) {
            randomDouble(), randomDouble(), randomDouble()
        };
        colors[2 * i + 1] = (Vec3) {
            randomDouble(), randomDouble(), randomDouble()
        };
    }
    positions[10] = (Vec3) { 0, R, 0 };
    positions[11] = (Vec3) { 0, -R, 0 };
    colors[10] = (Vec3) { randomDouble(), randomDouble(), randomDouble() };
    colors[11] = (Vec3) { randomDouble(), randomDouble(), randomDouble() };
    for (int i = 0; i < 10; i += 2) {
        indices[3 * i + 0] = (i + 1) % 10;
        indices[3 * i + 1] = (i + 2) % 10;
        indices[3 * i + 2] = i;
    }
    for (int i = 1; i < 10; i += 2) {
        indices[3 * i + 0] = (i + 1) % 10;
        indices[3 * i + 1] = i;
        indices[3 * i + 2] = (i + 2) % 10;
    }
    for (int i = 0; i < 5; ++i)
    {
        indices[3 * (10 + 2 * i) + 0] = 10;
        indices[3 * (10 + 2 * i) + 1] = 2 * i;
        indices[3 * (10 + 2 * i) + 2] = (2 * (i + 1)) % 10;
        indices[3 * (10 + 2 * i + 1) + 0] = 11;
        indices[3 * (10 + 2 * i + 1) + 1] = (2 * (i + 1) + 1) % 10;
        indices[3 * (10 + 2 * i + 1) + 2] = 2 * i + 1;
    }
    return createMesh(positions, colors, indices, 12, 60);
}

static struct Mesh
createCube(float size) {
    float s = size / 2;
    Vec3 positions[8] = {
        (Vec3) { -s, -s, -s },
        (Vec3) { -s, s, -s },
        (Vec3) { s, s, -s },
        (Vec3) { s, -s, -s },
        (Vec3) { -s, -s, s },
        (Vec3) { -s, s, s },
        (Vec3) { s, s, s },
        (Vec3) { s, -s, s },
    };
    Vec3 colors[8];
    GLuint indices[36] = {
        3, 2, 1, 3, 1, 0,
        0, 1, 5, 0, 5, 4,
        1, 2, 6, 1, 6, 5,
        2, 3, 7, 2, 7, 6,
        3, 0, 4, 3, 4, 7,
        4, 5, 6, 4, 6, 7,
    };
    return createMesh(positions, colors, indices, 8, 36);
}

static void
initOpengl(void) {
    glEnable(GL_DEPTH_TEST);
    glDebugMessageCallback(debugCallback, NULL);
    glClearColor(0, 0, 0, 1);
    solidShader.id = compileShader("simple.vert", "simple.frag");
    solidShader.modelViewLocation =
        glGetUniformLocation(solidShader.id, "modelView");
    solidShader.projectionLocation =
        glGetUniformLocation(solidShader.id, "projection");
    hatchShader.id = compileShader("simple.vert", "hatch.frag");
    hatchShader.modelViewLocation =
        glGetUniformLocation(hatchShader.id, "modelView");
    hatchShader.projectionLocation =
        glGetUniformLocation(hatchShader.id, "projection");
    hatchShader.lineGapLocation =
        glGetUniformLocation(hatchShader.id, "lineGap");
    hatchShader.lineWidthLocation =
        glGetUniformLocation(hatchShader.id, "lineWidth");
    icosahedron = createIcosahedron(1);
    cube = createCube(1);
    perspective(&projection, -1, 1, -1, 1, 1, 100);
}

static void
display(void) {
    angle += 0.01;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(hatchShader.id);
    glUniformMatrix4fv(hatchShader.projectionLocation,
        1, GL_TRUE, (GLfloat *)&projection);

    Mat4 t;
    identity(&modelView);
    translation(&t, 1.5, 0, -3); mulm(&modelView, &modelView, &t);
    rotationY(&t, 3.1 * angle);  mulm(&modelView, &modelView, &t);
    rotationX(&t, 2 * angle);    mulm(&modelView, &modelView, &t);
    scale(&t,
        fabs(sin(angle)),
        fabs(cos(angle + 1)),
        fabs(cos(angle + 3) * sin(angle + 2)));
    mulm(&modelView, &modelView, &t);
    glUniformMatrix4fv(hatchShader.modelViewLocation,
        1, GL_TRUE, (GLfloat *)&modelView);
    glUniform1f(hatchShader.lineGapLocation, 20);
    GLfloat lineWidth = fabs(sin(angle + 1.1)) * 3 + 1;
    glUniform1f(hatchShader.lineWidthLocation, lineWidth);
    drawMesh(icosahedron);

    glUseProgram(solidShader.id);
    glUniformMatrix4fv(solidShader.projectionLocation,
        1, GL_TRUE, (GLfloat *)&projection);

    identity(&modelView);
    translation(&t, -1.5, 0, -3); mulm(&modelView, &modelView, &t);
    rotationY(&t, 3.1 * angle);   mulm(&modelView, &modelView, &t);
    rotationX(&t, 2 * angle);     mulm(&modelView, &modelView, &t);
    glUniformMatrix4fv(hatchShader.modelViewLocation,
        1, GL_TRUE, (GLfloat *)&modelView);
    drawMesh(cube);

    glutSwapBuffers();
}

int main(int argc, char * argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitContextVersion(4, 3);
    glutInitContextFlags(GLUT_CORE_PROFILE | GLUT_DEBUG);
    glutInitWindowSize(window.width, window.height);
    glutCreateWindow("Shaders");
    glewExperimental = GL_TRUE;
    glewInit();
    glutDisplayFunc(display);
    glutIdleFunc(display);
    initOpengl();
    glutMainLoop();
}