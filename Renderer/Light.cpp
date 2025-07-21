#include "Light.h"
#include <d3d11.h>
#include <format>
#include <ranges>


DirectionLightBuffer::DirectionLightBuffer(int capacity) :
	capacity(capacity),
	_size(0)
{
	shadowMap_DSV.reserve(capacity);
}

void DirectionLightBuffer::CreateBuffer(UINT newCapacity)
{
	HRESULT result;

	capacity = newCapacity;

	D3D11_TEXTURE2D_DESC shadowMapDesc
	{
		.Width = textureSize,
		.Height = textureSize,
		.MipLevels = 1,
		.ArraySize = capacity,
		.Format = DXGI_FORMAT_R32_TYPELESS,
		.SampleDesc = { 1, 0 },
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE
	};

	ComPtr<ID3D11Texture2D> shadowMapArray;
	result = RendererUtility::GetDevice()->CreateTexture2D(&shadowMapDesc, NULL, &shadowMapArray);
	Check(result);

	D3D11_SHADER_RESOURCE_VIEW_DESC shadowMapSRVDesc
	{
		.Format = DXGI_FORMAT_R32_FLOAT,
		.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY,
		.Texture2DArray = { 0, 1 , 0, capacity }
	};

	shadowMap.LoadTexture(shadowMapArray.Get(), ETextureUsage::SRV, &shadowMapSRVDesc);
}

void DirectionLightBuffer::CreateView(uint32_t newSize)
{
	_size = newSize;
	shadowMap_DSV.resize(_size);

	HRESULT result;
	for (UINT i = 0; i < shadowMap_DSV.size(); i++)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC shadowMapDSVDesc
		{
			.Format = DXGI_FORMAT_D32_FLOAT,
			.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY,
			.Texture2DArray = { 0 , i, 1 }
		};
		result = RendererUtility::GetDevice()->CreateDepthStencilView(shadowMap, &shadowMapDSVDesc, &shadowMap_DSV[i]);
		Check(result);
	}


	D3D11_SHADER_RESOURCE_VIEW_DESC shadowMapSRVDesc
	{
		.Format = DXGI_FORMAT_R32_FLOAT,
		.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY,
		.Texture2DArray = { 0, 1 , 0, _size }
	};
	ID3D11Device* device = RendererUtility::GetDevice();
	ComPtr<ID3D11ShaderResourceView> shaderResourceView{};
	result = device->CreateShaderResourceView((ID3D11Texture2D*)shadowMap, &shadowMapSRVDesc, &shaderResourceView);
	Check(result);

	shadowMap.LoadTexture(shaderResourceView.Get());

	lightCameras.resize(directLights.size());
}

void DirectionLightBuffer::UpdateBuffer()
{
	directLightBuffer.Set(directLights, { .isSRV = true });
}


void DirectionLightBuffer::SetTextureSize(uint32_t size)
{
	textureSize = size;
	CreateBuffer(capacity);
	CreateView(_size);
}

void DirectionLightBuffer::PushDirectLight(std::string_view name, const DirectionLightData& light)
{
	directLights.emplace_back(light);
	directLightNames.emplace(name.data(), directLights.size() - 1);

	if (directLights.capacity() > capacity)
	{
		CreateBuffer(directLights.capacity());
	}

	CreateView(++_size);
}

void DirectionLightBuffer::PopDirectLight(std::string_view name)
{
	auto popIndex = directLightNames[name.data()];
	auto lastIndex = directLights.size() - 1;
	std::swap(directLights[popIndex], directLights.back());
	directLights.pop_back();
	directLightNames.erase(name.data());

	auto iter = std::find_if(directLightNames.begin(), directLightNames.end(), [lastIndex](const auto& pair) { return pair.second == lastIndex; });
	if (iter != directLightNames.end())
	{
		iter->second = popIndex;
	}

	CreateView(--_size);
}

DirectionLightData& DirectionLightBuffer::GetDirectLight(std::string_view name)
{
	return directLights[directLightNames[name.data()]];
}

DirectionLightData& DirectionLightBuffer::GetDirectLight(size_t index)
{
	return directLights[index];
}

void DirectionLightBuffer::ComputeLightMatrix(const BoundingBox& boundBox, 
											  const Vector3& lightDirection, 
											  _Out_ XMMATRIX& lightViewMatrix, 
											  _Out_ XMMATRIX& lightProjMatrix, 
											  _Out_opt_ BoundingOrientedBox* outProjectBox)
{
	Vector3 light;
	lightDirection.Normalize(light);
	// 1. ī�޶� ����ü�� ������ ��� (���� ��ǥ��� ����)
	XMFLOAT3 frustumCorners[8];
	boundBox.GetCorners(frustumCorners);

	// 2. �� �� ��� ����
	Vector3 lightUp(0.0f, 1.0f, 0.0f); // �⺻ Up ����
	if (abs(light.y) > 0.99f) // ���� ���� �Ʒ��� ���ϴ� ���
	{
		lightUp = Vector3(0.0f, 0.0f, -1.0f); // Up ���͸� �ٲ�
	}
	lightViewMatrix = XMMatrixLookToLH(Vector3::Zero, light, lightUp);

	// 3. �� ��ǥ��� ��ȯ�� ����ü�� ������ ���
	Vector4 lightMin(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
	Vector4 lightMax(-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (int i = 0; i < 8; ++i)
	{
		Vector4 corner = DirectX::XMLoadFloat3(&frustumCorners[i]);
		corner = Vector4::Transform(corner, Matrix(lightViewMatrix)); // �� �� �������� ��ȯ
		lightMin = XMVectorMin(lightMin, corner);
		lightMax = XMVectorMax(lightMax, corner);
		XMStoreFloat3(&frustumCorners[i], corner);
	}

	// 4. �� ��ǥ�迡�� AABB ��� ���� ���� ��� ����
	XMFLOAT3 lightBoxMin, lightBoxMax;
	XMStoreFloat3(&lightBoxMin, lightMin);
	XMStoreFloat3(&lightBoxMax, lightMax);

	lightProjMatrix = DirectX::XMMatrixOrthographicOffCenterLH(
		lightBoxMin.x, lightBoxMax.x, // left, right
		lightBoxMin.y, lightBoxMax.y, // bottom, top
		lightBoxMin.z, lightBoxMax.z + 1  // near, far
	);

	if (outProjectBox)
	{
		BoundingOrientedBox::CreateFromPoints(*outProjectBox,
											  std::size(frustumCorners),
											  std::data(frustumCorners),
											  sizeof(XMFLOAT3));

	}
}

PointLightBuffer::PointLightBuffer(int capacity) :
	capacity(capacity),
	_size(0)
{
}

void PointLightBuffer::PushPointLight(std::string_view name, const PointLightData& light)
{
	pointLights.emplace_back(light);
	pointLightNames.emplace(name.data(), pointLights.size() - 1);
	++_size;
}

void PointLightBuffer::PopPointLight(std::string_view name)
{
	auto popIndex = pointLightNames[name.data()];
	auto lastIndex = pointLights.size() - 1;
	std::swap(pointLights[popIndex], pointLights.back());
	pointLights.pop_back();
	pointLightNames.erase(name.data());
	auto iter = std::find_if(pointLightNames.begin(), pointLightNames.end(), [lastIndex](const auto& pair) { return pair.second == lastIndex; });
	if (iter != pointLightNames.end())
	{
		iter->second = popIndex;
	}
	--_size;
}

PointLightData& PointLightBuffer::GetPointLight(std::string_view name)
{
	return pointLights[pointLightNames[name.data()]];
}

PointLightData& PointLightBuffer::GetPointLight(size_t index)
{
	return pointLights[index];
}

void PointLightBuffer::UpdateBuffer()
{
	pointLightBuffer.Set(pointLights, { .isSRV = true });
}
