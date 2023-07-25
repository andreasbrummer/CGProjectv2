#version 450
layout(set = 0, binding = 1) uniform samplerCube skybox;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(skybox, fragPos);
}
