#pragma once
#include <Core/TSingleton.h>
#include <Utility/AssimpUtility.h>
#include <unordered_map>
#include <string>
#include <functional>
#include <queue>
#include <set>
#include <ranges>
#include <Utility/utfConvert.h>
#include <Scene\Base\Scene.h>
#include <Physics/PhysicsManager.h>
#include <mutex>

class Component;
extern class SceneManager& sceneManager;
using ObjectList = std::vector<GameObject*>;
class SceneManager : public TSingleton<SceneManager>
{
	friend TSingleton;
	friend class WinGameApp;
	friend class D3D11_GameApp;
	friend class RendererTestApp;
	friend class GameObject;
	friend LRESULT CALLBACK ImGUIWndProcDefault(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	SceneManager();
	~SceneManager();

	std::unique_ptr<Scene> currScene;
	std::unique_ptr<Scene> nextScene;

	std::queue<std::shared_ptr<GameObject>> currAddQueue; //���� ������Ʈ �߰� ��⿭
	std::queue<std::shared_ptr<GameObject>>	nextAddQueue; //���� ������Ʈ �߰� ��⿭

	std::set<GameObject*> eraseSet;		  //���� ������Ʈ ���� ��⿭
	std::set<Component*> eraseComponentSet;	      //������Ʈ ���� ��⿭

	std::list<std::pair<std::wstring, GameObject*>> resourceObjectList; //���ҽ��� ����� ������Ʈ ��⿭

	std::map<std::wstring, Scene::InstanceID> objectFindMap;
	std::map<std::wstring, GameObject*> objectFindMap2;

	std::mutex asynCallBackMutex;
	std::vector<std::function<void()>> asynCallBackVec;
	//�� �ʱ�ȭ ������ Ȯ�ο�
	bool isStartScene = false;
public:
	template <typename T>
	void LoadScene();
	void LoadScene(const wchar_t* scenePath);
	void SaveScene(const wchar_t* savePath, bool Override = false);
	void AddScene(const wchar_t* scenePath);
	void SubScene(const wchar_t* scenefileName);
	const std::wstring& GetLastLoadScenePath() { return lastLoadScenePath; }

	/*���� ��*/
	Scene* GetActiveScene() { return currScene.get(); }

	/*���� ���� ������Ʈ �߰�*/
	void AddGameObject(std::shared_ptr<GameObject>& object);

	void DestroyObject(GameObject* obj);
	void DestroyObject(GameObject& obj);

	void DontDestroyOnLoad(GameObject* obj);
	void DontDestroyOnLoad(GameObject& obj);

	GameObject* FindObject(const wchar_t* name);
	template<class T>
	GameObject* FindFirstObject();

	GameObject* GetObjectToID(unsigned int instanceID);

	size_t GetObjectsCount();
	ObjectList GetObjectList();
	std::vector<std::wstring> GetSceneList(); //�ε�� �� �׸� ��ȯ
	
	/*���� �� Imgui ��� ���� ��ȯ*/
	bool IsImGuiActive();

	/*ImGui �˾� �Լ� ��� *�ݵ�� �Լ� ������ PopImGuiPopupFunc()�� �˾� �����ؾ���.*/
	void PushImGuiPopupFunc(const std::function<void()>& func);
	void PopImGuiPopupFunc();

	/**Loding ȭ�� �Լ� ���.*/
	void SetLodingImguiFunc(const std::function<void()>& func);
	void EndLodingImguiFunc();

	/* CallBackFunc */
	void PushAsynCallBackFunc(const std::function<void()>& callBack);
private:
	//Update
	void FixedUpdateScene();
	void UpdateScene();
	void LateUpdateScene();

	//Render
	void RenderScene();
	void AddObjects();
	void EraseObjects();
	void ChangeScene();
	
private:
	void AddObjectCurrScene(std::shared_ptr<GameObject>& obj);
	void AddObjectNextScene(std::shared_ptr<GameObject>& obj);
	/*�ߺ� �̸� Ȯ�� �� �ߺ��̸� ��ȣ�� �ٿ� ����.*/
	std::wstring ChangeObjectName(unsigned int instanceID, const std::wstring& _pervName, const std::wstring& _newName);
	void EraseObject(unsigned int id);
	void EraseObjectFindMap(GameObject* obj);

	std::function<void()> ImGuiLodingFunc;
	bool EndGame = false;

	std::wstring lastLoadScenePath;

	void WaitThreads();
};

template<typename T>
inline void SceneManager::LoadScene()
{
	static_assert(std::is_base_of_v<Scene, T>, "T is not Scene");
	if (currScene && !currScene->ImGUIPopupQue.empty())
	{
		return;
	}
	PhysicsManager::ClearPhysicsScene();
	nextScene.reset(new T);
	nextScene->sceneName = utfConvert::utf8_to_wstring(typeid(T).name());
	resourceObjectList.clear();
}

template<class T>
inline GameObject* SceneManager::FindFirstObject()
{
	auto find = objectFindMap
		| std::views::filter([this](const auto& item)-> bool
							 {
								 if (currScene->objectList.size() <= item.second) return false;
								 return dynamic_cast<T*>(currScene->objectList[item.second].get());
							 })
		| std::views::transform([this](const auto& item) -> T*
								{
									return static_cast<T*>(currScene->objectList[item.second].get());
								})


		| std::views::take(1);
	if (find.empty())
	{
		return nullptr;
	}
	return find.front();
}

