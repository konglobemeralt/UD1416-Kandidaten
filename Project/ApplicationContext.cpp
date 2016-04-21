#include "ApplicationContext.h"
#include "Classes\AntiAliasing.h"
#include "Classes\Compositing.h"
#include "Classes\DataStructures.h"
#include "Classes\GraphicsManager.h"
#include "Classes\Lightning.h"
#include "Classes\Text.h"
#include "Classes\ToneMapping.h"
#include "Classes\WindowManager.h"

ApplicationContext* ApplicationContext::applicationContext = nullptr;
ApplicationContext::ApplicationContext()
{
	m_user = TEXT;
	m_antiAliasing = new AntiAliasing;
	m_text = new Text;
	m_lightning = new Lightning;
	m_compositing = new Compositing;
	m_toneMapping = new ToneMapping;
	m_graphicsManager = new GraphicsManager;
	m_windowManager = new WindowManager;
}
ApplicationContext::~ApplicationContext()
{
	delete m_antiAliasing;
	m_antiAliasing = nullptr;
	delete m_text;
	m_text = nullptr;
	delete m_lightning;
	m_lightning = nullptr;
	delete m_compositing;
	m_compositing = nullptr;
	delete m_toneMapping;
	m_toneMapping = nullptr;
	delete m_graphicsManager;
	m_graphicsManager = nullptr;
	delete m_windowManager;
	m_windowManager = nullptr;
}
void ApplicationContext::Startup()
{
	if (applicationContext == nullptr)
	{
		applicationContext = new ApplicationContext();

	}
}

void ApplicationContext::Shutdown()
{
	if (applicationContext != nullptr)
	{
		delete applicationContext;
		applicationContext = nullptr;
	}
}

void ApplicationContext::Initialize()
{
	//applicationContext->m_graphicsManager->initGraphics();
	switch (applicationContext->m_user)
	{
	case TEXT:
		applicationContext->m_text->Initialize();
		break;
	case COMPOSITING:
		applicationContext->m_compositing->Initialize();
		break;
	case TONEMAPPING:
		applicationContext->m_toneMapping->Initialize();
		break;
	case ANTIALIASING:
		applicationContext->m_antiAliasing->Initialize();
		break;
	case LIGHTNING:
		applicationContext->m_lightning->Initialize();
		break;
	case ALL:
		applicationContext->m_text->Initialize();
		applicationContext->m_text->Render();
		applicationContext->m_compositing->Initialize();
		applicationContext->m_compositing->SetText(applicationContext->m_text->GetText());
		
		//applicationContext->m_toneMapping->Initialize();
		//applicationContext->m_antiAliasing->Initialize();
		//applicationContext->m_lightning->Initialize();
		break;
	default:
		break;
	}
}