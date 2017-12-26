#version 330 core

uniform mat4 modelViewInverseTranspose;
uniform mat4 modelView;
uniform mat4 projection;
uniform sampler2D tex;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 textureCoordiante;
layout (location = 2) in vec3 normal;

out vec3 fragmentPosition;
out vec4 fragmentColor;
out vec3 fragmentNormal;

void main() {
    vec4 worldPosition = modelView * vec4(position, 1);
    gl_Position = projection * worldPosition;
    float d = length(worldPosition);
    fragmentPosition = gl_Position.xyz / gl_Position.z;
    fragmentColor = texture(tex, vec2(textureCoordiante.x, 1 - textureCoordiante.y)) / d;
    fragmentNormal = (modelViewInverseTranspose * vec4(normal, 1)).xyz;
}
