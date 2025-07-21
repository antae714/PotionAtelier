#pragma once
#include <Component/Render/MeshRender.h>
#include <format>
#include <stack>

struct MatrixPallete
{
	Matrix MatrixPalleteArray[320]; 
	Matrix BoneWIT[320];
};

struct OffsetMatrices
{
	std::vector<Matrix> data;
};

class BoneObject;
class SimpleMaterial;
class SimpleBoneMeshRender : public MeshRender
{
	friend class GameObjectFactory;
public:
	struct Vertex
	{
		Vector4 position{ 0,0,0,1 };
		Vector3 normal;
		Vector3 Tangent;
		Vector2 Tex;
		int BlendIndecses[12] = {0,};
		float BlendWeights[12] = {0,};
	};
public:
	SimpleBoneMeshRender();
	virtual ~SimpleBoneMeshRender() override;
	virtual void Serialized(std::ofstream& ofs);
	virtual void Deserialized(std::ifstream& ifs);

private:
	/*Deserialized�� �׸��.*/
	inline static std::vector<SimpleBoneMeshRender*> DeserializedListVec;
	/*Deserialized ���� �� ȣ���ؾ��ϴ� �Լ�(GameObjectFactory���� ȣ��)*/
	static void EndDeserialized();

public:
	virtual void Awake() 		 override;
protected:
	virtual void FixedUpdate()	 override {};
	virtual void Update() 		 override {};
	virtual void LateUpdate()	 override {};
	virtual void UpdateMeshDrawCommand() override;

	/*SetMeshID ���� ȣ��Ǵ� �����Լ�*/
	virtual void LoadMeshResource(std::filesystem::path& meshResourcePath) override;
	virtual void SaveMeshResource(std::filesystem::path& meshResourcePath) override;
public:
	virtual void CreateMesh() override;

public:
	std::vector<Vertex> vertices;
	std::vector<UINT>   indices;

	/*�ֻ��� �θ���� ��ȸ�ϸ鼭 Bone�� ã�� ����մϴ�.*/
	void AddBonesFromRoot();
	std::shared_ptr<OffsetMatrices> offsetMatrices = nullptr;
private:
	MatrixPallete matrixPallete = {};
	std::vector<BoneObject*> boneList;
	ConstantBuffer matrixPalleteConstBuffer;
};

