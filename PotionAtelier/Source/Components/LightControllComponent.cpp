#include "LightControllComponent.h"
#include "framework.h"	
#include "Object\GameManager.h"	
LightControllComponent::LightControllComponent()
{
}

void LightControllComponent::Serialized(std::ofstream& ofs)
{
	Binary::Write::data(ofs, 1);
	Binary::Write::data(ofs, lightType);
	Binary::Write::data(ofs, lightIndex);
	Binary::Write::data(ofs, LightKeyFrameData.size());
	for (auto& item : LightKeyFrameData)
	{
		Binary::Write::data(ofs, item.time);
		Binary::Write::data(ofs, item.blendType);
		if (lightType == 0)
		{
			Binary::Write::Vector4(ofs, item.directLight.Color);
			Binary::Write::Vector4(ofs, item.directLight.Directoin);
		}
		else if (lightType == 1)
		{
			Binary::Write::Vector4(ofs, item.pointLight.Color);
			Binary::Write::Vector4(ofs, item.pointLight.position);
		}
	}
}

void LightControllComponent::Deserialized(std::ifstream& ifs)
{
	int version = Binary::Read::data<int>(ifs);
	lightType = Binary::Read::data<int>(ifs);
	lightIndex = Binary::Read::data<int>(ifs);
	size_t size = Binary::Read::data<size_t>(ifs);
	LightKeyFrameData.resize(size);
	for (auto& item : LightKeyFrameData)
	{
		item.time = Binary::Read::data<float>(ifs);
		item.blendType = Binary::Read::data<int>(ifs);
		if (lightType == 0)
		{
			item.directLight.Color = Binary::Read::Vector4(ifs);
			item.directLight.Directoin = Binary::Read::Vector4(ifs);
		}
		else if (lightType == 1)
		{
			item.pointLight.Color = Binary::Read::Vector4(ifs);
			item.pointLight.position = Binary::Read::Vector4(ifs);
		}
	}
}

void LightControllComponent::InspectorImguiDraw()
{
	if (ImGui::TreeNode("LightControllComponent"))
	{
		ImGui::DragFloat("CurrentTime", &currentTime, 0.1f, 0.0f, 1.0f);
		ImGui::Combo("LightType", &lightType, "Directional\0Point\0");
		ImGui::InputInt("LightIndex", &lightIndex);

		if (ImGui::Button("Add KeyFrame"))
		{
			LightKeyFrameData.emplace_back();
		}

		std::ranges::sort(LightKeyFrameData, [](const LightData& a, const LightData& b) { return a.time < b.time; });
		for (size_t i = 0; i < LightKeyFrameData.size(); i++)
		{
			ImGui::BeginChild(std::format("{}", i).c_str(),
							  ImVec2(0, 0),
							  ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize, ImGuiWindowFlags_NoMove);
			if (ImGui::BeginPopupContextWindow())
			{
				bool isDelete = false;
				if (ImGui::Button("Delete"))
				{
					LightKeyFrameData.erase(LightKeyFrameData.begin() + i);
					ImGui::CloseCurrentPopup();
					i--;
					isDelete = true;
				}
				ImGui::EndPopup();
				if (isDelete)
				{
					ImGui::EndChild();
					continue;
				}
			}

			ImGui::DragFloat("Time", &LightKeyFrameData[i].time);

			ImGui::Combo("BlendType", &LightKeyFrameData[i].blendType, "Linear\SmoothStep\0");
			if (lightType == 0)
			{
				Vector3 dir;
				float intensity = LightKeyFrameData[i].directLight.Color.w;
				dir = Vector3(LightKeyFrameData[i].directLight.Directoin);

				ImGui::ColorEdit4("Color", &LightKeyFrameData[i].directLight.Color);
				ImGui::DragVector3("Direction", &dir, 0.01f, -1.f, 1.f);
				ImGui::DragFloat("Intensity", &intensity, 1.f, 0.0000001f, 100.f);

				LightKeyFrameData[i].directLight.Color.w = intensity;
				LightKeyFrameData[i].directLight.Directoin = Vector4(dir.x, dir.y, dir.z, 0);
			}
			else if (lightType == 1)
			{
				Vector3 dir;
				float intensity = LightKeyFrameData[i].directLight.Color.w;
				dir = Vector3(LightKeyFrameData[i].pointLight.position);

				ImGui::ColorEdit4("Color", &LightKeyFrameData[i].pointLight.Color);
				ImGui::DragVector3("Position", &dir, 0.01f, -1.f, 1.f);
				ImGui::DragFloat("Intensity", &intensity, 1.f, 0.0000001f, 100.f);

				LightKeyFrameData[i].pointLight.Color.w = intensity;
				LightKeyFrameData[i].pointLight.position = Vector4(dir.x, dir.y, dir.z, 0);
			}
			ImGui::EndChild();
		}
		ImGui::TreePop();
	}
}

// ���� ���� �Լ�: a�� b ���̸� t ������ �����մϴ�.
float4 LerpLinear(const float4& a, const float4& b, float t)
{
	return a + (b - a) * t;
}

// ������ ���� ���� �Լ�: t ���� ������ ���� �Լ��� �����Ͽ� �����մϴ�.
float4 LerpSmoothStep(const float4& a, const float4& b, float t)
{
	float smoothT = t * t * (3.0f - 2.0f * t);
	return a + (b - a) * smoothT;
}

// blendType ���� ���� ������ ���� ����� �����ϴ� �Լ�.
// ��: blendType == 0�̸� ���� ����, blendType == 1�̸� ������ ���� ����
float4 Interpolate(const float4& a, const float4& b, float t, int blendType)
{
	switch (blendType)
	{
	case 0: // ���� ����
		return LerpLinear(a, b, t);
	case 1: // ������ ���� ����
		return LerpSmoothStep(a, b, t);
	default:
		// �⺻ ���� ����� ���� �������� ó���մϴ�.
		return LerpLinear(a, b, t);
	}
}


void LightControllComponent::Update()
{
#ifdef _EDITOR
	if (!Scene::EditorSetting.IsPlay()) return;
#endif
	auto& renderer = D3D11_GameApp::GetRenderer();
	currentTime = GameManager::GetGM().GetProgressPercentage();

	// Ű������ �����Ͱ� ����ִ� ��츦 üũ
	if (LightKeyFrameData.empty())
		return;

	// currentTime�� ù Ű�����Ӻ��� �۰ų� ������ ù Ű������ ���,
	// ������ Ű�����Ӻ��� ũ�� ������ Ű������ ���.
	if (currentTime <= LightKeyFrameData.front().time)
	{
		currentCount = 0;
	}
	else if (currentTime >= LightKeyFrameData.back().time)
	{
		currentCount = static_cast<int>(LightKeyFrameData.size()) - 1;
	}
	else
	{
		// LightKeyFrameData�� �ð� ������ ���ĵǾ� �ִٰ� ����.
		while (currentCount < static_cast<int>(LightKeyFrameData.size()) - 1 &&
			   LightKeyFrameData[currentCount].time < currentTime)
		{
			++currentCount;
		}
	}

	// ���� Ű�����Ӱ� ���� Ű������ ���̿��� ������ �����մϴ�.
	if (currentCount == 0)
	{
		// ù Ű�������� ��쿡�� ���� ���� ���� �״�� ���.
		if (lightType == 0)
		{
			renderer.directLight.GetDirectLight(lightIndex).Color =
				LightKeyFrameData[currentCount].directLight.Color;
			renderer.directLight.GetDirectLight(lightIndex).Directoin =
				LightKeyFrameData[currentCount].directLight.Directoin;
		}
		else if (lightType == 1)
		{
			renderer.pointLight.GetPointLight(lightIndex).Color =
				LightKeyFrameData[currentCount].pointLight.Color;
			renderer.pointLight.GetPointLight(lightIndex).position =
				LightKeyFrameData[currentCount].pointLight.position;
		}
	}
	else
	{
		// kf0: ���� Ű������, kf1: ���� Ű������
		const auto& kf0 = LightKeyFrameData[currentCount - 1];
		const auto& kf1 = LightKeyFrameData[currentCount];

		float timeSpan = kf1.time - kf0.time;
		float t = (currentTime - kf0.time) / timeSpan;

		// ���� ����� kf1�� blendType ���� ��� (�ʿ信 ���� kf0�̳� �� ���� ������ ����� �� ����)
		int blendType = kf1.blendType;

		if (lightType == 0)
		{
			// ���⼺ ����Ʈ: ����� ������ �����մϴ�.
			renderer.directLight.GetDirectLight(lightIndex).Color =
				Interpolate(kf0.directLight.Color, kf1.directLight.Color, t, blendType);
			renderer.directLight.GetDirectLight(lightIndex).Directoin =
				Interpolate(kf0.directLight.Directoin, kf1.directLight.Directoin, t, blendType);
			// �ʿ�� VP ��� ������ ��� (��, ��� ������ ���� ó�� �ʿ�)
		}
		else if (lightType == 1)
		{
			// ����Ʈ ����Ʈ: ����� ��ġ�� �����մϴ�.
			renderer.pointLight.GetPointLight(lightIndex).Color =
				Interpolate(kf0.pointLight.Color, kf1.pointLight.Color, t, blendType);
			renderer.pointLight.GetPointLight(lightIndex).position =
				Interpolate(kf0.pointLight.position, kf1.pointLight.position, t, blendType);
		}
	}
}

LightControllComponent::LightData::LightData() :
	time(0.0f),
	blendType(0)
{
}
