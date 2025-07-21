#pragma once
enum HoldableType
{
	None = (1 << 0),
	Ingredient = (1 << 1),
	Potion = (1 << 2)
};

enum IngredientType
{
	// Raw
	DragonTail				= (1 << 0),
	MagicFlower				= (1 << 1),
	MagicWood				= (1 << 2),

	//Processed
	SlicedDragonTail		= (1 << 3),
	CompressedDragonTail	= (1 << 4),
	SlicedMagicWood			= (1 << 5),
	GrindedMagicWood		= (1 << 6),
	GrindedMagicFlower		= (1 << 7),
	CompressedMagicFlower	= (1 << 8),

	Invalid					= (1 << 9)
};

enum PotionType
{
	//��ü �Ҹ�
	FailurePotion			= (1 << 0),
	//ġ��
	HealthPotion			= (1 << 1),
	//�ߵ�
	AddictionPotion			= (1 << 2),
	//ȭ��
	FlamePotion				= (1 << 3),
	//��Ȥ
	SeductionPotion			= (1 << 4),
	//����
	ManaPotion				= (1 << 5),
	//����
	FrozenPotion			= (1 << 6),
	//������
	RainbowPotion			= (1 << 7)
};

inline constexpr const char* GetPotionName(PotionType potion)
{
	switch (potion)
	{
	case FailurePotion:
		return "FailedPotion";
	case HealthPotion:
		return "HealthPotion";
	case AddictionPotion:
		return "AddictionPotion";
	case FlamePotion:
		return "FlamePotion";
	case SeductionPotion:
		return "SeductionPotion";
	case ManaPotion:
		return "ManaPotion";
	case FrozenPotion:
		return "FrozenPotion";
	case RainbowPotion:
		return "SpecialPotion";
	default:
		return "Null";
	}
}