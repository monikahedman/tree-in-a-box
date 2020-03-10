#version 400 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;   // Normal of the vertex
layout(location = 5) in vec2 texCoord; // UV texture coordinates

// Transformation matrices
uniform mat4 p;
uniform mat4 v;
uniform mat4 m;

out vec2 uv;
out vec4 vertexNormal;
//out vec4 screenPos;

void main() {
    uv = texCoord;

    vec4 position_cameraSpace = v * m * vec4(position, 1.0);
    vec4 normal_cameraSpace = vec4(normalize(mat3(transpose(inverse(v * m))) * normal), 0);

    vec4 position_worldSpace = m * vec4(position, 1.0);
    vec4 normal_worldSpace = vec4(normalize(mat3(transpose(inverse(m))) * normal), 0);

    vertexNormal = normal_worldSpace;
    gl_Position = p * position_cameraSpace;
}
