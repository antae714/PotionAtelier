// -----------------------------------------------------
// [FXAA Compute Shader Example]
// Created by : ChatGPT o1
// -----------------------------------------------------

#include <Shared.hlsli>
#include <PostProcessHeader.hlsli>

// ----------------------------------------------------------------
// ��ǻƮ ���̴����� ������ ���/�Լ�
// ----------------------------------------------------------------

// (����) ��� ����. cbuffer �Ǵ� ConstantBuffer<T> ����.
cbuffer FXAAConstants : register(b0)
{
	uint gScreenWidth;
	uint gScreenHeight;
	float2 gInvScreenSize; // = (1.0/width, 1.0/height)

	float FXAA_SPAN_MAX; // �ִ� Ž�� �Ÿ�(�ȼ� ����)
	float FXAA_REDUCE_MUL; // ������ (���� ���� ����)
	float FXAA_REDUCE_MIN; // �ּ� �Ӱ谪 (������ �� ������ ������ ����)
};

float3 LoadColor(float2 uv)
{
	return T_Input.SampleLevel(ClampSampler, uv, 0).rgb;
}

// UAV�� ��� ����
void StoreColor(uint px, uint py, float4 color)
{
	T_Output[uint2(px, py)] = color;
}

[numthreads(64, 1, 1)]
void main(uint3 groupThreadID : SV_GroupThreadID,
             uint3 groupID : SV_GroupID,
             uint3 dispatchThreadID : SV_DispatchThreadID)
{
    // dispatchThreadID.xy -> (�ȼ� ��ǥ)
    // dispatchThreadID.z  -> (�Ϲ����� 2D ������������ 0�� ��)

    // -------------------------------
    // 1. ȭ�� ������ ������� üũ
    // -------------------------------
	if (dispatchThreadID.x >= gScreenWidth ||
        dispatchThreadID.y >= gScreenHeight)
	{
		return;
	}

    // �ȼ� ��ǥ
	uint px = dispatchThreadID.x;
	uint py = dispatchThreadID.y;

    // UV ��ǥ (0~1)
	float2 uv = float2((float) px + 0.5f, (float) py + 0.5f) * gInvScreenSize;
    // (0.5f�� �ؼ� ���� ���ø��� ���� offset, ������)

    // -------------------------------
    // 2. �߽� �ȼ� �� �ֺ� �ȼ� ���� ���ø�
    // -------------------------------
	float3 rgbM = LoadColor(uv);

	float2 texel = float2(gInvScreenSize.x, gInvScreenSize.y);

	float3 rgbN = LoadColor(uv + float2(0.0, -texel.y));
	float3 rgbS = LoadColor(uv + float2(0.0, texel.y));
	float3 rgbW = LoadColor(uv + float2(-texel.x, 0.0));
	float3 rgbE = LoadColor(uv + float2(texel.x, 0.0));

    // -------------------------------
    // 3. ���� ���� (��̳ͽ� ���)
    // -------------------------------
	float lumM = dot(rgbM, float3(0.299, 0.587, 0.114));
	float lumN = dot(rgbN, float3(0.299, 0.587, 0.114));
	float lumS = dot(rgbS, float3(0.299, 0.587, 0.114));
	float lumW = dot(rgbW, float3(0.299, 0.587, 0.114));
	float lumE = dot(rgbE, float3(0.299, 0.587, 0.114));

	float lumMax = max(lumM, max(max(lumN, lumS), max(lumW, lumE)));
	float lumMin = min(lumM, min(min(lumN, lumS), min(lumW, lumE)));
	float lumRange = lumMax - lumMin;

    // -------------------------------
    // 4. ������ ����� ���ϸ� �׳� ���� ���
    // -------------------------------
	if (lumRange < FXAA_REDUCE_MIN)
	{
		StoreColor(px, py, float4(rgbM, 1.0));
		return;
	}

    // -------------------------------
    // 5. ���� ����(dir) ���
    // -------------------------------
	float2 dir;
    // ��������(��+�� - 2*�߽�) => x
	dir.x = -((lumN + lumS) - 2.0 * lumM);
    // �������(��+�� - 2*�߽�) => y
	dir.y = ((lumW + lumE) - 2.0 * lumM);

    // ����ȭ
	float maxDir = max(abs(dir.x), abs(dir.y));
	if (maxDir > 1e-5)
	{
		dir /= maxDir;
	}

    // -------------------------------
    // 6. ���� �������� �߰� ���ø�
    // -------------------------------
	float2 offset = dir * FXAA_SPAN_MAX * gInvScreenSize;
    // ��: �ִ� 8�ȼ� �Ÿ�(�⺻�� 8.0)

	float3 rgb1 = LoadColor(uv + offset * (1.0 / 3.0));
	float3 rgb2 = LoadColor(uv + offset * (2.0 / 3.0));
	float3 fxaaResult = 0.5 * (rgb1 + rgb2);

    // -------------------------------
    // 7. ��� ����
    // -------------------------------
	float blendFactor = saturate(lumRange * 2.0); // ���� ����
	float3 finalColor = lerp(rgbM, fxaaResult, blendFactor);

    // -------------------------------
    // 8. ���� ���� ��� (UAV�� ����)
    // -------------------------------
	StoreColor(px, py, float4(finalColor, 1.0));
}
