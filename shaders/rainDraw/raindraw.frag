#version 400 core

in vec2 uv;
in vec4 basePosition;
out vec4 fragColor;
const float BOX_RADIUS = 12.0;
const float FADE_RADIUS = 0.5;

void main(void)
{
    vec2 aspect = vec2(0.2, 1.0);
    vec2 center = vec2(0.25);
    if (length((center - uv) / aspect) > 0.25) {
        discard;
    }

    // Makes the rain fade in and out.
    float multiplier = 1.0;
    multiplier *= smoothstep(BOX_RADIUS, BOX_RADIUS - FADE_RADIUS, basePosition.y);
    multiplier *= smoothstep(BOX_RADIUS, BOX_RADIUS - FADE_RADIUS, basePosition.x);
    multiplier *= smoothstep(BOX_RADIUS, BOX_RADIUS - FADE_RADIUS, basePosition.z);
    multiplier *= smoothstep(-BOX_RADIUS, FADE_RADIUS - BOX_RADIUS, basePosition.y);
    multiplier *= smoothstep(-BOX_RADIUS, FADE_RADIUS - BOX_RADIUS, basePosition.x);
    multiplier *= smoothstep(-BOX_RADIUS, FADE_RADIUS - BOX_RADIUS, basePosition.z);
    multiplier *= (0.25 - length((center - uv) / aspect)) / 0.25;
    fragColor = vec4(1.0, 1.0, 1.0, 0.35 * multiplier);
}
