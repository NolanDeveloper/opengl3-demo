#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;
uniform sampler2D tex;

in vec3 normal;
in vec3 toLight;
in vec3 toCamera;
in vec2 texcoords;

out vec4 resultingColor;

Material light = Material(vec3(1, 1, 1), vec3(1, 1, 1), vec3(1, 1, 1), 0);

// returns intensity of reflected ambient lighting
vec3 ambientLighting() {
   return material.ambient * light.ambient;
}

// returns intensity of diffuse reflection
vec3 diffuseLighting(in vec3 N, in vec3 L) {
   // calculation as for Lambertian reflection
   float diffuseTerm = clamp(dot(N, L), 0, 1) ;
   return material.diffuse * light.diffuse * diffuseTerm;
}

// returns intensity of specular reflection
vec3 specularLighting(in vec3 N, in vec3 L, in vec3 V) {
   float specularTerm = 0;
   // calculate specular reflection only if
   // the surface is oriented to the light source
   if(dot(N, L) > 0)
   {
      // half vector
      vec3 H = normalize(L + V);
      specularTerm = pow(dot(N, H), material.shininess);
   }
   return material.specular * light.specular * specularTerm;
}

void main(void) {
   // normalize vectors after interpolation
   vec3 L = normalize(toLight);
   vec3 V = normalize(toCamera);
   vec3 N = normalize(normal);
   // get Blinn-Phong reflectance components
   vec3 Iamb = ambientLighting();
   vec3 Idif = diffuseLighting(N, L);
   vec3 Ispe = specularLighting(N, L, V);
   // diffuse color of the object from texture
   vec3 diffuseColor = texture(tex, vec2(texcoords.x, 1 - texcoords.y)).rgb;
   // combination of all components and diffuse color of the object
   resultingColor.xyz = diffuseColor * (Iamb + Idif + Ispe);
   resultingColor.a = 1;
} 
