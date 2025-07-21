#pragma once
#include "Interactable.h"

// Memo
// �ֹ� �ⱸ ���̽� ������Ʈ
// ����ؼ� ����ϼ���.


class Cooker : public Interactable
{
protected:


public:
	virtual ~Cooker() = default;
	Cooker();

public:
	virtual void Awake() {}
protected:
	virtual void FixedUpdate() {}
	virtual void Update() {}
	virtual void LateUpdate() {}

public:
	virtual void OnCook(class PlayerController* controller) abstract;

protected:
	GameObject* FindHoldingObject(Transform* parent);
};
