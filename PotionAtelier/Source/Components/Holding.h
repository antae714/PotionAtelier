#pragma once
#include <map>
#include <string>
#include "framework.h"
#include "../HoldableTypes.h"

// Memo
// �̰� �׳� ��� �ִ� �޽��� �ٲٴ� �뵵�θ� ����
// ��� �ִ� ���� ���� ó���� �׳� SamplerCharacterController���� ó���ϰ�

class Holding : public Component
{
	GameObject* current_mesh{ nullptr };

public:
	virtual void Awake();
	virtual void Start();
protected:
	virtual void FixedUpdate() {}
	virtual void Update() {}
	virtual void LateUpdate() {}



public:
	bool SetType(HoldableType type, UINT sub_type);
	void SetEmpty();
	void SetFocus(bool isFocus);

	virtual void InspectorImguiDraw() override;
	//virtual void Serialized(std::ofstream& ofs) override;
	//virtual void Deserialized(std::ifstream& ifs) override;
private:
	std::wstring GetObjectTag(HoldableType type, UINT sub_type);
};

