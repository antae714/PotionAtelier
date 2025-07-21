#include "Camera.h"
#include <D3DCore/D3D11_GameApp.h>
#include <Math/Mathf.h>
#include <Utility/ImguiHelper.h>

using namespace DirectX;

Camera::Camera()
{
	FOV = 90;
	Near = 1.f;
	Far = 3000.f;
}

Camera::~Camera()
{
	if (this == mainCam)
	{
		mainCam = nullptr;
	}
}

void Camera::InspectorImguiDraw()
{
	ImGui::EditCamera("Camera", this);
}

const Matrix& Camera::GetVM() const
{
	return view;
}

const Matrix& Camera::GetIVM() const
{
	return inversView;
}

const Matrix& Camera::GetPM() const
{
	return projection;
}

const Matrix& Camera::GetIPM() const
{
	return inversProjection;
}

void Camera::UpdateCameraMatrix()
{
	const SIZE& size = D3D11_GameApp::GetClientSize();
	float width = (float)size.cx;
	float height = (float)size.cy;

	view = XMMatrixLookToLH(transform.position, transform.Forward, transform.Up);
	inversView = XMMatrixInverse(nullptr, view);

	if(isPerspective)
		projection = XMMatrixPerspectiveFovLH(FOV * Mathf::Deg2Rad, width / height, Near, Far);
	else
		projection = XMMatrixOrthographicLH(width, height, Near, Far);

	inversProjection = XMMatrixInverse(nullptr, projection);
}

void Camera::SyncCamera(Camera* otherCamera)
{
	if (this == otherCamera)
		return;

	this->transform = otherCamera->transform;
	this->FOV = otherCamera->FOV;
	this->Near = otherCamera->Near;
	this->Far = otherCamera->Far;
	this->isPerspective = otherCamera->isPerspective;
}

Vector3 Camera::ScreenToWorldPoint(float screenX, float screenY, float distance)
{
	SIZE clientSize = D3D11_GameApp::GetClientSize();

	float mouseX = screenX;
	float mouseY = screenY;
	float screenWidth = (float)clientSize.cx;
	float screenHeight = (float)clientSize.cy;

	//���콺 ��ǥ NDC ��ȯ
	float ndcX = (2.0f * mouseX) / screenWidth - 1.0f;
	float ndcY = 1.0f - (2.0f * mouseY) / screenHeight; // y�� ���� ó��
	float ndcZ = (distance - Near) / (Far - Near);
	XMFLOAT3 mouseNDC(ndcX, ndcY, ndcZ);

	//���� ��ǥ�� ��ȯ
	XMVECTOR mouseWorld = XMVector3TransformCoord(XMLoadFloat3(&mouseNDC), inversProjection);
	mouseWorld = XMVector3TransformCoord(mouseWorld, inversView);

	return mouseWorld;
}

Vector2 Camera::WorldToScreenPoint(const Vector3& wp)
{
	// �۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ��۵��ϴ���Ȯ�ξȵ�
	XMVECTOR worldVec = XMVectorSet(wp.x, wp.y, wp.z, 1.0f);
	XMVECTOR clipVec = XMVector4Transform(worldVec, view);
	clipVec = XMVector4Transform(clipVec, projection);

	XMFLOAT4 ndc;
	XMStoreFloat4(&ndc, clipVec);

	// Homogeneous Divide
	if (ndc.w != 0.0f)
	{
		ndc.x /= ndc.w;
		ndc.y /= ndc.w;
		ndc.z /= ndc.w;
	}
	else
	{
		__debugbreak;
		return{};
	}

	SIZE size = WinGameApp::GetClientSize();

	Vector2 screenPos;
	screenPos.x = (ndc.x + 1.0f) * 0.5f * size.cx;
	screenPos.y = (1.0f - ndc.y) * 0.5f * size.cy;

	return screenPos;
}

Ray Camera::ScreenPointToRay(int pointX, int pointY)
{
	SIZE clientSize = D3D11_GameApp::GetClientSize();

	// ȭ�� �������� ���콺 Ŭ�� ��ġ (������ ��ǥ)
	float mouseX = (float)pointX; // ���콺 X ��ǥ
	float mouseY = (float)pointY; // ���콺 Y ��ǥ
	float screenWidth = (float)clientSize.cx; // ȭ�� �ʺ�
	float screenHeight = (float)clientSize.cy; // ȭ�� ����

	// NDC�� ��ȯ (x, y�� -1���� 1 ���� ������ ��ȯ)
	float ndcX = (2.0f * mouseX) / (float)clientSize.cx - 1.0f;
	float ndcY = 1.0f - (2.0f * mouseY) / (float)clientSize.cy;  // y�� ���� ó��
	float nearZ = 0.f;
	float farZ = 1.0f;
	XMFLOAT3 nearPoint(ndcX, ndcY, nearZ);
	XMFLOAT3 farPoint(ndcX, ndcY, farZ);

	//���� ��ǥ�� ��ȯ
	XMVECTOR nearWorld = XMVector3TransformCoord(XMLoadFloat3(&nearPoint), inversProjection);
	nearWorld = XMVector3TransformCoord(nearWorld, inversView);

	XMVECTOR farWorld = XMVector3TransformCoord(XMLoadFloat3(&farPoint), inversProjection);
	farWorld = XMVector3TransformCoord(farWorld, inversView);

	// ������ ���� ���� ���
	FXMVECTOR Origin = nearWorld;
	FXMVECTOR Direction = XMVector3Normalize(XMVectorSubtract(farWorld, nearWorld));

	Ray ray(Origin, Direction);
	return ray;
}

Vector2 Camera::NdcToScreenPoint(const Vector2& ndcPoint)
{
	const SIZE& size = D3D11_GameApp::GetClientSize();
	float screenWidth = (float)size.cx;
	float screenHeight = (float)size.cy;
	float xScreen = (ndcPoint.x + 1.0f) * 0.5f * screenWidth;
	float yScreen = (1.0f - ndcPoint.y) * 0.5f * screenHeight;
	return Vector2(xScreen, yScreen);
}

void Camera::Awake()
{

}

void Camera::FixedUpdate()
{

}

void Camera::Update()
{
	if (mainCam == this)
	{
#ifdef _EDITOR
		if (!Scene::EditorSetting.IsPlay())
			return;
#endif // _EDITOR
		DefferdRenderer& renderer = D3D11_GameApp::GetRenderer();
		UpdateCameraMatrix();
		renderer.SetCameraMatrix(inversView);
		if (isPerspective)
		{
			renderer.SetPerspectiveProjection(FOV * Mathf::Deg2Rad, Near, Far);
		}
		else
		{
			renderer.SetOrthographicProjection(Near, Far);
		}
	}
}

void Camera::LateUpdate()
{

}



