#pragma once
#include "framework.h"
#include "../HoldableTypes.h"
#include "../ResourceFinder.h"
#include "Interactable.h"

// �÷��̾� ������Ʈ ���� �� �ؾߵǴ� ��
// 1. �� �� �ڽ����� HoldingObject(�±� �ޱ� �ʼ�) �ֱ�
// 2. ���� �տ� HoldAnchor �±� �ޱ� -> ���ڷ� �̰� �������� ��

class PlayerControllHelper
{
public:
	bool lock_move{ false };
	bool lock_interact{ false };
	bool lock_attack{ false };
	bool lock_change_animation{ false };
	bool lock_change_animation_until_focus_out_or_QTE_done{ false };

	class PlayerController* controller{ nullptr };
	TransformAnimation* animator{ nullptr };

	void PlayAnimation(PlayerAnimType type);
};


class PlayerController : public Component
{
	friend class Throwable;
	friend class Cooker;
	friend class CuttingBoardCooker;

	PlayerControllHelper helper;

	/* Character Control */
	CharacterController* controller{ nullptr };
	float max_speed{ 100.0f };
	float shot_ray_interval{ 0.1f };
	float ray_distance{ 5.0f };
	Vector2 ray_size{ 0.75f, 0.75f };

	// ��ȣ�ۿ� ���̴� ���������� ���� Ű ��������
	Vector3 separated_interact_direction{ 1, 0, 0 };
	// ĳ������ ȸ���� �����ؼ� �ε巴�� ������ ��
	float superficial_rotation{ 0 };
	float rotate_speed{ 5.0f };
	bool smooth_rotation{ true };
	bool fasten_rotation_speed_by_delta_angle{ true };
	bool separate_interact_direction{ false };

	/* Animation Controll */
	TransformAnimation* player_animator{ nullptr };
	float max_stamina{ 100.0f };
	float stamina{ 100.0f };
	float dash_speed_increase_mult{ 1.5f };
	float dash_consume_stamina_per_second{ 60.0f };
	float stamina_recover_amount_per_second{ 20.0f };
	bool stamina_exhausted{ false };
	//float dash_speed_proportion{ 4.0f };	// �뽬 �ӵ� ������ 
	//float dash_duration{ 0.1f };			// �뽬 ���ӽð�
	//float dash_cooldown{ 1.0f };			// �뽬 ��Ÿ��
	//float dash_deceleration{ 50.0f };		// �뽬 ����
	//float dash_timer{ 0.0f };
	//float dash_cooldown_timer{ 0.0f };

	/* Stamina UI */
	Vector2 ui_stamina_size{ 96, 24 };
	Vector2 ui_stamina_anchor{ 0, -80 };
	Vector2 ui_stamina_fill_size{ 90, 18 };
	Vector2 ui_stamina_fill_anchor{ 0, -80 };
	Color stamina_fill_color_exhausted{ 1, 0, 0, 1 };
	Color stamina_fill_color_full{ 0, 1, 0, 1 };
	class UIPoping* ui_stamina_background{ nullptr };
	class UIPoping* ui_stamina_fill{ nullptr };

	class TutorialManagerComponent* tutorial_manager{ nullptr };

	/* Interaction */
	KeyboardKeys attack_key{ KeyboardKeys::X };
	KeyboardKeys interact_key{ KeyboardKeys::Space };
	KeyboardKeys dash_key{ KeyboardKeys::LeftCtrl };

	class Interactable* current_focus{ nullptr };
	bool interact_at_current_frame{ false };


	// �����ϴ� �ͺ��� ������ �����Ӱ� �����̰� �δ� �� ����
	//bool on_cooker{ false };
	//class Cooker* current_use_cooker{ nullptr };


	// Start() ȣ���� �� �ε� �ð����� PhysicsScene ������Ʈ ������
	class Scene* current_scene{ nullptr };



	ParticleSpawnComponent* VFX_Run{ nullptr };
	std::string run{};
	
	float particle_spawn_interval{ 0.1f };

	/* Holding */
public:
	class Holding* hand_graphic{ nullptr };
	class Holding* focus_holding{ nullptr };
	bool something_on_hand{ false };
	HoldableType hold_type{ HoldableType::None };
	UINT hold_subtype{ 0 };


	/* Broom */
	bool on_attack{ false };
	Vector3 broom_scale_goal{};
	Vector3 broom_scale_init{ 0, 0, 0 };
	float broom_showing_t{ 0.0f };
	float broom_showing{ false };
	float broom_showing_speed{ 12.0f };
	float broom_maintain_sec{ 1.0f };
	class BroomComponent* broom{ nullptr };


	/* Knife */
	GameObject* knife{ nullptr };
	GameObject* grinder{ nullptr };



	class AudioBankClip* holdAudio{ nullptr };
	class AudioBankClip* dropAudio{ nullptr };
	class AudioBankClip* SFX_NotValid{ nullptr };

private:
	class GameManagerComponent* gm{ nullptr };

public:
	virtual ~PlayerController() = default;
	virtual void Awake();
protected:
	virtual void Start();
	virtual void FixedUpdate() {}
	virtual void Update();
	virtual void LateUpdate() {}

	void Move();
	void RayForward();
	void Interact();

private:
	bool stamina_ui_show{ false };
	void ShowStaminaUI();
	void AdjustStaminaUI();
	void HideStaminaUI();

	//void Cook();
	// �̰� �ƴ϶� Cooker�� Cooker��� �۾� �Ϸ�� �� Interact�ϸ� ������ ��Ḧ ��ȯ���ִ� ���·�
	//void TEST_CookDone();
public:
	void Pick(HoldableType type, UINT sub_type);
	std::pair<HoldableType, UINT> Swap(HoldableType type, UINT sub_type);
	void PutDown();

	void InformQTEDone();
	void PlayCookingAnimation(InteractableType type);
private:
	void FindBroom(Transform* parent);
	void FindKnife(Transform* parent);
	void FindGrinder(Transform* parent);
	class Holding* FindHoldingObject(Transform* parent);
	void FindAnimationObject(Transform* parent);



public:
	virtual void InspectorImguiDraw() override;

	virtual void Serialized(std::ofstream& ofs) override;
	virtual void Deserialized(std::ifstream& ifs) override;
};