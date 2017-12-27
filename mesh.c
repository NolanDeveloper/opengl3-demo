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
    glBindTexture(GL_TEXTURE_2D, mesh.texture);
    glDrawArrays(GL_TRIANGLES, 0, mesh.numberOfVertices);
}

struct FilePosition {
    unsigned long line;
    unsigned long column;
    unsigned long offset;
};

static char * fileContent;
static struct FilePosition currentPosition, oldPosition;

#define peekChar() (fileContent[currentPosition.offset])

static void
error(void) {
    printf("Error at %ld:%ld...\n", currentPosition.line, currentPosition.column);
    exit(1);
}

static void
consumeChar(void) {
    if (!peekChar()) error();
    ++currentPosition.column;
    if ('\n' == peekChar()) {
        ++currentPosition.line;
        currentPosition.column = 0;
    }
    ++currentPosition.offset;
}

static void 
rememberPosition(void) {
    oldPosition = currentPosition;
}

static void 
restorePosition(void) {
    currentPosition = oldPosition;
}

static void
reset(void) {
    currentPosition.offset = 0;
    currentPosition.line = 0;
    currentPosition.column = 0;
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

#define skipSpaces() \
    do { while (isspace(peekChar()) && '\n' != peekChar()) consumeChar(); } while (0)

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
    int result = strncmp(&fileContent[currentPosition.offset], line, n);
    if (!result) for (int i = 0; i < n; ++i) consumeChar();
    return result;
}

static int
parseFloat(GLfloat * out) {
    skipSpaces();
    char * endptr;
    char * beginptr = &fileContent[currentPosition.offset];
    GLfloat t = (GLfloat)strtof(beginptr, &endptr);
    if (out) *out = t;
    currentPosition.offset += endptr - beginptr;
    return endptr == beginptr;
}

static int
parseUnsignedLong(unsigned long * out) {
    skipSpaces();
    char * endptr;
    char * beginptr = &fileContent[currentPosition.offset];
    unsigned long t = strtoul(beginptr, &endptr, 10);
    if (out) *out = t;
    currentPosition.offset += endptr - beginptr;
    return endptr == beginptr;
}

static int 
parsePosition(GLfloat * out) {
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
        out[0] = x / w;
        out[1] = y / w;
        out[2] = z / w;
    }
    return 0;
}

static int 
parseTextureCoordinate(GLfloat * out) {
    skipSpacesAndComments();
    if (accept("vt ")) return 1;
    if (parseFloat(out ? &out[0] : NULL)) error();
    if (parseFloat(out ? &out[1] : NULL)) error();
    parseFloat(NULL);
    if (accept("\n")) error();
    return 0;
}

static int
parseNormal(GLfloat * out) {
    skipSpacesAndComments();
    if (accept("vn ")) return 1;
    if (parseFloat(out ? &out[0] : NULL)) error();
    if (parseFloat(out ? &out[1] : NULL)) error();
    if (parseFloat(out ? &out[2] : NULL)) error();
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
        out->indexOfPosition          = position - 1;
        out->indexOfTextureCoordinate = textureCoordinate - 1;
        out->indexOfNormal            = normal - 1;
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
createMeshFromObj(char * filepath, GLuint texture) {
    fileContent = loadFile(filepath);
    reset();
    unsigned long numberOfPositions          = 0;
    unsigned long numberOfTextureCoordinates = 0;
    unsigned long numberOfNormals            = 0;
    unsigned long numberOfFaces              = 0;
    while (!parsePosition(NULL))          ++numberOfPositions;
    while (!parseTextureCoordinate(NULL)) ++numberOfTextureCoordinates;
    while (!parseNormal(NULL))            ++numberOfNormals;
    while (!parseFace(NULL))              ++numberOfFaces;
    reset();
    GLfloat (*positions)[3]          = emalloc(numberOfPositions          * sizeof(GLfloat[3]));
    GLfloat (*textureCoordinates)[3] = emalloc(numberOfTextureCoordinates * sizeof(GLfloat[3]));
    GLfloat (*normals)[3]            = emalloc(numberOfNormals            * sizeof(GLfloat[3]));
    struct Face * faces              = emalloc(numberOfFaces              * sizeof(struct Face));
    for (int i = 0; !parsePosition((GLfloat *)&positions[i]);                   ++i);
    for (int i = 0; !parseTextureCoordinate((GLfloat *)&textureCoordinates[i]); ++i);
    for (int i = 0; !parseNormal((GLfloat *)&normals[i]);                       ++i);
    unsigned long numberOfTriangles = 0;
    for (int i = 0; !parseFace(&faces[i]); ++i) {
        numberOfTriangles += faces[i].numberOfVertices - 2;
    }
    unsigned long numberOfVertices = numberOfTriangles * 3;
    size_t bufferSize = numberOfVertices * sizeof(GLfloat[3]);
    GLfloat (*p)[3], (*t)[3], (*n)[3];
    GLfloat (*positionsBufferData)[3]          = p = emalloc(bufferSize);
    GLfloat (*textureCoordinatesBufferData)[3] = t = emalloc(bufferSize);
    GLfloat (*normalsBufferData)[3]            = n = emalloc(bufferSize);
    for (unsigned long i = 0; i < numberOfFaces; ++i) {
        struct VertexAttributeIndices * vertices = faces[i].vertices;
        for (unsigned long j = 1; j < faces[i].numberOfVertices - 1; ++j) {
            memcpy(p++, &positions[vertices[0].indexOfPosition], sizeof(GLfloat[3]));
            memcpy(p++, &positions[vertices[j].indexOfPosition], sizeof(GLfloat[3]));
            memcpy(p++, &positions[vertices[j + 1].indexOfPosition], sizeof(GLfloat[3]));
            memcpy(t++, &textureCoordinates[vertices[0].indexOfTextureCoordinate], sizeof(GLfloat[3]));
            memcpy(t++, &textureCoordinates[vertices[j].indexOfTextureCoordinate], sizeof(GLfloat[3]));
            memcpy(t++, &textureCoordinates[vertices[j + 1].indexOfTextureCoordinate], sizeof(GLfloat[3]));
            memcpy(n++, &normals[vertices[0].indexOfNormal], sizeof(GLfloat[3]));
            memcpy(n++, &normals[vertices[j].indexOfNormal], sizeof(GLfloat[3]));
            memcpy(n++, &normals[vertices[j + 1].indexOfNormal], sizeof(GLfloat[3]));
        }
    }
    struct Mesh mesh;
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);
    mesh.texture                  = texture;
    mesh.numberOfVertices         = numberOfVertices;
    mesh.positionsBuffer          = createBuffer(positionsBufferData, bufferSize);
    mesh.textureCoordinatesBuffer = createBuffer(textureCoordinatesBufferData, bufferSize);
    mesh.normalsBuffer            = createBuffer(normalsBufferData, bufferSize);
    struct Material material = {
        .ambient = { 0.2, 0.2, 0.2 },
        .diffuse = { 0.6, 0.6, 0.6 },
        .specular = { 0.8, 0.8, 0.8 },
        .shininess = 100,
    };
    mesh.material = material;
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
    for (unsigned long i = 0; i < numberOfFaces; ++i) {
        free(faces[i].vertices);
    }
    free(faces);
    free(positionsBufferData);
    free(textureCoordinatesBufferData);
    free(normalsBufferData);
    return mesh;
}
