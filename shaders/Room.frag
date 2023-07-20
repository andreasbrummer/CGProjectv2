#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

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
} gubo;

layout(set = 1, binding = 0) uniform UniformBufferObject {
	float amb;
	float gamma;
	vec3 sColor;
	mat4 mvpMat;
	mat4 mMat;
	mat4 nMat;
} ubo;

layout(set = 1, binding = 1) uniform sampler2D tex;

//point light parameters
//beta = decay factor. Beta = 0 --> constant, beta = 1 --> inverse linear, beta = 2 --> inverse squared
//g alto = molto forte
const float beta = 1.0f;
const float g = 1.5f;

//spot light parameters
const float betaS = 2.0f;
const float gS = 4.0;
//cosin --> inner diameter
//cosout --> decay
const float cosout = 0.85;
const float cosin  = 0.98;

vec3 BRDFCG(vec3 V, vec3 N, vec3 L, vec3 Md, float F0, float metallic, float roughness) {
	//vec3 V  - direction of the viewer
	//vec3 N  - normal vector to the surface
	//vec3 L  - light vector (from the light model)
	//vec3 Md - main color of the surface
	//float F0 - Base color for the Fresnel term
	//float metallic - parameter that mixes the diffuse with the specular term.
	//                 in particular, parmeter K seen in the slides is: float K = 1.0f - metallic;
	//float roughness - Material roughness (parmeter rho in the slides).
	//specular color Ms is not passed, and implicitely considered white: vec3 Ms = vec3(1.0f);

	vec3 h = normalize(L + V);
	float LdotN = max(0.00001f, dot(L, N));
	float VdotN = max(0.00001f, dot(L, N));
	float hdotN = max(0.00001f, dot(L, N));
	float hdotV = max(0.00001f, dot(L, N));

	float k = 1.0f - metallic;
	vec3 Ms = vec3(1.0f);
	
	float gNV = 2.0 / (1.0 + sqrt(1.0 + pow(roughness, 2.0) * (1.0 - pow(VdotN, 2.0)) / (pow(VdotN, 2.0))));
	float gNL = 2.0 / (1.0 + sqrt(1.0 + pow(roughness, 2.0) * (1.0 - pow(LdotN, 2.0)) / (pow(LdotN, 2.0))));
	float G = gNV * gNL;

	float D = pow(roughness, 2.0)/(radians(180) * pow((pow(clamp(hdotN, 0.0, 1.0), 2.0) * (pow(roughness, 2.0) - 1.0) + 1.0), 2.0));

	float F = F0 + (1.0 - F0) * pow((1.0 - clamp(hdotV, 0.0, 1.0)), 5.0);

	vec3 specular = Ms * D * F * G / (4.0 * clamp(VdotN, 0.0, 1.0));
	vec3 diffuse = Md * clamp(LdotN, 0.0, 1.0);

	vec3 reflection = k * diffuse + (1 - k) * specular;

	return reflection;
}

void main() {
	vec3 norm = normalize(fragNorm);                // surface normal
	vec3 eyeDir = normalize(gubo.eyePos - fragPos);    // viewer direction
	vec3 lightDir = normalize(gubo.DlightDir);            // light direction

	// Calculate the directional light contribution
	vec3 directLight = gubo.DlightColor * max(dot(norm, lightDir), 0.0);

	// Calculate the point light contribution

	//PL1
	//light model
	vec3 pLightDir = normalize(gubo.PLightPos - fragPos);
	vec3 pointLightColor = gubo.PLightColor.rgb * pow(g / length(gubo.PLightPos - fragPos), beta);

	vec3 pDiffSpec1 = BRDFCG(eyeDir, norm, pLightDir, texture(tex, fragUV).rgb, 1.0f, 0.5f, 0.67f);
	vec3 Ambient = texture(tex, fragUV).rgb * 0.05f;
	vec4 outColorPL = vec4(clamp((pDiffSpec1) * pointLightColor.rgb + Ambient,0.0,1.0), 1.0f);

	//PL2
	//light model
	vec3 pLightDir2 = normalize(gubo.PLightPos2 - fragPos);
	vec3 pointLightColor2 = gubo.PLightColor2.rgb * pow(g / length(gubo.PLightPos2 - fragPos), beta);

	vec3 pDiffSpec2 = BRDFCG(eyeDir, norm, pLightDir, texture(tex, fragUV).rgb, 1.0f, 0.9f, 0.45f);
	vec4 outColorPL2 = vec4(clamp((pDiffSpec2) * pointLightColor2.rgb + Ambient,0.0,1.0), 1.0f);

	//Ambient
	vec3 albedo = texture(tex, fragUV).rgb;        // main color
	vec3 MD = albedo;
	vec3 MS = ubo.sColor;
	vec3 MA = albedo * ubo.amb;
	vec3 LA = gubo.AmbLightColor;

	// Write the shader here

	// Combine the lighting contributions
	vec3 finalColor = MD * (directLight) + LA * MA;
	
	outColor = vec4(clamp(finalColor, 0.0, 1.0), 1.0) + outColorPL + outColorPL2;   // output color
}