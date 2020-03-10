#version 400 core
const float M_PI= 3.1415926535897932384626433832795;

in vec2 uv;
in vec4 vertexNormal;
out vec3 fragColor;

uniform sampler2D colorTexture;
uniform sampler2D positionTexture;
uniform sampler2D normalTexture;
uniform vec4 eyePositionWorldspace;
uniform vec3 lightPos;
uniform vec3 lightIntensity;
//uniform float lightBrightness;

void main(){
//     retrieve data from buffer
    vec2 screenspace_uv = uv; // gl_FragCoord.xy;
    vec3 color = texture(colorTexture, screenspace_uv).rgb;
    vec3 normal = texture(normalTexture, screenspace_uv).rgb;
    vec3 positionWorld = texture(positionTexture, screenspace_uv).rgb;

    vec3 n = normalize(normal.xyz);

    vec3 L = normalize(lightPos - positionWorld.xyz);

    vec3 R = normalize(reflect(-L, normalize(normal.xyz)));
    vec3 V = normalize(eyePositionWorldspace.xyz - positionWorld);

    // hard coded
    vec3 objectSpecular = vec3(0.2, 0.2, 0.2);
    float specularExponent = 2.0;
//    vec3 specularIntensity = clamp(objectSpecular*pow(clamp(dot(R, V), 0.0, 1.0), specularExponent), 0.0, 1.0);

//     intensity of the actual scene
    vec3 intensity = (color * clamp(dot(n, L), 0.0, 1.0));// + specularIntensity;

    // goes from origin to eye position in world space
    vec4 lookVector = normalize(vec4(eyePositionWorldspace.xyz, 0.0));
    vec4 angleVector = normalize(vertexNormal);
    float dotProd = dot(lookVector, angleVector);
    float angle = acos(dotProd);
    float frac = 1.0 - 2*(angle / (0.5 * M_PI));
    float lightBrightness = 1.0;
    float attenuatedBrightness = lightBrightness * frac;

//    fragColor = vec4(lightIntensity*intensity, 1.0);
//    fragColor = vec4(positionWorld, 1.0f);
//    fragColor = vec4(1.0, 0.0, 0.0, 1.0);

        fragColor = positionWorld + normal + color;
        fragColor -= 999.9;
        fragColor = clamp(fragColor, 0.0, 1.0);
}
