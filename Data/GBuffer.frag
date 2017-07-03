#version 330 core

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

void main() {
   gPosition = vPosition;
   gNormal = texture(normalTexture, vTexCoord).rgb;// normalize(vNormal);
   gAlbedoSpec.rgb = texture(diffuseTexture, vTexCoord).rgb;
   gAlbedoSpec.a = texture(specularTexture, vTexCoord).r;
}
