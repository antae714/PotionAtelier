#include "VideoObject.h"
#include <mfmediaengine.h>
#include <mfapi.h>
#include <mfobjects.h>
#include <mferror.h>
#include <mfidl.h>
#include <windows.h>
#include <iostream>
#include <filesystem>
#include <dcomp.h>
#include <Object\BackGroundMusicObject.h>

#pragma comment ( lib, "mfplat.lib")
#pragma comment ( lib, "Mf.lib")
#pragma comment ( lib, "dcomp.lib")

std::atomic<bool> load_complete = false;
std::atomic<bool> Video_complete = false;
class MediaEngineNotify : public IMFMediaEngineNotify
{
private:
	std::atomic<ULONG> refCount; // ���� ī��Ʈ�� ���� ����

public:
	MediaEngineNotify() : refCount(1) {} // �ʱ�ȭ

	// IMFMediaEngineNotify��(��) ���� ��ӵ�
	HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) override
	{
		if (__uuidof(IMFMediaEngineNotify) == riid)
		{
			*ppvObject = static_cast<IMFMediaEngineNotify*>(this);
			AddRef(); // ���� ī��Ʈ ����
			return S_OK;
		}

		*ppvObject = nullptr;
		return E_NOINTERFACE;
	}

	ULONG __stdcall AddRef(void) override
	{
		return refCount.fetch_add(1) + 1; // ���� ī��Ʈ�� ������Ű�� ��ȯ
	}

	ULONG __stdcall Release(void) override
	{
		ULONG newCount = refCount.fetch_sub(1) - 1; // ���� ī��Ʈ�� ���ҽ�Ű�� ��ȯ
		if (newCount == 0)
		{
			delete this; // ���� ī��Ʈ�� 0�� �Ǹ� ��ü�� ����
		}
		return newCount;
	}

	HRESULT __stdcall EventNotify(DWORD event, DWORD_PTR param1, DWORD param2) override
	{
		// �̺�Ʈ ó�� ������ ���⿡ �߰�
		switch (event)
		{
		case MF_MEDIA_ENGINE_EVENT_LOADEDMETADATA:
			break;
		case MF_MEDIA_ENGINE_EVENT_ERROR:
			// ���� �߻� �� ó��
			break;
			// �߰� �̺�Ʈ ���̽��� ���⿡ �߰��� �� �ֽ��ϴ�.
		default:
			break;
		}
		return S_OK; // ���������� �̺�Ʈ�� ó���� ���
	}
};
class VideoComponent : public Component
{
public:
    virtual ~VideoComponent()
    {
		HRESULT result;

		if (attributes)
		{
			attributes->DeleteAllItems();
			result = mediaEngine->Shutdown();
			attributes.ReleaseAndGetAddressOf();
			mediaEngine.ReleaseAndGetAddressOf();


			isCreditEnd = true;
			result = MFShutdown();
		}

		th.join();
    }
	virtual void Awake() {}

	ComPtr<IDCompositionDevice> dcompDevice;
	ComPtr<IDCompositionTarget> dCompositionTarget;
	ComPtr<IDCompositionVisual> dCompositionVisual;
	ComPtr<IMFAttributes> attributes;
	ComPtr<IMFMediaEngine> mediaEngine;
	virtual void Start() 
	{

		HRESULT result = MFStartup(MF_VERSION);

		ComPtr<IMFMediaEngineClassFactory> mediaEngineFactory;
		ComPtr<IMFMediaEngineNotify > unknown = new MediaEngineNotify();
		ComPtr<IDXGIDevice> dxgiDevice;
		RendererUtility::GetDevice()->QueryInterface(IID_PPV_ARGS(dxgiDevice.GetAddressOf()));
		result = DCompositionCreateDevice(dxgiDevice.Get(), __uuidof(IDCompositionDevice), (void**)&dcompDevice);
		ComPtr<IDCompositionMatrixTransform> sacle;
		dcompDevice->CreateMatrixTransform(&sacle);
		result = dcompDevice->CreateTargetForHwnd(D3D11_GameApp::GetHWND(), false, &dCompositionTarget);
		result = dcompDevice->CreateVisual(&dCompositionVisual);
		result = dCompositionTarget->SetRoot(dCompositionVisual.Get());
		sacle->SetMatrix(D2D1::Matrix3x2F::Scale(1920.0f / 640.0f, 1080.0f / 480.0f));
		dCompositionVisual->SetTransform(sacle.Get());
		//CreateTargetForHwnd�� ���� dCompositionTarget�� ����� 640.0f, 480.0f....
	//	result = dCompositionVisual->SetTransform(D2D1::Matrix3x2F::Scale(1920.0f / 640.0f, 1080.0f / 480.0f));




		result = MFCreateAttributes(&attributes, 0);
		result = attributes->SetUnknown(MF_MEDIA_ENGINE_PLAYBACK_VISUAL, dCompositionVisual.Get());
		//result = attributes->SetUINT64(MF_MEDIA_ENGINE_PLAYBACK_HWND, (UINT64)DG::Core::GetInstance().GetWindowHandle());
		////result = attributes->SetUINT64(MF_MEDIA_ENGINE_OPM_HWND, (UINT64)DG::Core::GetInstance().GetWindowHandle());
		////result = attributes->SetUINT32(MF_MEDIA_ENGINE_VIDEO_OUTPUT_FORMAT, DXGI_FORMAT_B8G8R8A8_UNORM);
		result = attributes->SetUnknown(MF_MEDIA_ENGINE_CALLBACK, unknown.Get());


		result = CoCreateInstance(CLSID_MFMediaEngineClassFactory, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&mediaEngineFactory));
		result = mediaEngineFactory->CreateInstance(MF_MEDIA_ENGINE_REAL_TIME_MODE | MF_MEDIA_ENGINE_DISABLE_LOCAL_PLUGINS, attributes.Get(), &mediaEngine);

		std::wstring wstr = std::wstring(videoPath.begin(), videoPath.end());
		result = mediaEngine->SetSource(wstr.data());
		result = mediaEngine->Load();
		result = mediaEngine->Play();
		isCreditEnd = false;
		th = std::thread([this]()
						 {
							 while (!isCreditEnd)
							 {
								 if (dcompDevice)
								 {
									 dcompDevice->Commit();
								 }
							 }
						 });
		if (BackGroundMusicObject* bgmObject = GameObject::Find<BackGroundMusicObject>(L"BackGroundMusic"))
		{
			BGM = bgmObject->component;
		}
		if (BGM)
		{
			BGM->Stop();
		}
	}
	std::thread th;
	bool isCreditEnd;
	BackGroundMusicComponent* BGM = nullptr;
protected:
	virtual void FixedUpdate() {}
	virtual void Update() 
	{

#ifdef _EDITOR
		if (!Scene::EditorSetting.IsPlay()) return;
#endif 

		isCreditEnd = mediaEngine->GetCurrentTime() >= mediaEngine->GetDuration();

		auto& input = inputManager.input;
		;

		if (input.IsKeyDown(KeyboardKeys::Escape) || isCreditEnd)
		{
			isCreditEnd = true;
			if (nextScenenPath.size())
			{
				std::wstring wstr = std::wstring(nextScenenPath.begin(), nextScenenPath.end());
				TimeSystem::Time.DelayedInvok([wstr]()
											  {
												  sceneManager.LoadScene(wstr.c_str());
											  }, 0.f);			
			}
			else
			{
				if (BGM)
				{
					BGM->Play();
				}
				GameObject::Destroy(gameObject);
			}
		}

	}
	virtual void LateUpdate() {}
	virtual void Render() {}
	virtual void InspectorImguiDraw() 
	{
		ImGui::InputText("Next Scene Path", (char*)nextScenenPath.c_str(), nextScenenPath.size(), ImGuiInputTextFlags_CallbackResize,
						 [](ImGuiInputTextCallbackData* data)
						 {
							 if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
							 {
								 std::string* str = (std::string*)data->UserData;
								 IM_ASSERT(data->Buf == str->c_str());
								 str->resize(data->BufTextLen);
								 data->Buf = str->data();
							 }
							 return 0;
						 }, &nextScenenPath);

		ImGui::InputText("Video Path", (char*)videoPath.c_str(), videoPath.size(), ImGuiInputTextFlags_CallbackResize,
						 [](ImGuiInputTextCallbackData* data)
						 {
							 if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
							 {
								 std::string* str = (std::string*)data->UserData;
								 IM_ASSERT(data->Buf == str->c_str());
								 str->resize(data->BufTextLen);
								 data->Buf = str->data();
							 }
							 return 0;
						 }, & videoPath);




	}

	virtual void Serialized(std::ofstream& ofs) 
	{
		Binary::Write::string(ofs, nextScenenPath);
		Binary::Write::string(ofs, videoPath);
	}
	
	virtual void Deserialized(std::ifstream& ifs) 
	{
		nextScenenPath = Binary::Read::string(ifs);
		videoPath = Binary::Read::string(ifs);

	}
	std::string nextScenenPath;
	std::string videoPath;
};


VideoObject::VideoObject()
{
	AddComponent<VideoComponent>();
	AddComponent<UIRenderComponenet>();
}

void VideoObject::Awake()
{
}
