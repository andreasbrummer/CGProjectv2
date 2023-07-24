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

    vec3 PLightPosPool;
    vec4 PLightColorPool;

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
layout(set = 1, binding = 2) uniform sampler2D metallicMap;
layout(set = 1, binding = 3) uniform sampler2D RoughnessMap;

#define PI 3.14159265359
//point light parameters
const float beta = 0.4f;
const float g = 3.0f;

//point light pool parameters
//beta alto = poco diffuso. g alto = molto forte
const float betaPool = 1.5f;
const float gPool = 0.3f;

//spot light parameters
const float betaS = 0.5f;
const float gS = 3.0;
//cosin --> inner diameter
//cosout --> decay
const float cosout = 0.65;
const float cosin  = 0.75;


float lowerscore_g_GGX_version(vec3 n, vec3 general_vector, float rho) {

    return 2 / (1 + pow( 1 + (pow(rho,2 * ( (1 - pow(dot(n,general_vector),2)) / pow(dot(n,general_vector),2) ))) ,(1/2)));
}

vec3 BRDF(vec3 V, vec3 N, vec3 L, vec3 Md, float F0, float metallic, float roughness) {
    //vec3 V  - direction of the viewer
    //vec3 N  - normal vector to the surface
    //vec3 L  - light vector (from the light model)
    //vec3 Md - main color of the surface
    //float F0 - Base color for the Fresnel term
    //float metallic - parameter that mixes the diffuse with the specular term.
    //                 in particular, parmeter K seen in the slides is:
    float K = 1.0f - metallic;
    //float roughness - Material roughness (parmeter rho in the slides).
    //specular color Ms is not passed, and implicitely considered white:
    vec3 Ms = vec3(1.0f);

    vec3 hlx = normalize(L + V);

    float D = (pow(roughness,2)) / (3.14 * pow(pow(clamp(dot(N,hlx),0,1),2) * (pow(roughness,2) - 1) + 1.0f ,2)); //GGX model

    float F = F0 + ( (1-F0) * pow( (1 - clamp( dot(V,hlx),0,1 ) ),5) );

    float G = lowerscore_g_GGX_version(N,V,roughness) * lowerscore_g_GGX_version(N,L,roughness);

    vec3 fspecular = Ms * ((D * F * G) / (4 * clamp(dot(V,N),0,1)));

    vec3 fdiffuse = Md * clamp(dot(L,N),0,1);

    vec3 ftot = K * fdiffuse + (1-K) * fspecular;

    return ftot;
}

void main() {
    vec3 N = normalize(fragNorm);
    //vec3 Tan = normalize(fragTan.xyz - Norm * dot(fragTan.xyz, Norm));
    //vec3 Bitan = cross(Norm, Tan) * fragTan.w;
    //mat3 tbn = mat3(Tan, Bitan, Norm);
    //vec4 nMap = texture(normMap, fragUV);
    //vec3 N = normalize(tbn * (nMap.rgb * 2.0 - 1.0));

    vec3 albedo = texture(tex, fragUV).rgb;

    vec4 Metallic = texture(metallicMap, fragUV);
    vec4 Roughness = texture(RoughnessMap, fragUV);
    float roughness = Roughness.r;
    float metallic = Metallic.r;


    //BRDF

    vec3 L = gubo.DlightDir;
    vec3 lightColor = gubo.DlightColor.rgb;

    vec3 V = normalize(gubo.eyePos - fragPos);

    vec3 DiffSpec = BRDF(V, N, L, albedo, 0.3f, metallic, roughness);
    vec3 Ambient = albedo * 0.05f;

    outColor = vec4(clamp(0.95 * DiffSpec * lightColor.rgb + Ambient,0.0,1.0), 1.0f);
}