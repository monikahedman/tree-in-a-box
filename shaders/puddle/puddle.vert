#version 330 core

layout(location = 0) in vec3 position; // Position of the vertex
layout(location = 1) in vec3 normal;   // Normal of the vertex
layout(location = 5) in vec2 texCoord; // UV texture coordinates

out vec3 color; // Computed color for this vertex
out vec3 out_normal;
out vec2 texc;
out vec3 inPosition;

// Transformation matrices
uniform mat4 p;
uniform mat4 v;
uniform mat4 m;

// Light data
const int MAX_LIGHTS = 10;
uniform int lightTypes[MAX_LIGHTS];         // 0 for point, 1 for directional
uniform vec3 lightPositions[MAX_LIGHTS];    // For point lights
uniform vec3 lightDirections[MAX_LIGHTS];   // For directional lights
uniform vec3 lightColors[MAX_LIGHTS];

// Material data
uniform vec3 ambient_color;
uniform vec3 diffuse_color;
uniform vec2 repeatUV;

void main() {
    out_normal = normal;
    inPosition = position;
    texc = texCoord * repeatUV;

    vec4 position_cameraSpace = v * m * vec4(position, 1.0);
    vec4 normal_cameraSpace = vec4(normalize(mat3(transpose(inverse(v * m))) * normal), 0);

    vec4 position_worldSpace = m * vec4(position, 1.0);
    vec4 normal_worldSpace = vec4(normalize(mat3(transpose(inverse(m))) * normal), 0);

    gl_Position = p * position_cameraSpace;

    // Adds the ambient component.
    color = ambient_color.xyz;

    // Adds the diffuse component.
    for (int i = 0; i < MAX_LIGHTS; i++) {
        vec4 vertexToLight = vec4(0);
        if (lightTypes[i] == 0) {
            // The light is a point light.
            vertexToLight = normalize(v * vec4(lightPositions[i], 1) - position_cameraSpace);
        } else if (lightTypes[i] == 1) {
            // The light is a directional light.
            vertexToLight = normalize(v * vec4(-lightDirections[i], 0));
        }
        float diffuseIntensity = max(0.0, dot(vertexToLight, normal_cameraSpace));
        color += max(vec3(0), lightColors[i] * diffuse_color * diffuseIntensity);
    }
//    color = clamp(color, 0.0, 1.0);
    color = ambient_color.xyz;
}
