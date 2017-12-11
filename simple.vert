#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec4 fragmentColor;

uniform mat4 modelView;
uniform mat4 projection;

void main() {
    gl_Position = projection * modelView * vec4(position, 1);
    fragmentColor = vec4(color, 1);
}
