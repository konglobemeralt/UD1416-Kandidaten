#ifndef H_WINDOWMANAGER
#define H_WINDOWMANAGER

#include <d3d11.h>
class GraphicsHandler;
class WindowManager {
private:
	HWND m_hwnd;
	HINSTANCE m_hInstance;

public:
	WindowManager(HINSTANCE hInstance, WNDPROC WndProc);
	WindowManager();
	~WindowManager();
	void Initialize(HINSTANCE hInstance, WNDPROC WndProc);
	HWND* getWindowHandle();
	HINSTANCE* getHinstance();
};

#endif