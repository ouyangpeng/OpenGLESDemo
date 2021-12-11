#version 300 es
precision mediump float;

struct Light {
    vec3 position;
    vec3 direction;
    vec3 color;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;
};

in vec3 normal;
in vec3 fragPos;
in vec2 v_texCoord;

layout(location = 0) out vec4 outColor;

uniform sampler2D s_TextureMap;

uniform vec3 viewPos;
uniform Light light;

void main()
{
    vec4 objectColor = texture(s_TextureMap, v_texCoord);

    vec3 lightDir = normalize(light.position - fragPos);

    // Check if lighting is inside the spotlight cone
    float theta = dot(lightDir, normalize(-light.direction));

    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // Ambient
    float ambientStrength = 0.25;
    vec3 ambient = ambientStrength * light.color;

    // Diffuse
    vec3 norm = normalize(normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color;

    // Specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = spec * light.color;

    // Attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // ambient  *= attenuation;
    // Also remove attenuation from ambient, because if we move too far,
    // the light in spotlight would then be darker than outside
    // (since outside spotlight we have ambient lighting).

    diffuse  *= attenuation;
    specular *= attenuation;

    diffuse *= intensity;
    specular*= intensity;

    vec3 finalColor = (ambient + diffuse + specular) * vec3(objectColor);
    outColor = vec4(finalColor, 1.0f);
}