#version 330 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aCol;
layout (location = 2) in mat4 instanceMatrix;

out vec3 colour;

uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * instanceMatrix * vec4(aPos);
    colour      = aCol.xyz;
}
