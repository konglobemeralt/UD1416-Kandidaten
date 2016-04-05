#ifndef H_WINDOWMANAGER
#define H_WINDOWMANAGER

#include <d3d11.h>

class WindowManager {
private:
	HWND m_hwnd;

	UINT windowWidth = 1280;
	UINT windowHeight = 1024;

public:
	WindowManager(HINSTANCE hInstance, WNDPROC WndProc);
	~WindowManager();

	UINT getWindowWidth();
	UINT getWindowHeight();

	HWND* getWindowHandle();
};

#endif