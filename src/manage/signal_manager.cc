#include "signal_manager.h"

#include <functional>
#include "config_loader.h"
#include "core/event_loop.h"


SignalManager::SignalManager(EventLoop* loop)
  : loop_(loop),
    sigfd_(CreateSigfd()),
    sigfd_channel_(loop, sigfd_)
{
    sigfd_channel_.set_readCallback(std::bind(&SignalManager::HandleRead, this));
    sigfd_channel_.EnableReading();
}

SignalManager::~SignalManager()
{
    sigfd_channel_.DisableAll();
    sigfd_channel_.Remove();
    ::close(sigfd_);
}

int SignalManager::CreateSigfd()
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1); 
    sigaddset(&mask, SIGUSR2); 
   if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)  {
       printf("sigpromask");
   }
    int sigfd = ::signalfd(-1, &mask, 0);
    if (sigfd < 0) {
        printf("fail in sigfd_create.\n");
    }
}

void SignalManager::HandleRead()
{  
    struct signalfd_siginfo siginfo;
    ::read(sigfd_, &siginfo, sizeof siginfo);
    if (siginfo.ssi_signo == SIGUSR1) {
        printf("signal quit.\n");
        QuitHandle();
    } else if (siginfo.ssi_signo == SIGUSR2) {
        printf("signal reload.\n");
        ReloadHandle();
    }
}

void SignalManager::QuitHandle()
{
    loop_->Quit();
    loop_->set_state(EventLoop::kSigQuit);
}

void SignalManager::ReloadHandle()
{
    // judge server:port changed to create new service context.
    auto conf_ptr = ConfigLoader::GetInstance()->config_document_ptr();
    std::string old_server_port_str = (*conf_ptr)["service"]["host"].GetString() 
                                        + std::string(":")
                                        + std::to_string((*conf_ptr)["service"]["port"].GetInt());
    if (!ConfigLoader::GetInstance()->Load("./config.json")) {
        printf("Failed to load configuration file.\n");
        return;
    }
    conf_ptr = ConfigLoader::GetInstance()->config_document_ptr();
    if (!conf_ptr) {
        printf("Invalid configuration file.\n");
        return ;
    }
    std::string new_server_port_str = (*conf_ptr)["service"]["host"].GetString() 
                                        + std::string(":")
                                        + std::to_string((*conf_ptr)["service"]["port"].GetInt());
    if (old_server_port_str != new_server_port_str) {
        loop_->Quit();
        loop_->set_state(EventLoop::kSigReload);
    } else {
        printf("config same.\n");
    }
}