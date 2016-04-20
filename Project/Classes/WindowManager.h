#ifndef H_WINDOWMANAGER
#define H_WINDOWMANAGER

#include <d3d11.h>

class WindowManager {
private:
	HWND m_hwnd;

public:
	WindowManager(HINSTANCE hInstance, WNDPROC WndProc);
	~WindowManager();

	HWND* getWindowHandle();
};

#endif