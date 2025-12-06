#include "Shared.hlsli"
#include "GBufferMaterial.hlsli"


struct CustomBuffer
{{
{0}
// 인풋 따로 받게끔 ex) 색, 가중치 같은거
}};

cbuffer CustomBuffer : register(b5)
{{
	CustomBuffer customData;
}};

{1}
{2}


GBufferMaterial GetCustomGBufferMaterial(PS_INPUT input)
{{
    GBufferMaterial material = GetDefaultGBufferMaterial(input);

{3}
{4}
    return material;
}}

#define GetGBufferMaterial GetCustomGBufferMaterial
#include "../EngineShader/BasePassPS.hlsl"