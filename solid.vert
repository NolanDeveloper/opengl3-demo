#version 330 core

uniform mat4 modelViewInverseTranspose;
uniform mat4 modelView;
uniform mat4 projection;
uniform vec3 lightPos;

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexTextureCoordiante;
layout (location = 2) in vec3 vertexNormal;

out vec3 normal;
out vec3 toLight;
out vec3 toCamera;
out vec2 texcoords;

void main() {
    // position in world space
    vec4 worldPosition = modelView * vec4(vertexPosition, 1);
    // normal in world space
    normal = normalize(modelViewInverseTranspose * vec4(vertexNormal, 1)).xyz;
    // direction to light
    toLight = normalize(lightPos - worldPosition.xyz);
    // direction to camera
    toCamera = normalize(-worldPosition.xyz);
    // texture coordinates to fragment shader
    texcoords = vertexTextureCoordiante.xy;
    // screen space coordinates of the vertex
    gl_Position = projection * modelView * worldPosition;
}
