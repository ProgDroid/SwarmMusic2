#version 330 core
struct Material {
    vec3  ambient;
    vec3  diffuse;
    vec3  specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColour;

in vec3 fragPos;
in vec3 normal;
in vec3 LightPos;
in vec3 colour;

uniform Light light;

uniform vec3 specular;
uniform float shininess;

void main() {
    Material material = Material(colour, colour, specular, shininess);

    // ambient
    vec3 ambient = light.ambient * material.ambient;

    // diffuse
    vec3  norm     = normalize(normal);
    vec3  lightDir = normalize(LightPos - fragPos);
    float diff     = max(dot(norm, lightDir), 0.0);
    vec3  diffuse  = light.diffuse * (diff * material.diffuse);

    // specular
    vec3  viewDir    = normalize(-fragPos);
    vec3  reflectDir = reflect(-lightDir, norm);
    float spec       = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3  specular   = light.specular * (spec * material.specular);

    vec3 result  = ambient + diffuse + specular;
    FragColour   = vec4(result, 1.0);
}
