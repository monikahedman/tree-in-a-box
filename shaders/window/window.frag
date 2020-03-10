#version 400 core
#define sms(a, b, t) smoothstep(a, b, t)

// These uniforms are designed to match ShaderToy.
uniform vec3 iResolution;
uniform float iTime;

// This comes from the vertex shader.
in vec2 uv;

// This is the FBO's image.
uniform sampler2D tex;

// This is the output.
out vec4 fragColor;

// If this is true, no blurring is done.
uniform bool disableBlur;

// If this is true, the additional blur color isn't added.
uniform bool disableBlurColor;

// If this is true, the shader just shows the original FBO.
uniform bool disableShader;

// Some of these constants could become uniforms.
// However, the number of uniforms is limited.

// the number of rows of drops
// More rows will create more drops and smaller drops.
#define NUM_ROWS 3.0

// a drop's radius in local UV coordinates
#define DROP_RADIUS 0.1

// the radius used for smoothstep
#define DROP_STEP_RADIUS 0.01

// a drop trail's radius in local UV coordinates
#define TRAIL_RADIUS 0.08

// the radius used for smoothstep
#define TRAIL_STEP_RADIUS 0.01

// the number of trail drops per trail
#define TRAIL_COUNT 6.0

// the falling speed of the drops
#define FALL_SPEED 0.25

// used to smooth out fog horizontally
#define FOG_SMOOTHING 0.8

// the aspect ratio of the boxes the drops move within
#define ASPECT_RATIO 0.3

// how many seconds before time loops to avoid floating-point inaccuracy
#define LOOP_TIME 7200.0

// the distortion in the drops (positive for reflection, negative for refraction)
#define DISTORTION -1.5

// the blur applied outside the drops (in UV)
#define BLURRINESS 0.02

// the number of nearby samples taken for the blur
// Larger numbers (e.g. 512) cause noticeable slowdowns.
#define BLUR_SAMPLES 16.0

// the color added to blurred areas
#define BLUR_ADDED_COLOR vec4(1.0, 1.0, 1.0, 0.05)

// Calculates a drop's x coordinate given its y coordinate.
float dropX(float y) {
    // Since y is between -0.5 and 0.5, it needs to be multipied by 2 pi.
    float t = y * 6.28;
    return 0.4 * sin(t + sin(t));
}

// Calculates a drop's y coordinate given the time.
float dropY(float t) {
    return -0.45 * sin(t + sin(t + sin(t) * 0.5));
}

// This is a pseudorandom number generator.
// It's OK, but it's not great.
float rand(vec2 n) {
    return fract(sin(dot(n.xy, vec2(12.43, 72.29))) * 419.54);
}

// Calculates a layer of drops.
// The output has two parts:
// xy: the offset
// z: the fog level
// The inputs are UV and time.
vec3 layer(vec2 in_uv, float t) {
    // Creates a vector based on the aspect ratio.
    // This makes the aspect ratio easier to work with.
    // Note: Local UVs are only stretched in the y direction.
    const vec2 aspect = vec2(1.0, 1.0 / ASPECT_RATIO);

    // There are three types of UV:
    // 1. uvStatic: This is a UV such that each box occupies one unit (i.e. a box is 1 wide and 1 high).
    // 2. uvMoving: This is like uvStatic, but with a yOffset to simulate movement.
    // 3. uvLocal: This is the fractional component of uvMoving.
    vec2 uvStatic = in_uv;
    uvStatic.x *= iResolution.x / iResolution.y;
    uvStatic *= NUM_ROWS;
    uvStatic.x /= ASPECT_RATIO;

    // Makes the UV change with time to account for falling.
    float yOffset = t * FALL_SPEED;
    vec2 uvMoving = uvStatic;
    uvMoving.y += yOffset;

    // Adds randomness to the time.
    vec2 id = floor(uvMoving);
    float r = rand(id);
    float tRandom = t + r * 6.28;

    // Finds the UV for each drop region.
    // Note: The UV is actually -0.5 to 0.5 because that's easier to work with.
    vec2 uvLocal = fract(uvMoving);
    uvLocal -= 0.5;

    // Calculates the drop's x and y.
    float y = dropY(tRandom);
    float xOffset = (r - 0.5) * 0.4; // [-0.4, 0.4]
    float x = (0.4 - abs(xOffset)) * dropX(uvStatic.y) + xOffset;

    // Calculates the offset from the drop's position.
    vec2 dropOffset = (uvLocal - vec2(x, y)) * aspect;
    vec2 trailOffset = (uvLocal - vec2(x, yOffset)) * aspect;

    // This duplicates the trail.
    const float adjustedTrailCount = TRAIL_COUNT / aspect.y;
    trailOffset.y = (fract(trailOffset.y * adjustedTrailCount) - 0.5) / adjustedTrailCount;

    // Generates the drop and trail.
    float drop = sms(DROP_RADIUS + DROP_STEP_RADIUS, DROP_RADIUS - DROP_STEP_RADIUS, length(dropOffset));
    float trail = sms(TRAIL_RADIUS + TRAIL_STEP_RADIUS, TRAIL_RADIUS - TRAIL_STEP_RADIUS, length(trailOffset));
    float fog = sms(0.5, y, uvLocal.y) * sms(-TRAIL_STEP_RADIUS, TRAIL_STEP_RADIUS, dropOffset.y);
    trail *= fog;
    fog *= sms(DROP_RADIUS, DROP_RADIUS * FOG_SMOOTHING, abs(dropOffset.x));

    // Makes sure the drop itself doesn't have fog.
    fog += sms(DROP_RADIUS + DROP_STEP_RADIUS, DROP_RADIUS - DROP_STEP_RADIUS, length(dropOffset));

    // Calculates the offset based on the drop and trail.
    vec2 offset = vec2(drop * dropOffset + trail * trailOffset);
    return vec3(offset, fog);
}

void main() {
    if (disableShader) {
        fragColor = texture(tex, uv);
        return;
    }

    float t = mod(iTime, LOOP_TIME);

    // Runs several layers of
    vec3 layer1 = layer(uv, t);
    vec3 layer2 = layer(uv * 1.22 + vec2(1.23, 6.78), t + 4.52);
    vec3 layer3 = layer(uv * 0.66 + vec2(8.90, 1.23), t + 2.65);

    // Calculates the overall offset (used for refraction) and fog (used for blurring).
    vec2 offset = layer1.xy + layer2.xy + layer3.xy;
    float fog = clamp(layer1.z + layer2.z + layer3.z, 0.0, 1.0);

    // Samples from the texture to get the original.
    vec4 original = texture(tex, uv + offset * DISTORTION);

    // Samples from the texture to get a blurred version.
    vec4 blurred = vec4(0.0);
    if (disableBlur) {
        blurred = texture(tex, uv);
    } else {
        for (float i = 0.0; i < BLUR_SAMPLES; i += 1.0) {
            float theta = i / BLUR_SAMPLES * 6.28;
            vec2 sampleOffset = vec2(sin(theta), cos(theta)) * BLURRINESS * rand(vec2(i) + uv);
            blurred += texture(tex, uv + sampleOffset);
        }
        blurred /= BLUR_SAMPLES;
    }

    // Interpolates the original and blurred versions.
    // Also adds additional color to the blurred version.
    if (!disableBlurColor) {
        blurred += vec4(BLUR_ADDED_COLOR.xyz * BLUR_ADDED_COLOR.w, 0.0);
    }
    vec4 color = (1.0 - fog) * blurred + original * fog;
    fragColor = vec4(color);
}
