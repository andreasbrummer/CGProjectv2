#version 450

layout(location = 0) in vec3 fragPos;
layout(location = 0) out vec4 outColor;
layout(set = 0, binding = 0) uniform GlobalUniformBufferObject {
	vec3 DlightDir;		// direction of the direct light
	vec3 DlightColor;	// color of the direct light
	vec3 AmbLightColor;	// ambient light
	vec3 eyePos;		// position of the viewer
} gubo;
void main() {
	vec3 N = normalize(fragNorm);				// surface normal