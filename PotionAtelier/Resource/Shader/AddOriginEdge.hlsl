
#include <../EngineShader/PostProcessHeader.hlsli>
#include <../EngineShader/Shared.hlsli>



RWTexture2D<unorm float4> T_Temp[2] : register(u2);



[numthreads(64, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	float4 edgeColor = T_Temp[0][DTid.xy];
	float4 originColor = T_Input[DTid.xy];
	
	
	
	T_Output[DTid.xy] = edgeColor.a == 0 ? edgeColor : originColor;
}