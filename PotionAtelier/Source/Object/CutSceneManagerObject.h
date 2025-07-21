#pragma once
#include <GameObject/Base/GameObject.h>
#include <framework.h>

class CutSceneObject : public GameObject
{
	SERIALIZED_OBJECT(CutSceneObject);
public:
	CutSceneObject();
	virtual ~CutSceneObject() override;
	class CutSceneComponet* componenet;
};
class CutSceneManagerObject : public GameObject
{
	SERIALIZED_OBJECT(CutSceneManagerObject);
public:
	CutSceneManagerObject();
	virtual ~CutSceneManagerObject() override;
};


class CutSceneManagerObject2 : public GameObject
{
	SERIALIZED_OBJECT(CutSceneManagerObject2);
public:
	CutSceneManagerObject2();
	virtual ~CutSceneManagerObject2() override;
};

