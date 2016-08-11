#include <string>

#include <crtdbg.h>
#include <iostream>

#include "WindowManager.h"
#include "GraphicsManager.h"
#include "DataStructures.h"
#include "..\ApplicationContext.h"
#define PI (3.141592653589793)

#define AppContext ApplicationContext::GetInstance()

using namespace DirectX;
using namespace std;

void setUser() {
	ApplicationContext::GetInstance().SetUser(ALL);
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
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	CoInitialize(NULL);

	// CONSIDER ADDING CONFIG FILE INSTEAD OF USING ARGS
	//the calling application must free the memory used by the argument list when it is no longer needed. To free the memory, use a single call to the LocalFree function.
	int numArgs;
	LPWSTR cmd = GetCommandLineW();
	LPWSTR* argList = CommandLineToArgvW(cmd, &numArgs);
	if (argList == NULL)
		return 0;

	vector<LPWSTR> cmdline;
	for (unsigned int i = 0; i < numArgs; i++) {
		cmdline.push_back(argList[i]);
	}
	LocalFree(argList);

	ApplicationContext::Startup();
	setUser();
	//WindowManager windowManager(hInstance, WndProc);
	ApplicationContext::GetInstance().GetWindowManager()->Initialize(hInstance, WndProc);
	WindowManager* windowManager = ApplicationContext::GetInstance().GetWindowManager();
	ApplicationContext::GetInstance().GetGraphicsManager()->initGraphics(windowManager->getWindowHandle());
	ApplicationContext::Initialize();
	
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
	static float fps = 1;
	static float uptime = 0;
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

			float fps = 1 / ((currentFrame - lastFrame) * 0.001f);
			int seconds = (int)(1500 / fps);
			int hh = (seconds / 60) / 60;
			seconds -= (hh * 60) * 60;
			int mm = seconds / 60;
			seconds -= mm * 60;

			wstring headerMessage;
			headerMessage.append(L"Uptime: " + to_wstring((unsigned int)uptime) + L"   FPS: " + to_wstring(fps) + L"    (Time to render 25 players á 60 frames: " + to_wstring(hh) + L"h " + to_wstring(mm) + L"m " + to_wstring(seconds) + L"s)");
			SetWindowText(*windowManager->getWindowHandle(), headerMessage.c_str());
			
			uptime += ((currentFrame - lastFrame) * 0.001f);
			lastFrame = currentFrame;
			
			AppContext.GetGraphicsManager()->Render();
			AppContext.GetGraphicsManager()->getSwapChain()->Present(0, 0);
		}
	}

	int videoCreated = system("cd FFMPEG\\bin && ffmpeg.exe -i ..\\..\\Results\\image%d.png -y -f avi -c:v huffyuv  ..\\..\\Results\\test.avi");  //image%03d.png
	if (videoCreated != 0)
		system("echo An error has occured! Make sure the FFMPEG folder has not been moved. && pause");

	DestroyWindow(*windowManager->getWindowHandle());
	AppContext.Shutdown();

	return (int) msg.wParam;
}