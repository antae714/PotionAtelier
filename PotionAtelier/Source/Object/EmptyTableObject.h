#pragma once
#include "framework.h"

class EmptyTableObject : public GameObject
{
	SERIALIZED_OBJECT(EmptyTableObject);

public:
	void Awake() override;
};