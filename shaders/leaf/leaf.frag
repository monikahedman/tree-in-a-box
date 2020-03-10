#version 330 core

in vec3 color;
in vec2 texc;
in vec3 inPosition;
in vec3 inNormal;
out vec3 fragColor;
out vec3 position;
out vec3 normal;

uniform sampler2D tex;
uniform int useTexture = 0;
uniform vec2 repeatUV;

uniform float leafWidth;

float leafFunction(float distanceAlongSpine) {
    return leafWidth * sin(distanceAlongSpine * 3.141);
}

void main(){
    float distanceAlongSpine = abs(texc.y + texc.x);
    float distanceToSpine = abs(texc.y - texc.x);
    if (distanceToSpine > leafFunction(distanceAlongSpine)) {
        discard;
    }

    vec3 texColor = texture(tex, texc*repeatUV).rgb;
    texColor = clamp(texColor + vec3(1-useTexture), vec3(0), vec3(1));
    fragColor = color * texColor;

    position = inPosition;
    normal = inNormal * 0.5 + 0.5;
}
