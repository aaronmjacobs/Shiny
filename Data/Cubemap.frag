#version 330 core

in vec3 vPosition;
in vec2 vTexCoord;

out vec4 color;

uniform samplerCube uTexture;

void main() {
	color = texture(uTexture, vPosition);
}
