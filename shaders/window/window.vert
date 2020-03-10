#version 400 core

layout(location = 0) in vec3 in_position;

// This needs to be at location 5 for glViewport to work.
layout(location = 5) in vec2 in_uv;

out vec2 uv;

void main() {
    uv = in_uv;
    gl_Position = vec4(in_position, 1.0);
}
