#ifndef __GBUFFERMATERIAL_HLSL__
#define __GBUFFERMATERIAL_HLSL__

#include "../EngineShader/Shared.hlsli"


struct GBufferMaterial
{
	float3 albedo;
	float metallic;
	
	float3 specular;
	float ambiantOcclusion;
	
	float3 normal;
	float roughness;
	
	float3 emissive;
	float ShadingModelID;
	
	float clipAlpha;
	float alpha;
	
	// ���� ������ΰ���
	// RGBA ���ܰ�������(rgb), ��ü���� �ܰ��� ����(a)
	// RXXX ����� ����
	// XXXX
	// XXXX
	float4 GBuffer[4];
};

GBufferMaterial GetDefaultGBufferMaterial(PS_INPUT input)
{
	GBufferMaterial material = (GBufferMaterial) 0;
	
	material.albedo = 0.0;
	material.alpha = 1.0;
	material.specular = 1.0;
	material.metallic = 0.0;
	material.roughness = 0.0;
	material.ambiantOcclusion = 0.2;
	
	material.normal = float3(0.0, 0.0, 1.0);
	material.emissive = 0;
	
	material.clipAlpha = 0.3333;
	
	return material;
}

#endif // __GBUFFERMATERIAL_HLSL__