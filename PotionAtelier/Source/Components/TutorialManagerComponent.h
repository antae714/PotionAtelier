#pragma once
#include "framework.h"
#include "Interactable.h"

enum class TutorialEventType
{
	DASH, // PlayerController

	CUTTING_START, // CuttingBoardCooker
	CUTTING_DONE,
	SQUEEZING_START, // SqueezerCooker
	SQUEEZING_DONE,
	GRINDING_START, // HandMillCooker
	GRINDING_DONE,
	
	GET_INGREDIENT_FROM_INGREDIENT_STAND, // IngredientStand
	INPUT_INGREDIENT_INTO_CAULDRON, // Cauldron
	BOILING_DONE, // Cauldron
	GET_POTION_FROM_CAULDRON,

	SUBMIT_POTION_VIA_COUNTER, // DeliverCounter

	// ������ �̺�Ʈ�� ���� �߻����� ����.
	// TEMP
	GET_CLOSE_TO_ANGRY_COUNTER // PlayerController
};



class TutorialManagerComponent : public Component
{
	// �÷��̾� ��ǳ��
	GameObject* player{ nullptr };
	class PlayerController* player_controller{ nullptr };
	class TextBubble* player_textbubble{ nullptr };

	// ��� Ʃ�丮�� �޼���
	class TextPoping* upper_text{ nullptr };
	class UIPoping* upper_text_ui{ nullptr };

	// ������
	Vector2 upper_text_size{ 591, 167 };
	Vector2 upper_text_anchor{ 0, -418 };
	Vector2 upper_text_additional_anchor{ 0, -8 };
	float upper_text_scale{ 0.5f };
	Vector2 tutorial_popup_size{ 1360, 815 };

	// Ʃ�丮�� �˾�
	class UIRenderComponenet* tuto_popup{ nullptr };
	GameObject* tuto_popup_object{ nullptr };
	GameObject* tuto_skip_object{ nullptr };
	GameObject* tuto_Arrow_object{ nullptr };

	bool on_tutorial{ false };
	int step{ 0 };
	int sub_step{ 0 };

	Vector3 tutorial_37_close_position{ 0, 0, 0 };
	// �̰� ��� �� ã���� �ȵǴϱ� �ð� �������� ���ݾ� ������
	float tutorial_37_close_dist_threshold{ 20.0f };

	// �����̽��� ������ ��ǳ�� ��ȯ
	KeyboardKeys turn_text_bubble_key{ KeyboardKeys::Space };
	bool process_tutorial_by_press_key{ false };

public:
	bool lock_move{ false };

	bool lock_interact{ false };
	// lock_interact ���μ���
	bool lock_interact_cutting_board{ false };
	bool lock_interact_squeezer{ false };
	bool lock_interact_grinder{ false };
	bool lock_interact_counter{ false };
	bool lock_interact_trash_bin{ false };
	bool lock_interact_cauldron{ false };
	bool lock_interact_empty_table{ false };

	bool lock_interact_ingredient_stand{ false };
	// lock_interact_ingredient_stand ���μ���
	bool lock_interact_ingredient_stand_type_dragon_tail{ false };
	bool lock_interact_ingredient_stand_type_magic_flower{ false };
	bool lock_interact_ingredient_stand_type_magic_wood{ false };
	
	std::map<TutorialEventType, std::vector<std::function<void()>>> events;


	bool OnTutorial() { return on_tutorial; }
	void ListenEvent(TutorialEventType type);
	bool CanInteract(Interactable* interactable);
	void StartTutorial();
private:
	void LockAll();
	void UnlockAll();
	void ProcessTutorial();

	std::vector<std::function<void()>> step_funcs;
	void Step1();
	void Step2();
	void Step3();
	void Step4();
	void Step5();
	void Step6();
	void Step7();
	void Step8();
	void Step9();
	void Step10();
	void Step11();
	void NesxtScene();
	void Step12();

	void ShowUpperText();
	void HideUpperText();
	void SetUpperText(std::wstring_view text);





public:
	virtual void Awake() override {}
protected:
	virtual void Start() override;
	virtual void FixedUpdate() override {}
	virtual void Update() override;
	virtual void LateUpdate() override {}

public:
	virtual void InspectorImguiDraw() override;
	virtual void Serialized(std::ofstream& ofs) override;
	virtual void Deserialized(std::ifstream& ifs) override;
};

