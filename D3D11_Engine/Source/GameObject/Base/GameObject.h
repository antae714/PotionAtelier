#pragma once
#include <Core/Transform.h>
#include <Utility/Console.h>
#include <memory>
#include <string>
#include <Manager/GameObjectFactory.h>
#include <Utility/ExceptionUtility.h>

#include <Physics/Struct/CollisionInfo.h>

#include <Core/EventManager.h>


#pragma warning( disable : 4267)

class Component;
class TransformAnimation;
class RenderComponent;
class Rigidbody;
class Collider;
class BoxCollider;
class SphereCollider;
class CapsuleCollider;
class MeshCollider;
class Interactable;

class GameObject
{		 
	SERIALIZED_OBJECT(GameObject)
	friend class Scene;
	friend class SceneManager;
	friend class D3DRenderer;
	friend class PhysicsManager;
	friend class Component;
public:
	GameObject();
	/*�̸�, �ν��Ͻ� ���̵� �ο� �� ȣ��Ǵ� �Լ�.*/
	virtual void Awake() {};
	virtual ~GameObject();

	/** �߰������� ����ȭ�� ������ �ʿ�� �������̵�*/
	virtual void Serialized(std::ofstream& ofs) {};
	/** �߰������� ����ȭ�� ������ �ʿ�� �������̵�*/
	virtual void Deserialized(std::ifstream& ifs) {};

	virtual void InspectorImguiDraw();
public:
	static void Destroy(GameObject& obj);
	static void Destroy(GameObject* obj);
	static void DestroyComponent(Component& component);
	static void DestroyComponent(Component* component);
	static void DontDestroyOnLoad(GameObject& obj);
	static void DontDestroyOnLoad(GameObject* obj);
	static GameObject* Find(const wchar_t* name);
	template<typename ObjectType>
	static ObjectType* Find(const wchar_t* name)
	{
		return dynamic_cast<ObjectType*>(Find(name));
	}
	template<class T>
	static T* FindFirst();

	template<typename ObjectType>
	inline static ObjectType* NewGameObject(const wchar_t* name);

	/*���� �Ҵ縸*/
	template<typename ObjectType>
	inline static std::shared_ptr<GameObject> MakeGameObject(const wchar_t* name);
public:
	unsigned int GetInstanceID() const { return instanceID; }
	const std::wstring& GetName() const { return name; }
	const std::wstring& SetName(const wchar_t* _name);
	std::string GetNameToString() const;

	bool SetTag(const std::wstring& tag);
	void UnsetTag(const std::wstring& tag)
	{
		auto find = tagSet.find(tag);
		if (find != tagSet.end())
		{
			tagSet.erase(find);
		}
	}
	const std::set<std::wstring> GetTags() const { return tagSet; }
	bool HasTag(const std::wstring& tag) const { return tagSet.find(tag) != tagSet.end(); }

private:
	std::wstring name;
	std::set<std::wstring> tagSet;
	unsigned int instanceID = -1;

public:
	Transform transform;
	bool Active = true;

	/*�θ� Active ���� Ȱ��ȭ ����*/
	bool GetWorldActive() const;
public:
	/*������Ʈ �̸�. (�ߺ� ����)*/
	_declspec (property(get = GetName, put = SetName)) std::wstring& Name;

	/*������Ʈ �߰�*/
	template <typename T>
	T& AddComponent();

	/*TransformAnimation�� �ϳ��� ���� ����.*/
	template <>
	TransformAnimation& AddComponent();

	/*������Ʈ ��������*/
	template <typename T>
	T& GetComponent();

	/*������Ʈ ��������. (������ nullptr ��ȯ)*/
	template <typename T>
	T* IsComponent();

	/*������Ʈ �ּҷ� �ε��� Ȯ���ϱ�. ������ -1 ��ȯ*/
	int GetComponentIndex(Component* findComponent);

	/*�ε����� ������Ʈ ��������. �Ķ���ͷ� ĳ������ ������Ʈ Ÿ���� ����.*/
	template <typename T>
	T* GetComponentAtIndex(int index);

	/*�� ������Ʈ�� ������Ʈ ����*/
	int GetComponentCount() { return componentList.size(); }

	DirectX::BoundingOrientedBox GetOBBToWorld() const;
	DirectX::BoundingBox GetBBToWorld() const;

	DirectX::BoundingBox Bounds;

	/*ī�޶� �ø� ����*/
	inline bool IsCameraCulling() const { return isCulling; }

	/*���� ���� �θ������ ���� ������ ��ȯ���ݴϴ�.*/
	std::vector<GameObject*> GetHierarchyToParent(GameObject* TargetParent);
	void GetHierarchyToParent(std::vector<GameObject*>& OutVector);
private:
	void Start();
	void FixedUpdate();
	void Update();
	void LateUpdate();
	void Render();
private:
	void EraseComponent(Component* component);
public:
	virtual void OnCollisionEnter(CollisionInfo info) {};
	virtual void OnCollisionStay(CollisionInfo info) {};
	virtual void OnCollisionExit(CollisionInfo info) {};
	virtual void OnTriggerEnter(CollisionInfo info) {};
	virtual void OnTriggerStay(CollisionInfo info) {};
	virtual void OnTriggerExit(CollisionInfo info) {};





private:
	std::vector<std::unique_ptr<Component>> componentList;
	std::vector<RenderComponent*> renderList;
	std::vector<Component*> startList;

	std::vector<Collider*> colliders;
	/* [Warning!!!] Interactable�� �� ���� ã�� ���� �ӽ÷� �־�� */
	Interactable* interactable{ nullptr };

public:
	std::vector<Collider*>& GetEveryCollider();
	Interactable* _GetInteractableComponent() { return interactable; }


public:
	std::weak_ptr<GameObject> GetWeakPtr() { return myptr; }
private:
	std::weak_ptr<GameObject> myptr;
	bool isCulling = false; //�̹� ������ ī�޶� �ø� ����

protected:
	class PhysicsActor* physicsActor{ nullptr };
	unsigned int phyiscsLayer{ 0 };
public:
	class PhysicsActor* GetPhysicsActor() { return physicsActor; }
	class PhysicsActor** GetPhysicsActorAddress() { return &physicsActor; }
	void SetPhysicsLayer(unsigned int slot);
	unsigned int GetPhysicsLayer() { return phyiscsLayer; }
};

template<typename T>
inline T& GameObject::AddComponent()
{
	static_assert(std::is_base_of_v<Component, T>, "is not Component");

	T* nComponent = new T;
	nComponent->SetOwner(this);
	nComponent->index = componentList.size();
	nComponent->Awake();
	componentList.emplace_back(nComponent);	
	startList.emplace_back(nComponent); //�ʱ�ȭ �׸�
	if constexpr (std::is_base_of_v<RenderComponent, T>)
	{
		renderList.push_back(nComponent);
	}
	if constexpr (std::is_base_of_v<Collider, T>)
	{
		colliders.push_back(nComponent);
	}
	if constexpr (std::is_base_of_v<Interactable, T>)
	{
		interactable = static_cast<Interactable*>(nComponent);
	}

	return *nComponent;
}

template<typename T>
inline T& GameObject::GetComponent()
{
	static_assert(std::is_base_of_v<Component, T>, "is not Component");

	for (auto& component : componentList)
	{
		if (typeid(*component) == typeid(T))
		{
			return static_cast<T&>(*component);
		}
	}
	__debugbreak(); //����) �������� �ʴ� ������Ʈ
	throw_GameObject("Exception : Component does not exist", this);
}

template<typename T>
inline T* GameObject::IsComponent()
{
	static_assert(std::is_base_of_v<Component, T>, "is not Component");

	for (auto& component : componentList)
	{
		if (typeid(*component) == typeid(T))
		{
			return static_cast<T*>(component.get());
		}
	}
	return nullptr;
}

template<typename T>
inline T* GameObject::GetComponentAtIndex(int index)
{
	static_assert(std::is_base_of_v<Component, T>, "is not Component");

	if (0 <= index && index < componentList.size())
	{
		T* component = dynamic_cast<T*>(componentList[index].get());
		return component;
	}
	Debug_printf("warrnig : GetComponentAtIndex(int index), index is out of range!\n");
	return nullptr;
}

template<typename ObjectType = GameObject>
ObjectType* NewGameObject(const wchar_t* name)
{
	return GameObject::NewGameObject<ObjectType>(name);
}

template<typename ObjectType = GameObject>
std::shared_ptr<GameObject> MakeGameObject(const wchar_t* name)
{
	return GameObject::MakeGameObject<ObjectType>(name);
}

#include <Manager/InstanceIDManager.h>

template<class T>
inline T* GameObject::FindFirst()
{
	return dynamic_cast<T*>(sceneManager.FindFirstObject<T>());
}


template<typename ObjectType>
inline ObjectType* GameObject::NewGameObject(const wchar_t* name)
{
	static_assert(std::is_base_of_v<GameObject, ObjectType>, "is not gameObject");

	unsigned int id = instanceIDManager.getUniqueID();
	void* pointer = gameObjectFactory.GameObjectAlloc(id);
	if (pointer)
	{
		ObjectType* pObject = reinterpret_cast<ObjectType*>(pointer);
		std::shared_ptr<ObjectType> newObject(pObject, GameObjectFactory::GameObjectDeleter);
		std::shared_ptr<GameObject> baseObject = std::static_pointer_cast<GameObject>(newObject);

		new(pObject)ObjectType();
		baseObject->instanceID = id;
		baseObject->myptr = baseObject;
		baseObject->Name = name;
		baseObject->Awake();

		sceneManager.AddGameObject(baseObject);
		return newObject.get();
	}
	else
	{
		__debugbreak();
		return nullptr;
	}

}

template<typename ObjectType>
inline std::shared_ptr<GameObject> GameObject::MakeGameObject(const wchar_t* name)
{
	static_assert(std::is_base_of_v<GameObject, ObjectType>, "is not gameObject");
	static std::mutex mut;

	mut.lock();
	unsigned int id = instanceIDManager.getUniqueID();
	void* pointer = gameObjectFactory.GameObjectAlloc(id);
	mut.unlock();
	if (pointer)
	{
		ObjectType* pObject = reinterpret_cast<ObjectType*>(pointer);
		std::shared_ptr<ObjectType> newObject(pObject, GameObjectFactory::GameObjectDeleter);
		std::shared_ptr<GameObject> baseObject = std::static_pointer_cast<GameObject>(newObject);

		new(pObject)ObjectType();
		baseObject->instanceID = id;
		baseObject->myptr = baseObject;
		baseObject->Name = name;
		baseObject->Awake();
		return baseObject;
	}
	else
	{
		__debugbreak();
		return nullptr;
	}
}
