#version 400 core

uniform sampler2D positions;

// These matrices are needed to place the particles in 3D space.
uniform mat4 p;
uniform mat4 v;
out vec2 uv;
out vec4 basePosition;

// These constants define the triangle that's drawn.
const int NUM_VERTICLES_PER_PARTICLE = 3;
const vec4 VERTICES[NUM_VERTICLES_PER_PARTICLE] = vec4[NUM_VERTICLES_PER_PARTICLE](
    vec4(0.0, 1.0, 0.0, 0.0),
    vec4(0.0, 0.0, 0.0, 0.0),
    vec4(1.0, 0.0, 0.0, 0.0)
);
const float PARTICLE_SIZE = 0.4;

float r(float n) {
    return fract(sin(n) * 753.5453123);
}

void main(void)
{
    int particleID = gl_VertexID / NUM_VERTICLES_PER_PARTICLE;
    int triangleID = gl_VertexID % NUM_VERTICLES_PER_PARTICLE;

    basePosition = texelFetch(positions, ivec2(particleID, 0), 0);

    // Calculates the triangle's position.
    uv = VERTICES[triangleID].xy;
    float angle = 6.28 * r(float(particleID) * 420.2);
    float x = uv.x * PARTICLE_SIZE;

    vec4 position = basePosition + vec4(x * sin(angle), uv.y * PARTICLE_SIZE, x * cos(angle), 1.0);
    gl_Position = p * v * position;
}
