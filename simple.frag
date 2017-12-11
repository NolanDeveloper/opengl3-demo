#version 330 core

in vec4 fragmentColor;

uniform float lineGap;
uniform float lineWidth;

void main() {
    gl_FragColor = fragmentColor;
}
