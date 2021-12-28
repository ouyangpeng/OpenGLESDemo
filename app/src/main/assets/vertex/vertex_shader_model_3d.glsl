#version 300 es
precision mediump float;
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoord;

out vec2 v_texCoord;

uniform mat4 u_MVPMatrix;
uniform mat4 u_ModelMatrix;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

out vec3 ambient;
out vec3 diffuse;
out vec3 specular;

void main()
{
    v_texCoord = a_texCoord;
    vec4 position = vec4(a_position, 1.0);
    gl_Position = u_MVPMatrix * position;
    vec3 fragPos = vec3(u_ModelMatrix * position);

    // Ambient
    float ambientStrength = 0.25;
    ambient = ambientStrength * lightColor;

    // Diffuse
    float diffuseStrength = 0.5;
    vec3 unitNormal = normalize(vec3(u_ModelMatrix * vec4(a_normal, 1.0)));
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(unitNormal, lightDir), 0.0);
    diffuse = diffuseStrength * diff * lightColor;

    // Specular
    float specularStrength = 0.3;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, unitNormal);
    float spec = pow(max(dot(unitNormal, reflectDir), 0.0), 16.0);
    specular = specularStrength * spec * lightColor;
}