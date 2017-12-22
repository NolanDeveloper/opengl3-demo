struct Mesh {
    GLuint vao;
    GLuint positionsBuffer;
    GLuint textureCoordinatesBuffer;
    GLuint normalsBuffer;
    unsigned numberOfVertices;
};

extern struct Mesh createMeshFromObj(char * filepath);

extern void drawMesh(struct Mesh mesh);
