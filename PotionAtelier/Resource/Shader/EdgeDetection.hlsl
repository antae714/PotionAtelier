#include <../EngineShader/PostProcessHeader.hlsli>
#include <../EngineShader/Shared.hlsli>



RWTexture2D<unorm float4> T_Temp[2] : register(u2);
cbuffer Config : register(b0)
{
	float edgeWidth;
	float edgeThreshold;
};

[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint width, height;
	T_GBuffer[0].GetDimensions(width, height);

	int2 coord = DTid.xy;
	float mask9[9] =
	{
		-1, -1, -1,
        -1, 8, -1,
        -1, -1, -1
	};

	int2 offsets[9] =
	{
		int2(-1, -1), int2(0, -1), int2(1, -1),
        int2(-1, 0), int2(0, 0), int2(1, 0),
        int2(-1, 1), int2(0, 1), int2(1, 1)
	};
	
	float4 result = 0.0;
	for (int i = 0; i < 9; i++)
	{
		int2 sampleCoord = coord + offsets[i];
		result += T_GBuffer[0].Load(uint3(sampleCoord, 0)) * mask9[i];
		
	}
	
	if (length(result.rgb) > edgeThreshold)
	{
		//¿Ü°û¼±
		float4 finalColor = float4(T_GBuffer[0].Load(uint3(coord, 0)).xyz, 0);
		T_Temp[0][DTid.xy] = length(finalColor.rgb) > 0 ? finalColor : 1;
	}
	else
	{
		T_Temp[0][DTid.xy] = 1;
	}
	
}
