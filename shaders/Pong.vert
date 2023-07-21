#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(set = 0, binding = 0) uniform PongBufferObject {
	vec2 pos;
} pbo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec2 outUV;
void main() {
	gl_Position = vec4(inPosition + pbo.pos, 0.5, 1.0);
	outUV = inUV;
}