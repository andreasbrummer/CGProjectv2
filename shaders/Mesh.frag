#version 450#extension GL_ARB_separate_shader_objects : enablelayout(location = 0) in vec3 fragPos;layout(location = 1) in vec3 fragNorm;layout(location = 2) in vec2 fragUV;layout(location = 0) out vec4 outColor;layout(set = 0, binding = 0) uniform GlobalUniformBufferObject {	vec3 DlightDir;        // direction of the direct light	vec3 DlightColor;    // color of the direct light	vec3 AmbLightColor;    // ambient light	vec3 eyePos;        // position of the viewer	vec3 PLightPos;	vec4 PLightColor;} gubo;layout(set = 1, binding = 0) uniform UniformBufferObject {	float amb;	float gamma;	vec3 sColor;	mat4 mvpMat;	mat4 mMat;	mat4 nMat;} ubo;layout(set = 1, binding = 1) uniform sampler2D tex;const float beta = 0.5f;const float g = 2.5f;void main() {	vec3 N = normalize(fragNorm);                // surface normal	vec3 V = normalize(gubo.eyePos - fragPos);    // viewer direction	vec3 L = normalize(gubo.DlightDir);            // light direction	//direct light	vec3 D = normalize(gubo.PLightPos - fragPos);	//vec4 C = normalize(gubo.PLightColor);	vec3 lightColor = gubo.PLightColor.rgb * pow(g / length(gubo.PLightPos - fragPos), beta);	vec3 Diffuse = texture(tex, fragUV).rgb * 0.99f * clamp(dot(N, D),0.0,1.0);	vec3 Specular = vec3(pow(clamp(dot(N, normalize(D + V)),0.0,1.0), 160.0f));	vec3 Ambient = texture(tex, fragUV).rgb * 0.01f;	// Calculate the directional light contribution	vec3 directLight = gubo.DlightColor * max(dot(N, L), 0.0);	// Calculate the point light contribution	vec3 PL = normalize(gubo.PLightPos - fragPos);	vec3 pointLightColor = gubo.PLightColor.rgb * pow(g / length(gubo.PLightPos - fragPos), beta);	vec3 pointLight = pointLightColor * max(dot(N, PL), 0.0);		vec3 albedo = texture(tex, fragUV).rgb;        // main color	vec3 MD = albedo;	vec3 MS = ubo.sColor;	vec3 MA = albedo * ubo.amb;	vec3 LA = gubo.AmbLightColor;	// Write the shader here	// Combine the lighting contributions	vec3 finalColor = MD * (directLight + pointLight) + LA * MA;		outColor = vec4(clamp(finalColor, 0.0, 1.0), 1.0);    // output color	/*	outColor = vec4(	clamp(MD * clamp(dot(L,N),0.0f,1.0f) +	MS * pow(clamp(dot(N, normalize(L + V)), 0.0f, 1.0f), ubo.gamma) +	LA * MA,	0.0f, 1.0f), 1.0f);    // output color	*/}