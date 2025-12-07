#include <../EngineShader/PostProcessHeader.hlsli>
#include <../EngineShader/Shared.hlsli>

RWTexture2D<unorm float4> T_Temp[2] : register(u2);
cbuffer Config : register(b0)
{
	
	float edgeWidth;
	float depthEdgeThreshold;
	float normalEdgeThreshold;
	float pad2;
	
	float4 edgeColor;
};

[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint width, height;
	T_Depth.GetDimensions(width, height);

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
	
	float result = 0.0;
	float subResult = 1000.0;
	for (int i = 0; i < 9; i++)
	{
		int2 sampleCoord = coord + offsets[i];
		float sampleA = T_GBuffer[0].Load(uint3(sampleCoord, 0)).a;
		subResult *= sampleA;
		result += sampleA * mask9[i];
		
	}
	
	if (result * saturate(abs(subResult)) > depthEdgeThreshold)
	{
		//¿Ü°û¼±
		T_Temp[0][DTid.xy] = float4(edgeColor.rgb, 0);
	}
	else
	{
		T_Temp[0][DTid.xy] = 1;
	}
	//float depthEdge = 0.0;
	//float3 normalEdge = 0.0;
	//for (int i = 0; i < 9; i++)
	//{
	//	int2 sampleCoord = coord + offsets[i];
	//	sampleCoord = clamp(sampleCoord, int2(0, 0), int2(width - 1, height - 1));
	//	float4 worldPosition;
	//	float2 uv = float2(sampleCoord) / float2(width, height);
	//	uv.y = 1.0f - uv.y;
	//	float2 ndc = uv * 2.0 - 1.0;
		
	//	worldPosition.xy = ndc;
	//	worldPosition .z = T_Depth.Load(uint3(sampleCoord, 0)).r;
	//	worldPosition.w = 1.0;
	//	worldPosition = mul(worldPosition, IPM);
	//	worldPosition = worldPosition / worldPosition.w;
	//	depthEdge += worldPosition.z * mask9[i];
		
	//	normalEdge += T_Normal.Load(uint3(sampleCoord, 0)).xyz * mask9[i];
	//}
	
	//if (depthEdge > depthEdgeThreshold && length(normalEdge) > normalEdgeThreshold)
	//{
	//	//¿Ü°û¼±
	//	T_Temp[0][DTid.xy] = 0;
	//}
	//else
	//{
	//	T_Temp[0][DTid.xy] = 1;
	//}
	
	
}
