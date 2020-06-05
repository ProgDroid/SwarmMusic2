#version 330 core
out vec4 FragColor;

in vec2 texCoord;

uniform float greenColour;
uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;
uniform float howBricky;

void main() {
    FragColor = mix(texture(ourTexture, texCoord), texture(ourTexture2, texCoord), howBricky);
    FragColor.y = greenColour;
}
