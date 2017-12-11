#version 330 core

in vec4 fragmentColor;

uniform float lineGap;
uniform float lineWidth;

void main() {
    if (mod(gl_FragCoord.x, lineGap) < lineWidth ||
            mod(gl_FragCoord.y, lineGap) < lineWidth) {
        gl_FragColor = vec4(1.0);
    } else {
        gl_FragColor = fragmentColor;
    }
}
