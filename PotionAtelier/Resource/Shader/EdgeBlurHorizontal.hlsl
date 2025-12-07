#include <../EngineShader/PostProcessHeader.hlsli>
#include <../EngineShader/Shared.hlsli>


RWTexture2D<unorm float4> T_Temp[2] : register(u2);
cbuffer Config : register(b0)
{
	float edgeWidth;
};



[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	float BlurRange = edgeWidth * 0.5f;
	float4 blurResult = 0.0;
	int count = 0;
	
	for (float i = -BlurRange; i <= BlurRange; i++)
	{
		int2 sampleCoord = DTid.xy + int2(i, 0);
		float4 sampleColor = T_Temp[1][sampleCoord];
		if (sampleColor.a == 0)
		{
			blurResult += sampleColor;
			++count;
		}
	}
	if (count > 0)
	{
		blurResult = blurResult / count;
		blurResult.a = 0;
	}
	else
	{
		blurResult.a = 1;
	}
	T_Temp[0][DTid.xy] = blurResult;
}