#version 330 core

in vec3 color;
in vec3 out_normal;
in vec2 texc;
in vec3 inPosition;
out vec3 fragColor;
out vec3 position;
out vec3 normal;

uniform float iTime;
uniform sampler2D tex;
uniform vec2 repeatUV;
uniform bool enabled;

// These constants could be passed as uniforms.
#define THICKNESS 0.05
#define RADIUS 0.2
#define DURATION 0.5
#define DISTORTION 2.0
#define SPECULAR_INTENSITY 0.2
#define NUM_ROWS 5.0

float rand(vec2 n) {
    return fract(sin(dot(n.xy, vec2(12.43, 72.29))) * 419.54);
}

float getRadiusForTime(float t) {
    float tmod = mod(t, DURATION);
    return RADIUS * pow(tmod / DURATION, 0.65);
}

vec3 layer(vec2 in_uv, float in_t) {
    // Gets local UV coordinates (within a box) in the range [-0.5, 0.5].
    vec2 uvLocal = fract(in_uv) - 0.5;
    vec2 id = floor(in_uv);

    // Finds some random numbers.
    float rt = (rand(id * 3.45 + 7.89) - 0.5) * 0.5;
    float t = in_t + rt;
    float iteration = floor(t / DURATION);
    float rx = (rand(id + iteration) - 0.5) * 0.5;
    float ry = (rand((id + iteration) * 1.23 + 4.56) - 0.5) * 0.5;

    // Calculates distances related to the ring.
    vec2 xy = vec2(rx, ry);
    vec2 fromXY = uvLocal - xy;
    vec2 nFromXY = normalize(fromXY);
    vec2 border = nFromXY * getRadiusForTime(t);
    vec2 fromBorder = fromXY - border;
    float dFromBorder = length(fromBorder);

    // Calculates the ring's mask.
    float ring = smoothstep(-THICKNESS, 0.0, dFromBorder);
    ring *= smoothstep(THICKNESS, 0.0, dFromBorder);
    ring *= (RADIUS + THICKNESS - length(fromXY)) / (RADIUS + THICKNESS);

    // Calculates "specular" highlights.
    vec2 lightDirection = vec2(1.0, 1.0);
    float light = ring * abs(dot(normalize(lightDirection), normalize(fromBorder)));

    return vec3(fromBorder * DISTORTION * ring, clamp(light, 0.0, 1.0));
}

void main(){
    vec2 original_uv = texc * repeatUV;
    vec2 uv = original_uv * NUM_ROWS;

    normal = out_normal * 0.5 + 0.5;
    position = inPosition;

    // Shows just the texture if rain is disabled.
    if (!enabled) {
        fragColor = color * texture(tex, original_uv).rgb;
        return;
    }

    // Generates the layers.
    vec3 layer1 = layer(uv, iTime);
    vec3 layer2 = layer(uv + vec2(0.723, 0.456), iTime + 3.93);
    vec3 layer3 = layer(uv + vec2(0.123, 0.678), iTime + 9.12);
    vec3 total = layer1 + layer2 + layer3;

    // Generates the pixel color using the layers.
    bool isFacingUp = (out_normal.y > 0.5);

    // Samples the texture.
    // This is distorted for upward-facing normals.
    vec3 texColor = isFacingUp ? texture(tex, original_uv + total.xy).rgb : texture(tex, original_uv).rgb;
    texColor = clamp(texColor, vec3(0.0), vec3(1.0));
    fragColor = color * texColor;

    // The shader part only runs for things facing (roughly) up.
    if (isFacingUp) {
        fragColor += total.z * SPECULAR_INTENSITY;
    }
}
