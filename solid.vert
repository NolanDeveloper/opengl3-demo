#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 textureCoordiante;
layout (location = 2) in vec3 normal;

out vec4 fragmentColor;

uniform mat4 modelView;
uniform mat4 projection;
uniform sampler2D tex;

// vec3 lightDirection = vec3(1);

void main() {
    vec4 worldPosition = modelView * vec4(position, 1);
    gl_Position = projection * worldPosition;
    fragmentColor = texture(tex, vec2(textureCoordiante.x, 1 - textureCoordiante.y))
        / length(worldPosition);
}
