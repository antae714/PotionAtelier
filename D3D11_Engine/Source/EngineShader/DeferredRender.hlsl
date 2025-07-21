
#include "Shared.hlsli"
#include "Light.hlsli"
#include "PostProcessHeader.hlsli"



float3 CalculateWorldPosition(float2 uv, float depth, float4x4 invProjectionMatrix, float4x4 cameraWorldMatrix)
{
    // NDC ��ǥ�� ��ȯ (-1 ~ 1 ����)
	float2 ndc = float2(uv.x * 2.0 - 1.0, (1.0 - uv.y) * 2.0 - 1.0);

    // �� ���������� ��ġ (���� ��ȯ�� �ݴ� ����)
	float4 clipSpacePos = float4(ndc, depth, 1.0);
	float4 viewSpacePos = mul(clipSpacePos, invProjectionMatrix);
	viewSpacePos = viewSpacePos / viewSpacePos.w; // �� �������� ��ȯ

    // ���� �������� ��ȯ
	float4 worldSpacePos = mul(viewSpacePos, cameraWorldMatrix);

	return worldSpacePos.xyz;
}




[numthreads(64, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint width, height;
	T_Output.GetDimensions(width, height);
	
	float2 uv = float2(DTid.x / (float) width, DTid.y / (float) height);
	
	//CS������ Load�� ��Ȯ��
	//�ػ󵵰� ��ġ�Ѵٴ� ����Ͽ�
	
	float depth = T_Depth.Load(DTid).r;
	if (depth == 1.0)
	{
		return;
	}
	
	float4 GBuffer0 = T_Albedo.Load(DTid);
	float4 GBuffer1 = T_Specular.Load(DTid);
	float4 GBuffer2 = T_Normal.Load(DTid);
	float4 GBuffer3 = T_Emissive.Load(DTid);
	
	float3 albedo = GammaToLinearSpace(GBuffer0.rgb);
	float3 normal = GBuffer2.rgb;
	float3 emisive = GammaToLinearSpace(GBuffer3.rgb);
	
	
	float3 worldPosition = CalculateWorldPosition(uv, depth, IPM, IVM);
	float3 view = normalize(MainCamPos - worldPosition);
	float3 specular = GBuffer1.rgb;
	float metallic = GBuffer0.a;
	float roughness = GBuffer2.a;
	float ambientOcclusion = GBuffer1.a;
	float ShadingModelID = GBuffer3.a;
	
	float3 F0 = lerp(0.04, albedo, metallic);	
	float3 OutputColor = emisive + IntegrateBXDF(albedo, metallic, roughness, F0, worldPosition, normal, view, specular, ambientOcclusion, ShadingModelID);
	T_Output[DTid.xy] = float4(OutputColor, 1);

	//T_Output[DTid.xy] = ambientOcclusion;
	//T_Output[DTid.xy] = float4(worldPosition, 1);
	//T_Output[DTid.xy] = float4(0, (depth - 0.98) * 50, 0, 1);

}