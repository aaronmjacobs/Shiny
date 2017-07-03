#version 330 core

in vec3 vNormal;

out vec4 color;

void main() {
	color = vec4((vNormal + 1.0) * 0.5, 1.0f);
}
