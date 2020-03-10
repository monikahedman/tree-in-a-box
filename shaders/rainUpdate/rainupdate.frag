#version 400 core

uniform bool reset;
uniform sampler2D previousPositions;
uniform sampler2D previousVelocities;
uniform int numParticles;

in vec2 uv;
out vec4 position;
out vec4 velocity;

const float BOX_RADIUS = 12.0;

// Generates a random number between 0 and 1.
float random(float n) {
    return fract(sin(n) * 753.5453123);
}

vec4 initPosition(int index) {
    vec4 result = vec4(0.0, 0.0, 0.0, 1.0);
    result.x += 2.0 * BOX_RADIUS * random(float(index * 382.68)) - BOX_RADIUS;
    result.y += 2.0 * BOX_RADIUS * random(float(index * 123.13)) - BOX_RADIUS;
    result.z += 2.0 * BOX_RADIUS * random(float(index * 431.39)) - BOX_RADIUS;
    return result;
}

const float HORIZONTAL_VELOCITY = 0.01;
vec4 initVelocity(int index) {
    vec4 result = vec4(0.0, -0.05, 0.0, 0.0);
    result.x += 2.0 * HORIZONTAL_VELOCITY * random(float(index * 244.67)) - HORIZONTAL_VELOCITY;
    result.y += 2.0 * HORIZONTAL_VELOCITY * random(float(index * 518.23)) - HORIZONTAL_VELOCITY;
    return result;
}

vec4 updatePosition(int index, vec4 previousPosition, vec4 previousVelocity) {
    vec4 newPosition = previousPosition + previousVelocity;
    if (previousPosition.y < -BOX_RADIUS) {
        newPosition.y = BOX_RADIUS;
    }

    // Limits the position to within the box.
    if (newPosition.x < -BOX_RADIUS) {
        newPosition.x += 2.0 * BOX_RADIUS;
    }
    if (newPosition.x > BOX_RADIUS) {
        newPosition.x -= 2.0 * BOX_RADIUS;
    }
    if (newPosition.z < -BOX_RADIUS) {
        newPosition.z += 2.0 * BOX_RADIUS;
    }
    if (newPosition.z > BOX_RADIUS) {
        newPosition.z -= 2.0 * BOX_RADIUS;
    }

    return newPosition;
}

const float g = 0.002;
vec4 updateVelocity(int index, vec4 previousPosition, vec4 previousVelocity) {
    vec4 newVelocity = previousVelocity - vec4(0.0, g, 0.0, 0.0);
    if (previousPosition.y < -BOX_RADIUS) {
        newVelocity = initVelocity(index);
    }
    return newVelocity;
}

void main(void)
{
    int index = int(uv.x * numParticles);
    vec4 previousPosition = texture(previousPositions, uv);
    vec4 previousVelocity = texture(previousVelocities, uv);
    if (reset) {
        position = initPosition(index);
        velocity = initVelocity(index);
    } else {
        position = updatePosition(index, previousPosition, previousVelocity);
        velocity = updateVelocity(index, previousPosition, previousVelocity);
    }
}
