#version 330 core

uniform vec3 lightPos;

in vec3 fragmentPosition;
in vec4 fragmentColor;
in vec3 fragmentNormal;

vec3 ambientColor = vec3(0.2);
vec3 diffuseColor = vec3(0.6);
vec3 specularColor = vec3(0.8);
vec3 lightColor = vec3(1);
float shininess = 10;

void main() {
    vec3 normal = -normalize(fragmentNormal);
    vec3 lightDir = lightPos - fragmentPosition;
    float distance = length(lightDir);
    distance *= distance;
    lightDir = normalize(lightDir);
    float lambertian = max(dot(lightDir, normal), 0.0);
    float specular = 0.0;
    if(lambertian > 0.0) {
        vec3 viewDir = normalize(-fragmentPosition);
        vec3 halfDir = normalize(lightDir + viewDir);
        float specAngle = max(dot(halfDir, normal), 0.0);
        specular = pow(specAngle, shininess);
    }
    vec3 colorLinear = ambientColor + 
        (diffuseColor * lambertian + specularColor * specular) * lightColor;
    gl_FragColor = fragmentColor * vec4(colorLinear, 1.0);
}
