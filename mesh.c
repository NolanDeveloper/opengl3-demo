#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "geometry.h"
#include "mesh.h"
#include "utils.h"

extern void
drawMesh(struct Mesh mesh) {
    glBindVertexArray(mesh.vao);
    glDrawArrays(GL_TRIANGLES, 0, mesh.numberOfVertices);
}

static char * fileContent;
static unsigned long currentLine;
static unsigned long currentColumn;
static unsigned long currentOffset;
static unsigned long oldLine;
static unsigned long oldColumn;
static unsigned long oldOffset;

#define peekChar() (fileContent[currentOffset])

static void
error(void) {
    printf("Oh shi at %d:%d...\n", currentLine, currentColumn);
    exit(1);
}

static void
consumeChar(void) {
    if (!peekChar()) error();
    ++currentColumn;
    if ('\n' == peekChar()) {
        ++currentLine;
        currentColumn = 0;
    }
    ++currentOffset;
}

static void 
rememberPosition(void) {
    oldOffset = currentOffset;
    oldLine = currentLine;
    oldColumn = currentColumn;
}

static void 
restorePosition(void) {
    currentOffset = oldOffset;
    currentLine = oldLine;
    currentColumn = oldColumn;
}

static void
reset(void) {
    currentOffset = 0;
    currentLine = 0;
    currentColumn = 0;
}

struct VertexAttributeIndices {
    unsigned long indexOfPosition;
    unsigned long indexOfTextureCoordinate;
    unsigned long indexOfNormal;
};

struct Face {
    struct VertexAttributeIndices * vertices;
    unsigned numberOfVertices;
};

#define skipSpaces() do { while (isspace(peekChar()) && '\n' != peekChar()) consumeChar(); } while (0)

static void
skipSpacesAndComments(void) {
    while (isspace(peekChar()) || '#' == peekChar()) {
        while (isspace(peekChar())) consumeChar(); 
        if ('#' == peekChar()) {
            while ('\n' != peekChar()) consumeChar();
        }
    }
}

static int
accept(const char * line) {
    int n = strlen(line);
    int result = strncmp(&fileContent[currentOffset], line, n);
    if (!result) for (int i = 0; i < n; ++i) consumeChar();
    return result;
}

static int
parseFloat(GLfloat * out) {
    skipSpaces();
    char * endptr;
    char * beginptr = &fileContent[currentOffset];
    GLfloat t = (GLfloat)strtof(beginptr, &endptr);
    if (out) *out = t;
    currentOffset += endptr - beginptr;
    return endptr == beginptr;
}

static int
parseUnsignedLong(unsigned long * out) {
    skipSpaces();
    char * endptr;
    char * beginptr = &fileContent[currentOffset];
    unsigned long t = strtoul(beginptr, &endptr, 10);
    if (out) *out = t;
    currentOffset += endptr - beginptr;
    return endptr == beginptr;
}

static int 
parsePosition(Vec3 * out) {
    GLfloat x, y, z, w;
    skipSpacesAndComments();
    if (accept("v ")) return 1;
    if (parseFloat(&x)) error();
    if (parseFloat(&y)) error();
    if (parseFloat(&z)) error();
    if (parseFloat(&w)) w = 1;
    if (!w) error();
    if (accept("\n")) error();
    if (out) {
        out->x = x / w;
        out->y = y / w;
        out->z = z / w;
    }
    return 0;
}

static int 
parseTextureCoordinate(Vec3 * out) {
    skipSpacesAndComments();
    if (accept("vt ")) return 1;
    if (parseFloat(out ? &out->x : NULL)) error();
    if (parseFloat(out ? &out->y : NULL)) error();
    parseFloat(NULL);
    if (accept("\n")) error();
    return 0;
}

static int
parseNormal(Vec3 * out) {
    GLfloat x, y, z;
    skipSpacesAndComments();
    if (accept("vn ")) return 1;
    if (parseFloat(out ? &out->x : NULL)) error();
    if (parseFloat(out ? &out->y : NULL)) error();
    if (parseFloat(out ? &out->z : NULL)) error();
    if (accept("\n")) error();
    return 0;
}

static int
parseVertexAttributeIndices(struct VertexAttributeIndices * out) {
    unsigned long position = -1, textureCoordinate = -1, normal = -1;
    if (parseUnsignedLong(&position)) return 1;
    if (!accept("//")) {
        if (parseUnsignedLong(&normal)) error();
    } else if (!accept("/")) {
        if (parseUnsignedLong(&textureCoordinate)) error();
        if (!accept("/")) {
            if (parseUnsignedLong(&normal)) error();
        }
    } 
    if (out) {
        out->indexOfPosition = position - 1;
        out->indexOfTextureCoordinate = textureCoordinate - 1;
        out->indexOfNormal = normal - 1;
    }
    return 0;
}

static int
parseFace(struct Face * out) {
    skipSpacesAndComments();
    if (accept("f")) return 1;
    if (out) {
        rememberPosition();
        out->numberOfVertices = 0;
        while (!parseVertexAttributeIndices(NULL)) ++out->numberOfVertices;
        if (out->numberOfVertices < 3) error();
        restorePosition();
        out->vertices = emalloc(out->numberOfVertices * sizeof(struct VertexAttributeIndices));
    }
    for (int i = 0; !parseVertexAttributeIndices(out ? &out->vertices[i] : NULL); ++i);
    return 0;
}

static GLuint
createBuffer(void * data, size_t size) {
    GLuint id;
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    return id;
}

extern struct Mesh 
createMeshFromObj(char * filepath) {
    fileContent = loadFile(filepath);
    unsigned long numberOfPositions          = 0;
    unsigned long numberOfTextureCoordinates = 0;
    unsigned long numberOfNormals            = 0;
    unsigned long numberOfFaces              = 0;
    while (!parsePosition(NULL))          ++numberOfPositions;
    while (!parseTextureCoordinate(NULL)) ++numberOfTextureCoordinates;
    while (!parseNormal(NULL))            ++numberOfNormals;
    while (!parseFace(NULL))              ++numberOfFaces;
    reset();
    Vec3 * positions          = emalloc(numberOfPositions          * sizeof(Vec3));
    Vec3 * textureCoordinates = emalloc(numberOfTextureCoordinates * sizeof(Vec3));
    Vec3 * normals            = emalloc(numberOfNormals            * sizeof(Vec3));
    struct Face * faces       = emalloc(numberOfFaces              * sizeof(struct Face));
    for (int i = 0; !parsePosition(&positions[i]);                   ++i);
    for (int i = 0; !parseTextureCoordinate(&textureCoordinates[i]); ++i);
    for (int i = 0; !parseNormal(&normals[i]);                       ++i);
    unsigned long numberOfTriangles = 0;
    for (int i = 0; !parseFace(&faces[i]); ++i) {
        numberOfTriangles += faces[i].numberOfVertices - 2;
    }
    unsigned long numberOfVertices = numberOfTriangles * 3;
    size_t bufferSize = numberOfVertices * sizeof(Vec3);
    Vec3 * p, * t, * n;
    Vec3 * positionsBufferData          = p = emalloc(bufferSize);
    Vec3 * textureCoordinatesBufferData = t = emalloc(bufferSize);
    Vec3 * normalsBufferData            = n = emalloc(bufferSize);
    for (int i = 0; i < numberOfFaces; ++i) {
        struct VertexAttributeIndices * vertices = faces[i].vertices;
        for (int j = 1; j < faces[i].numberOfVertices - 1; ++j) {
            *p++ = positions[vertices[0].indexOfPosition];
            *p++ = positions[vertices[j].indexOfPosition];
            *p++ = positions[vertices[j + 1].indexOfPosition];
            *t++ = textureCoordinates[vertices[0].indexOfTextureCoordinate];
            *t++ = textureCoordinates[vertices[j].indexOfTextureCoordinate];
            *t++ = textureCoordinates[vertices[j + 1].indexOfTextureCoordinate];
            *n++ = normals[vertices[0].indexOfNormal];
            *n++ = normals[vertices[j].indexOfNormal];
            *n++ = normals[vertices[j + 1].indexOfNormal];
        }
    }
    struct Mesh mesh;
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);
    mesh.numberOfVertices         = numberOfVertices;
    mesh.positionsBuffer          = createBuffer(positionsBufferData, bufferSize);
    mesh.textureCoordinatesBuffer = createBuffer(textureCoordinatesBufferData, bufferSize);
    mesh.normalsBuffer            = createBuffer(normalsBufferData, bufferSize);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.positionsBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.textureCoordinatesBuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.normalsBuffer);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, 0, NULL);
    free(positions);
    free(textureCoordinates);
    free(normals);
    for (int i = 0; i < numberOfFaces; ++i) {
        free(faces[i].vertices);
    }
    free(faces);
    free(positionsBufferData);
    free(textureCoordinatesBufferData);
    free(normalsBufferData);
    return mesh;
}
