#include "WindowManager.h"
#include "GraphicsManager.h"
#include "..\ApplicationContext.h"
WindowManager::WindowManager(HINSTANCE hInstance, WNDPROC WndProc) {
	m_hInstance = hInstance;
	WNDCLASSEX wcex = { 0 };
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = L"BTH_D3D_DEMO";

	if (!RegisterClassEx(&wcex))
		exit(-1);

	RECT rc = { 0, 0, (LONG)ApplicationContext::GetInstance().GetGraphicsManager()->getWindowWidth(), (LONG)ApplicationContext::GetInstance().GetGraphicsManager()->getWindowHeight() };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	m_hwnd = CreateWindow(
		L"BTH_D3D_DEMO",
		L"BTH Direct3D Demo",
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);
}

void WindowManager::Initialize(HINSTANCE hInstance, WNDPROC WndProc)
{
	m_hInstance = hInstance;
	WNDCLASSEX wcex = { 0 };
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = L"BTH_D3D_DEMO";

	if (!RegisterClassEx(&wcex))
		exit(-1);

	RECT rc = { 0, 0, (LONG)ApplicationContext::GetInstance().GetGraphicsManager()->getWindowWidth(), (LONG)ApplicationContext::GetInstance().GetGraphicsManager()->getWindowHeight() };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	m_hwnd = CreateWindow(
		L"BTH_D3D_DEMO",
		L"BTH Direct3D Demo",
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);
}

HINSTANCE* WindowManager::getHinstance()
{
	return &m_hInstance;
}
WindowManager::WindowManager()
{

}

WindowManager::~WindowManager() {
}

HWND* WindowManager::getWindowHandle() {
	return &m_hwnd;
}