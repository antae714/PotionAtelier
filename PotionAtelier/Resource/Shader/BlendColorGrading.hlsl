#include <../EngineShader/PostProcessHeader.hlsli>
#include <../EngineShader/Shared.hlsli>


Texture3D T_LUT : register(t10);
Texture3D T_LUT2 : register(t11);

cbuffer cbColorGrading : register(b0)
{
	float blendWeight;
};

[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	float3 lutSize;
	T_LUT.GetDimensions(lutSize.x, lutSize.y, lutSize.z);
	lutSize = 16.0f;
	float3 originColor = T_Input[DTid.xy].rgb;
	float3 scale = (lutSize - 1.0) / lutSize;
	float3 offset = 1.0 / (2.0 * lutSize);
	
	float3 sampleCoord = originColor * scale + offset;

	float4 color1 = T_LUT.SampleLevel(ClampSampler, sampleCoord, 0);
	float4 color2 = T_LUT2.SampleLevel(ClampSampler, sampleCoord, 0);
	
	
	T_Output[DTid.xy] = float4(lerp(color1.rgb, color2.rgb, blendWeight), 1);
}