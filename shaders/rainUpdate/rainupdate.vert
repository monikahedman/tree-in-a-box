#version 400 core

// This is just a basic fullscreen quad vertex shader.

layout(location = 0) in vec3 in_position;
layout(location = 5) in vec2 in_uv;
out vec2 uv;

void main() {
    uv = in_uv;
    gl_Position = vec4(in_position, 1.0);
}
