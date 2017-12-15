#include <GL/glew.h>
#include <GL/freeglut.h>

#include "geometry.h"
#include "mesh.h"

static GLuint
createBuffer(void * data, size_t size, GLenum target, GLenum usage) {
    GLuint id;
    glGenBuffers(1, &id);
    glBindBuffer(target, id);
    glBufferData(target, size, data, usage);
    return id;
}

extern struct Mesh
createMesh(Vec3 * positions, Vec3 * colors, GLuint * indices,
        unsigned vertexCount, unsigned indexCount) {
    struct Mesh mesh;
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);
    mesh.positionBuffer =
        createBuffer(positions, vertexCount * sizeof(Vec3), GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    mesh.colorBuffer =
        createBuffer(colors, vertexCount * sizeof(Vec3), GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    mesh.indexBuffer =
        createBuffer(indices, indexCount * sizeof(Vec3), GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
    mesh.indexCount = indexCount;
    glBindBuffer(GL_ARRAY_BUFFER, mesh.positionBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.colorBuffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBuffer);
    glEnableVertexAttribArray(1);
    return mesh;
}

extern void
drawMesh(struct Mesh mesh) {
    glBindVertexArray(mesh.vao);
    glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, NULL);
}
