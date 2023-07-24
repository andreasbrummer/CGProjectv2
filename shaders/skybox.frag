#version 450
layout(set = 0, binding = 0) uniform GlobalUniformBufferObject {
    vec3 DlightDir;        // direction of the direct light
    vec3 DlightColor;    // color of the direct light
    vec3 AmbLightColor;    // ambient light
    vec3 eyePos;        // position of the viewer

    vec3 PLightPos;
    vec4 PLightColor;
    vec3 PLightPos2;
    vec4 PLightColor2;

    vec3 SLightPos;
    vec3 SLightDir;
    vec4 SLightColor;

    vec3 PLightPosLantern1;
    vec4 PLightColorLantern1;
    vec3 PLightPosLantern2;
    vec4 PLightColorLantern2;
} gubo;

layout(set = 1, binding = 1) uniform samplerCube skybox;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(skybox, fragPos);
}
