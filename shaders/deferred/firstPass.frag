#version 400 core
const float M_PI= 3.1415926535897932384626433832795;
const float EPSILON = .000005;
in vec2 uv;
in mat4x4 toScreenSpace;
in vec4 sphereVertexNormal;
out vec4 fragColor;

//uniform sampler2D lightTexture;
//uniform sampler2D lightPositions;
uniform sampler2D colorTexture;
uniform sampler2D positionTexture;
uniform sampler2D normalTexture;
uniform float screenWidth;
uniform float screenHeight;

uniform vec4 eyePositionWorldspace;

uniform vec3 lightPos;    // For point lights
uniform vec3 lightIntensity;
uniform float lightBrightness;

void main(){
    // retrieve data from buffer
    float screenX = gl_FragCoord.x / screenWidth;
    float screenY = gl_FragCoord.y / screenHeight;
    vec2 screenspace_uv = vec2(screenX, screenY);
    vec3 color = texture(colorTexture, screenspace_uv).rgb;
    vec3 normal = texture(normalTexture, screenspace_uv).rgb;
    vec3 positionWorld = texture(positionTexture, screenspace_uv).rgb;
    vec3 n = normalize(normal.xyz);

    vec3 L = lightPos - positionWorld.xyz;

    vec3 R = normalize(reflect(-normalize(L), normalize(normal.xyz)));
    vec3 V = normalize(eyePositionWorldspace.xyz - positionWorld);

    // hard coded
    vec3 objectSpecular = vec3(0.1, 0.1, 0.1);
    float specularExponent = 2.0;

    vec3 specularIntensity = clamp(objectSpecular*pow(clamp(dot(R, V), 0.0, 1.0), specularExponent), 0.0, 1.0);

    vec3 intensity = (color * clamp(dot(n, L), 0.0, 1.0)) + specularIntensity;
    vec4 intensity2 = clamp(vec4(intensity, 1.0), 0.0, 1.0);

    vec4 lookVector = normalize(vec4(eyePositionWorldspace.xyz, 0.0));
    vec4 angleVector = normalize(sphereVertexNormal);
    float dotProd = dot(lookVector, angleVector);
    float angle = acos(dotProd);
    float frac = 1.0 - 2*(angle / (0.5 * M_PI));
    float attenuatedBrightness = lightBrightness * frac;

    vec4 attenuatedLightIntensity = clamp(vec4(lightIntensity, attenuatedBrightness), 0.0, 1.0);

    fragColor = attenuatedLightIntensity * intensity2;
}
