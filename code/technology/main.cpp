#include <RadonFramework/precompiled.hpp>
#include <RadonFramework/Radon.hpp>
#include "RadonFramework/Diagnostics/Appender.hpp"
#include "RadonFramework/IO/LogConsole.hpp"
#include "RadonFramework/IO/Log.hpp"
#include "RadonFramework/Drawing/Forms/WindowServiceLocator.hpp"
#include "RadonFramework/Drawing/Forms/IApplication.hpp"
#include "RadonFramework/Threading/ThreadPool.hpp"
#include "RFECS/Collector.hpp"

#include "MainWindow.hpp"
#include "Allocator.hpp"
#include "Components/Identifiers.hpp"
#include "ComponentManagers/Renderer.hpp"

namespace Technology {

class GameLoop: public RF_Pattern::SignalReceiver
{
public:
    void Start()
    {
        RF_Mem::AutoPointer<RF_IO::Directory> dir = RF_IO::Directory::ApplicationDirectory();

        m_Allocator = RF_Mem::AutoPointer<RFECS::Allocator>(new Technology::Allocator());
        m_Collector.Setup(*m_Allocator);

        RF_Mem::AutoPointer<RFECS::BaseComponentManager> renderBehaviourManager(new ComponentManager::Renderer());
        auto* renderer = static_cast<ComponentManager::Renderer*>(renderBehaviourManager.Get());

        RF_Mem::AutoPointer<MainWindow> window(new MainWindow());
        window->Initialize(renderer);
        m_Collector.RegisterComponentManager(renderBehaviourManager);

        window->OnIdle += Connector<>(&GameLoop::Run);
        RF_Form::WindowServiceLocator::Default().Application()->Run(window.Get());
    }

    void Run()
    {
        m_Collector.Process();
    }

    RF_Mem::AutoPointer<RFECS::Allocator> m_Allocator;
    RFECS::Collector m_Collector;
};

}

void main()
{ 
    RadonFramework::Radon framework;
    RF_Mem::AutoPointer<RF_Diag::Appender> console(new RF_IO::LogConsole);
    RF_IO::Log::AddAppender(console);

    if(RF_Form::WindowServiceLocator::Default().Application()->IsRunningOnDesktop())
        RF_Form::WindowServiceLocator::Default().Application()->ShowConsole(false);

    Technology::GameLoop gameloop;
    gameloop.Start();

    RF_Pattern::Singleton<RF_Thread::ThreadPool>::GetInstance().WaitTillDoneWithInactiveQueue();
    return;
}