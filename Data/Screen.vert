#version 330 core

in vec3 aPosition;

out vec2 vTexCoord;

void main() {
   gl_Position = vec4(aPosition.xy, 0.0, 1.0);
   vTexCoord = (aPosition.xy + 1.0) / 2.0;
}
