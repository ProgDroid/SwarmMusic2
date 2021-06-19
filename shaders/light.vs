#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in mat4 instanceMatrix;
layout (location = 6) in vec4 aColour;

out vec3 fragPos;
out vec3 normal;
out vec3 LightPos;
out vec3 colour;

uniform vec3 lightPos;

uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0);
    fragPos     = vec3(view * instanceMatrix * vec4(aPos, 1.0));
    normal      = mat3(transpose(inverse(view * instanceMatrix))) * aNormal;
    LightPos    = vec3(view * vec4(lightPos, 1.0));
    colour      = aColour.xyz;
}
