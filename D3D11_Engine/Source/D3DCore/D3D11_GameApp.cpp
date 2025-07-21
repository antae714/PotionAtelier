#include "D3D11_GameApp.h"
#include <Thread/ThreadPool.h>
#include <shellapi.h>
#include <framework.h>
#include <NodeEditor/NodeEditor.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK ImGUIWndProcDefault(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	switch (message)
	{
	case WM_CREATE:
	{
		HCURSOR h = LoadCursorFromFile(L"Resource/Cus/UI_Cursor.cur");
		if(h)
			SetClassLongPtr(hWnd, GCLP_HCURSOR, (LONG_PTR)h);
		
		break;
	}

	case WM_SETCURSOR:
	{
		HCURSOR h = (HCURSOR)GetClassLongPtr(hWnd, GCLP_HCURSOR);
		if (h)
		{
			SetCursor(h);
			return TRUE; 
		}
		break;
	}

#pragma region DXTKInputSystem ���� ����
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			
		}
		Mouse::ProcessMessage(message, wParam, lParam);
		break;
	case WM_ACTIVATEAPP:
		Mouse::ProcessMessage(message, wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
		D3D11_GameApp::ProcessMouse(message, wParam, lParam);
		break;
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_MOUSEHOVER:
	case WM_MOUSEWHEEL:
		Mouse::ProcessMessage(message, wParam, lParam);
		break;
	case WM_SYSKEYDOWN:
		if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000) // Alt + Enter �Է½�
		{
			
		}
		D3D11_GameApp::ProcessKeyboard(message, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		D3D11_GameApp::ProcessKeyboard(message, wParam, lParam);
		break;
	case WM_MENUCHAR:
		// A menu is active and the user presses a key that does not correspond
		// to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
		return MAKELRESULT(0, MNC_CLOSE);
	case WM_MOUSEACTIVATE:
		// When you click to activate the window, we want Mouse to ignore that event.
		return MA_ACTIVATEANDEAT;
#pragma endregion 

	case WM_STYLECHANGED:
	{
		break;
	}
	case WM_DESTROY:
		D3D11_GameApp::GameEnd();
		break;
	case WM_EXITSIZEMOVE: 
	{
		break;
	}
	case WM_SYSCOMMAND:
		if (wParam == SC_CLOSE)
		{
			//D3D11_GameApp::GameEnd();
		}
		break;
	case WM_DROPFILES:
	{
		if (D3D11_GameApp* app = D3D11_GameApp::RunApp)
		{
			HDROP hDrop = (HDROP)wParam;
			UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
			wchar_t filePath[MAX_PATH]{};
			std::filesystem::path path;
			for (UINT i = 0; i < fileCount; i++) 
			{
				DragQueryFile(hDrop, i, filePath, MAX_PATH);
				path = filePath;
				auto find = app->fileEventHandlers.find(path.extension());
				if (find != app->fileEventHandlers.end())
				{
					find->second(filePath);
				}
			}
			DragFinish(hDrop);
		}
		break;
	}
	default:
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void D3D11_GameApp::SetDefaultFileHandlers()
{
	if (D3D11_GameApp* app = D3D11_GameApp::RunApp)
	{
		app->SetFileHandlers(L".fbx", &D3D11_GameApp::DefaultFBXDragDropHandler);
		app->SetFileHandlers(L".Proj", &D3D11_GameApp::DefaultNodeProjDragDropHandler);
	}
}

void D3D11_GameApp::SetFileHandlers(const wchar_t* extention, const FileHandler& EventFunc)
{
	if (D3D11_GameApp* app = D3D11_GameApp::RunApp)
	{
		app->fileEventHandlers[extention] = EventFunc;
	}
}

void D3D11_GameApp::RemoveFileHandlers(const wchar_t* extention)
{
	if (D3D11_GameApp* app = D3D11_GameApp::RunApp)
	{
		app->fileEventHandlers.erase(extention);
	}
}

void D3D11_GameApp::Present()
{
	RunApp->IDXGI.pSwapChain->Present(0, 0);
}

void D3D11_GameApp::GameEnd()
{
	sceneManager.EndGame = true;
#ifdef _EDITOR
	if(Scene::GuizmoSetting.UseImGuizmo)
	{
		if (WinUtility::ShowConfirmationDialog(L"�������� ���� ������ �����˴ϴ�", L"���� �����Ͻðڽ��ϱ�?"))
		{
			std::wstring savePath = sceneManager.GetLastLoadScenePath();
			if (savePath.empty())
			{
				savePath = WinUtility::GetSaveAsFilePath(L"Scene", sceneManager.currScene->GetSceneName());
			}
			if (!savePath.empty())
			{
				sceneManager.SaveScene(savePath.c_str(), true);
			}		
		}	
	}
#endif
}

void D3D11_GameApp::ChangeResolution(SIZE resize)
{
	if (D3D11_GameApp* app = static_cast<D3D11_GameApp*>(RunApp))
	{
		
	}
}

D3D11_GameApp::D3D11_GameApp()
{
	this->customWndProc = ImGUIWndProcDefault;
}

D3D11_GameApp::~D3D11_GameApp()
{

}

void D3D11_GameApp::Initialize(HINSTANCE hinstance)
{
	WinGameApp::Initialize(hinstance);
	inputManager.Initialize();
	InitMainRenderer();
	IDXGI.Init();
	RendererUtility::SetSwapChain(IDXGI.pSwapChain);

	ID3D11Device* pDevice = RendererUtility::GetDevice();
	ComPtr<ID3D11DeviceContext> pDeviceContext;
	pDevice->GetImmediateContext(&pDeviceContext);
	WinGameApp::InitImGUI(pDevice, pDeviceContext.Get());

	threadPool.Initialize();
}

void D3D11_GameApp::Uninitialize()
{
	WinGameApp::UninitImGUI();
	RendererUtility::SetSwapChain(nullptr);
	IDXGI.Uninit();
	UninitMainRenderer();
	inputManager.Uninitialize();
	WinGameApp::Uninitialize();
	threadPool.Destroy();

#ifdef  _EDITOR
	Scene::EditorSetting.editorCamera.reset();
#endif //  _EDITOR
}

void D3D11_GameApp::Awake()
{
	D3D11_GameApp::RunApp = this;
	SetDefaultFileHandlers();
	
	gameObjectFactory.InitializeMemoryPool();

	if (sceneManager.nextScene == nullptr)
		sceneManager.LoadScene<Scene>(); //�� �� �ε�

	sceneManager.ChangeScene();
	sceneManager.AddObjects();
}

void D3D11_GameApp::Update()
{
	inputManager.UpdateProcesser();

	fixedElapsedTime += TimeSystem::Time.GetDeltaTime(false);
	while (fixedElapsedTime >= TimeSystem::Time.GetFixedDelta())
	{		
		sceneManager.FixedUpdateScene();
		fixedElapsedTime -= TimeSystem::Time.GetFixedDelta();
	}

	sceneManager.UpdateScene();
	sceneManager.LateUpdateScene();
}

void D3D11_GameApp::Render()
{
	sceneManager.RenderScene(); //�� ������ �Լ�.
	EventManager::Update();
	TimeSystem::Time.UpdateDelayedInvoker();
	sceneManager.AddObjects();  //������Ʈ ����
	sceneManager.EraseObjects(); //������Ʈ ����.
	sceneManager.ChangeScene(); //���� �� ������ ��ȯ 
}

void D3D11_GameApp::ProcessMouse(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(RunApp && wParam != 0)
		RunApp->PreProcessMouseMessage(message, static_cast<MouseVK>(wParam));

	Mouse::ProcessMessage(message, wParam, lParam);
}

void D3D11_GameApp::ProcessKeyboard(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (RunApp && wParam != 0)
		RunApp->PreProcessKeyboradMessage(message, static_cast<KeyboardVK>(wParam));
}

void D3D11_GameApp::DefaultFBXDragDropHandler(const wchar_t* filePath)
{
	bool isStatic = WinUtility::ShowConfirmationDialog(L"Ȯ��", L"���� �޽��ΰ���?");
	Utility::LoadFBX(filePath, isStatic, SURFACE_TYPE::PBR);
}

void D3D11_GameApp::DefaultNodeProjDragDropHandler(const wchar_t* filePath)
{
	if (Scene* scene = sceneManager.currScene.get())
	{
		std::filesystem::path editorPath = filePath;
		if (editorPath.is_absolute())
		{
			editorPath = std::filesystem::relative(editorPath, std::filesystem::current_path());
		}
		ShaderNodeEditor* editor = scene->MakeShaderNodeEditor(editorPath.c_str());
		editor->EndPopupEvent = [scene, editorPath]()
			{
				scene->EraseShaderNodeEditor(editorPath.c_str());
			};
	}
}

void D3D11_GameApp::SetBorderlessWindowed()
{
	windowStyleEX = WS_POPUP;
}

void D3D11_GameApp::SetOptimalScreenSize()
{
	clientSize = { 0, 0 };
}

void D3D11_GameApp::InitMainRenderer()
{
	MainRenderer = std::make_unique<DefferdRenderer>();
	MainRenderer->directLight.PushDirectLight("Light 0", DirectionLightData
		{
			.Color = {1, 1, 1, 1},
			.Directoin = { 0, -1, 1, 1 }
		});

	{
		ComPtr<ID3D11ComputeShader> computeShader = nullptr;
		hlslManager.CreateSharingShader(L"Resource/EngineShader/DeferredRender.hlsl", &computeShader);
		MainRenderer->deferredCS.LoadShader(computeShader.Get());
	}
	{
		ComPtr<ID3D11PixelShader> pixelShader = nullptr;
		hlslManager.CreateSharingShader(L"Resource/EngineShader/CopyTexture.hlsl", &pixelShader);
		MainRenderer->copyTexturePS.LoadShader(pixelShader.Get());
	}
	{
		ComPtr<ID3D11VertexShader> vertexShader = nullptr;
		ComPtr<ID3D11InputLayout> inputLayout = nullptr;
		hlslManager.CreateSharingShader(L"Resource/EngineShader/FullScreenTriangle.hlsl", &vertexShader, &inputLayout);
		MainRenderer->fullScrennShader.LoadShader(vertexShader.Get(), inputLayout.Get());
	}
	{
		ComPtr<ID3D11PixelShader> pixelShader = nullptr;
		hlslManager.CreateSharingShader(L"Resource/EngineShader/UIPixelShader.hlsl", &pixelShader);
		MainRenderer->uiPixelShader.LoadShader(pixelShader.Get());
	}
	{
		ComPtr<ID3D11VertexShader> vertexShader = nullptr;
		ComPtr<ID3D11InputLayout> inputLayout = nullptr;
		hlslManager.CreateSharingShader(L"Resource/EngineShader/UIVertexShader.hlsl", &vertexShader, &inputLayout);
		MainRenderer->uiVertexShader.LoadShader(vertexShader.Get(), inputLayout.Get());
	}
	{
		ComPtr<ID3D11VertexShader> vertexShader = nullptr;
		ComPtr<ID3D11InputLayout> inputLayout = nullptr;
		hlslManager.CreateSharingShader(L"Resource/EngineShader/VS_Particle.hlsl", &vertexShader, &inputLayout);
		MainRenderer->particleVertexShader.LoadShader(vertexShader.Get(), inputLayout.Get());
	}
	{
		ComPtr<ID3D11GeometryShader> geoMetryShader = nullptr;
		hlslManager.CreateSharingShader(L"Resource/EngineShader/GS_CreateBillboardQuad.hlsl", &geoMetryShader);
		MainRenderer->particleGeometryShader.LoadShader(geoMetryShader.Get());
	}
	{
		ComPtr<ID3D11ComputeShader> geoMetryShader = nullptr;
		hlslManager.CreateSharingShader(L"Resource/EngineShader/CS_Particle.hlsl", &geoMetryShader);
		MainRenderer->particleComputeShader.LoadShader(geoMetryShader.Get());
	}
}

void D3D11_GameApp::UninitMainRenderer()
{
	MainRenderer.reset();
}

void D3D11_GameApp::DXGI::Init()
{
	using namespace Utility;

	HRESULT result;

	//dxgi ����̽�
	result = CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&pDXGIFactory);
	Check(result);

	//��� �׷��� ī�� �� ����� ����. 
	size_t i = 0;
	IDXGIAdapter* pAdapter = nullptr;
	while (pDXGIFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND) //�׷��� ī�� ����)
	{
		result = pAdapter->QueryInterface(__uuidof(IDXGIAdapter3), reinterpret_cast<void**>(&pAdapter));
		Check(result);
		DXGIAdapters.push_back(reinterpret_cast<IDXGIAdapter3*>(pAdapter));
		DXGIOutputs.resize(i + 1);
		size_t j = 0;
		IDXGIOutput* pOutput = nullptr;
		while (pAdapter->EnumOutputs(j, &pOutput) != DXGI_ERROR_NOT_FOUND)
		{
			result = (pOutput->QueryInterface(__uuidof(IDXGIOutput1), reinterpret_cast<void**>(&pOutput)));
			Check(result);
			DXGIOutputs[i].push_back(reinterpret_cast<IDXGIOutput1*>(pOutput));
			++j;
		}
		++i;
	}

	DXGI_ADAPTER_DESC adapterDesc;
	for (auto& Adapters : DXGIAdapters)
	{
		Adapters->GetDesc(&adapterDesc);
		Debug_wprintf(L"%lu : %s\n", adapterDesc.DeviceId, adapterDesc.Description); //��ī �̸�
	}

	DXGI_OUTPUT_DESC outputDesc;
	DXGIOutputs[0][0]->GetDesc(&outputDesc); //���� �����
	if (outputDesc.AttachedToDesktop)
	{
		Debug_wprintf(L"Moniter 1 : %s\n", outputDesc.DeviceName);
	}

	CreateSwapChain();

	ComPtr<ID3D11Texture2D> backBufferTexture;
	result = pSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBufferTexture));
	Check(result);

	backBuffer = std::make_unique<Texture>();
	backBuffer->LoadTexture(backBufferTexture.Get(), ETextureUsage::RTV);

	MainRenderer->SetRenderTarget(*backBuffer);
}

void D3D11_GameApp::DXGI::Uninit()
{
	using namespace Utility;

	backBuffer.reset();
	SafeRelease(pSwapChain);
	for (auto& i : DXGIOutputs)
	{
		SafeReleaseArray(i);
	}
	SafeRelease(pDXGIFactory);
}

void D3D11_GameApp::DXGI::ClearBackBuffer(const float(&color)[4]) const
{
	backBuffer->ClearTexture(ETextureUsage::RTV, (float*)color);
}

void D3D11_GameApp::DXGI::CreateSwapChain()
{
	using namespace Utility;
	ID3D11Device* pDevice = RendererUtility::GetDevice();
	HRESULT result;

	if (pSwapChain)
		return;

	DXGI_SWAP_CHAIN_DESC1 swapDesc{}; //����ü�� �Ӽ� ����ü  
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc{};
	swapDesc.BufferCount = 2; //���� ����
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //���� ��� ��� ����
	swapDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //��� ���� ����.
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; //�ø� ��� ���.   
	swapDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; //��ü ȭ�� ��ȯ�� �ػ� �� ����� ���� �ڵ� ���� �÷���
	swapDesc.BufferCount = 1; //���� ����
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; //�ø� ��� ���.   

	//���� ������ ����
	SIZE clientSize = WinGameApp::GetClientSize();
	swapDesc.Width = clientSize.cx;
	swapDesc.Height = clientSize.cy;

	//���ø� ���� *(MSAA)
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	//0/0�� �ڵ� ������. �׸��� â���� ������ ���� �ȵ�...
	fullScreenDesc.Windowed = true; //â��� ����
	fullScreenDesc.RefreshRate.Numerator = 0;
	fullScreenDesc.RefreshRate.Denominator = 0;

	UINT creationFlags = 0;
#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	result = pDXGIFactory->CreateSwapChainForHwnd(
		pDevice, WinGameApp::GetHWND(), &swapDesc, &fullScreenDesc, nullptr, &pSwapChain);
	Check(result);
}

