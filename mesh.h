struct Mesh {
    GLuint vao;
    GLuint positionBuffer;
    GLuint colorBuffer;
    GLuint indexBuffer;
    unsigned indexCount;
};

extern struct Mesh createMesh(
    Vec3 * positions, Vec3 * colors, GLuint * indices,
    unsigned vertexCount, unsigned indexCount);

extern void drawMesh(struct Mesh mesh);
