#pragma once
#ifndef APPLICATIONCONTEXT_H
#define APPLICATIONCONTEXT_H


class Text;
class Lightning;
class Compositing;
class AntiAliasing;
class ToneMapping;
class GraphicsManager;
class WindowManager;
class InfoRetriever;
class ApplicationContext
{
public:
	static void Startup();
	static void Shutdown();
	static void Initialize();

	static ApplicationContext& GetInstance() { return *applicationContext; }
	Text* GetTextObject() const { return m_text; }
	Lightning* GetLightningObject() const { return m_lightning; }
	Compositing* GetCompositingObject() const { return m_compositing; }
	AntiAliasing* GetAntiAliasingObject() const { return m_antiAliasing; }
	ToneMapping* GetToneMappingObject() const { return m_toneMapping; }
	GraphicsManager* GetGraphicsManager() const { return m_graphicsManager; }
	WindowManager* GetWindowManager() const { return m_windowManager; }
	void SetUser(int p_user) { m_user = p_user; }
	int GetUser() { return m_user; }
	

private:
	static ApplicationContext* applicationContext;
	Text* m_text = nullptr;
	Lightning* m_lightning = nullptr;
	Compositing* m_compositing = nullptr;
	AntiAliasing* m_antiAliasing = nullptr;
	ToneMapping* m_toneMapping = nullptr;
	GraphicsManager* m_graphicsManager = nullptr;
	WindowManager* m_windowManager = nullptr;
    InfoRetriever* m_InfoRetriever = nullptr;
	int m_user;


	ApplicationContext();
	~ApplicationContext();
	ApplicationContext(ApplicationContext const&) = delete;
	void operator=(ApplicationContext const&) = delete;
};



#endif