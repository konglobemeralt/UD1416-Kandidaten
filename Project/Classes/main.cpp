#include <string>
#include "WindowManager.h"
#include "GraphicsManager.h"
#include "DataStructures.h"
#include "..\ApplicationContext.h"
#define PI (3.141592653589793)

#define AppContext ApplicationContext::GetInstance()

using namespace DirectX;
using namespace std;

void setUser() {
	ApplicationContext::GetInstance().SetUser(ANTIALIASING);
/*		
		CHOOSE FROM:
		TEXT
		COMPOSITING
		TONEMAPPING
		ANTIALIASING
		LIGHTNING
		ALL
*/
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case VK_TAB:
			return 0;
		default:
			return 0;
		}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		//case WM_MOUSEMOVE:
		//	OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		//	return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	CoInitialize(NULL);
	ApplicationContext::Startup();
	setUser();
	//WindowManager windowManager(hInstance, WndProc);
	ApplicationContext::GetInstance().GetWindowManager()->Initialize(hInstance, WndProc);
	WindowManager* windowManager = ApplicationContext::GetInstance().GetWindowManager();
	ApplicationContext::GetInstance().GetGraphicsManager()->initGraphics(windowManager->getWindowHandle());
	ApplicationContext::Initialize();
	
	
	//GraphicsManager::getInstance().initGraphics(windowManager.getWindowHandle());

	MSG msg = { 0 };

	D3D11_VIEWPORT vp;
	vp.Width = (float)AppContext.GetGraphicsManager()->getWindowWidth();
	vp.Height = (float)AppContext.GetGraphicsManager()->getWindowHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	AppContext.GetGraphicsManager()->getDeviceContext()->RSSetViewports(1, &vp);

	ShowWindow(*windowManager->getWindowHandle(), nCmdShow);
	static int fps = 0;
	static int uptime = 1;
	DWORD currentFrame = timeGetTime();
	static DWORD lastFrame = currentFrame;

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			currentFrame = timeGetTime();

			if (currentFrame - lastFrame >= 1000) {
				wstring headerMessage;
				headerMessage.append(L"Uptime: " + to_wstring(uptime) + L"   FPS: " + to_wstring(fps));
				SetWindowText(*windowManager->getWindowHandle(), headerMessage.c_str());

				lastFrame = currentFrame;
				uptime++;
				fps = 0;
			}
			else
				fps++;

			AppContext.GetGraphicsManager()->Render();
			AppContext.GetGraphicsManager()->getSwapChain()->Present(0, 0);
		}
	}

	DestroyWindow(*windowManager->getWindowHandle());

	return (int) msg.wParam;
}