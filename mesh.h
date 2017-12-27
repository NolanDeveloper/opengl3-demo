struct Material {
    GLfloat ambient[3];
    GLfloat diffuse[3];
    GLfloat specular[3];
    GLfloat shininess;
};

struct Mesh {
    GLuint vao;
    GLuint positionsBuffer;
    GLuint textureCoordinatesBuffer;
    GLuint normalsBuffer;
    GLuint texture;
    struct Material material;
    unsigned numberOfVertices;
};

extern struct Mesh createMeshFromObj(char * filepath, GLuint texture);

extern void drawMesh(struct Mesh mesh);
