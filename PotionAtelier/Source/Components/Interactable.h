#pragma once
#include "framework.h"

// Memo
// Space�ٷ� ��ȣ �ۿ� ������ ��ü ���̽� ������Ʈ
// �÷��̾� Sweep�� �ɸ� ù ��° ������Ʈ -> OnFocusIn
// ��� -> OnFocusOut
// Sweep�� �ɸ� ���·� ��ȣ�ۿ�Ű -> OnInteract

enum class InteractableType
{
	None,
	Cooker_Cut,
	Cooker_Grind,
	Cooker_Squeeze,
	Counter, // ��ǰ â��
	IngredientStand, // ����� ������
	TrashBin, // ��������
	EmptyTable, // ���� �÷����� ��
	Cauldron // ������
};

class Interactable : public Component
{
public:
	InteractableType type{ InteractableType::None };

	virtual void Awake() {}
protected:
	virtual void FixedUpdate() {}
	virtual void Update() {}
	virtual void LateUpdate() {}



public:
	virtual void OnFocusIn(class PlayerController* controller) {}
	virtual void OnFocusOut(class PlayerController* controller) {}
	virtual bool OnInteract(class PlayerController* controller) { return true; }

	virtual void OnAttacked(class PlayerController* controller) {}
};