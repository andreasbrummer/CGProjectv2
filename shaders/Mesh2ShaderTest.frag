#version 450#extension GL_ARB_separate_shader_objects : enablelayout(location = 0) in vec3 fragPos;layout(location = 1) in vec3 fragNorm;layout(location = 2) in vec2 fragUV;layout(location = 0) out vec4 outColor;layout(set = 0, binding = 0) uniform GlobalUniformBufferObject {	vec3 DlightDir;        // direction of the direct light	vec3 DlightColor;    // color of the direct light	vec3 AmbLightColor;    // ambient light	vec3 eyePos;        // position of the viewer	vec3 PLightPos;	vec4 PLightColor;	vec3 PLightPos2;	vec4 PLightColor2;	vec3 SLightPos;	vec3 SLightDir;	vec4 SLightColor;} gubo;layout(set = 1, binding = 0) uniform UniformBufferObject {	float amb;	float gamma;	vec3 sColor;	mat4 mvpMat;	mat4 mMat;	mat4 nMat;} ubo;layout(set = 1, binding = 1) uniform sampler2D tex;//point light parameters//beta = decay factor. Beta = 0 --> constant, beta = 1 --> inverse linear, beta = 2 --> inverse squared//g alto = molto forteconst float beta = 1.0f;const float g = 1.0f;//spot light parametersconst float betaS = 2.0f;const float gS = 4.0;//cosin --> inner diameter//cosout --> decayconst float cosout = 0.85;const float cosin  = 0.98;//BRDF Lambert (Diffuse) + Phong(specular)vec3 BRDFLP(vec3 V, vec3 N, vec3 L, vec3 Md, vec3 Ms, float gamma) {	//vec3 V  - direction of the viewer	//vec3 N  - normal vector to the surface	//vec3 L  - light vector (from the light model)	//vec3 Md - main color of the surface	//vec3 Ms - specular color of the surface	//float gamma - Exponent for power specular term. Greater gamma --> smaller highlight and objects appears more shiny (more like a mirror)		vec3 diffuse = Md * max(dot(L, N), 0.0);	vec3 R = -reflect(L, N);	vec3 specular = Ms * pow(clamp(dot(V,R), 0.0, 1.0), gamma);	vec3 reflection = diffuse + specular;	return reflection;}//BRDF Lambert (Diffuse) + Blinn(specular)vec3 BRDFLB(vec3 V, vec3 N, vec3 L, vec3 Md, vec3 Ms, float gamma) {	//vec3 V  - direction of the viewer	//vec3 N  - normal vector to the surface	//vec3 L  - light vector (from the light model)	//vec3 Md - main color of the surface	//vec3 Ms - specular color of the surface	//float gamma - Exponent for power specular term		vec3 diffuse = Md * max(dot(L, N), 0.0);	vec3 h = normalize(L + V);	vec3 specular = Ms * pow(clamp(dot(N, h), 0.0, 1.0), gamma);	vec3 final = diffuse + specular;		return final;}void main() {	vec3 norm = normalize(fragNorm);                // surface normal	vec3 eyeDir = normalize(gubo.eyePos - fragPos);    // viewer direction	vec3 lightDir = normalize(gubo.DlightDir);            // light direction	// Calculate the point light contribution	//PL1	//light model	vec3 pLightDir1 = normalize(gubo.PLightPos - fragPos);	vec3 pointLightColor1 = gubo.PLightColor.rgb * pow(g / length(gubo.PLightPos - fragPos), beta);	//vec3 pDiffSpec1 = BRDFLB(eyeDir, norm, pLightDir1, texture(tex, fragUV).rgb, ubo.sColor, ubo.gamma);	vec3 pDiffSpec1 = BRDFLP(eyeDir, norm, pLightDir1, texture(tex, fragUV).rgb, ubo.sColor, ubo.gamma);	vec4 outColorPL1 = vec4(pDiffSpec1 * pointLightColor1.rgb, 1.0f);	//PL2	//light model	vec3 pLightDir2 = normalize(gubo.PLightPos2 - fragPos);	vec3 pointLightColor2 = gubo.PLightColor2.rgb * pow(g / length(gubo.PLightPos2 - fragPos), beta);	//vec3 pDiffSpec2 = BRDFLB(eyeDir, norm, pLightDir2, texture(tex, fragUV).rgb, ubo.sColor, ubo.gamma);	vec3 pDiffSpec2 = BRDFLP(eyeDir, norm, pLightDir2, texture(tex, fragUV).rgb, ubo.sColor, ubo.gamma);	vec4 outColorPL2 = vec4(pDiffSpec2 * pointLightColor2.rgb, 1.0f);	//SpotLight	vec3 SLightDir = normalize(gubo.SLightPos - fragPos);	vec3 SLightColor = gubo.SLightColor.rgb * pow (gS / length(gubo.SLightPos - fragPos), betaS) * clamp((dot(SLightDir, gubo.SLightDir) - cosout) / (cosin - cosout), 0.0, 1.0);	//vec3 SDiffSpec = BRDFLB(eyeDir, norm, SLightDir, texture(tex, fragUV).rgb, ubo.sColor, ubo.gamma);	vec3 SDiffSpec = BRDFLP(eyeDir, norm, SLightDir, texture(tex, fragUV).rgb, ubo.sColor, ubo.gamma);	vec4 SOutColor = vec4(SDiffSpec * SLightColor.rgb, 1.0f);	//Ambient	vec3 albedo = texture(tex, fragUV).rgb;		// main color	vec3 MA = albedo * ubo.amb;	vec3 LA = gubo.AmbLightColor;		vec4 outAmbient = vec4(LA * MA, 1.0f);	outColor = clamp(outAmbient + outColorPL1 + outColorPL2 + SOutColor, 0.0f, 1.0f);   // output color}