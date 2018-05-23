#include <iostream>
#include "http/http_server.h"
#include "http/http_response.h"
#include "http/http_request.h"
#include "core/event_loop.h"
#include <list>

#include <signal.h>
#include <unistd.h>
#include <memory>

#include "manage/config_loader.h"
#include "manage/pid_file_manager.h"

struct ServiceContext {
    EventLoop                                loop;
    HttpServer                               server;
    PidFileManager                  file_manager_ptr;
    ServiceContext(const std::string& ip, uint32_t port, const std::string& name, const std::string& version)
      : loop(),
        server(&loop, ip, port),
        file_manager_ptr(name, version)
    {}
};

using ServiceContextPtr = std::shared_ptr<ServiceContext>;

static ServiceContextPtr create_service_context(const std::shared_ptr<rapidjson::Document>& config_ptr)
{
    std::string ip = (*config_ptr)["service"]["host"].GetString();
    uint32_t port = (*config_ptr)["service"]["port"].GetInt();
    std::string name = (*config_ptr)["service"]["name"].GetString();
    std::string version = (*config_ptr)["service"]["version"].GetString();
    auto ret_ptr = std::make_shared<ServiceContext>(ip, port, name, version);
    if (!ret_ptr) {
        return nullptr;
    }
    ret_ptr->server.Start();
    ret_ptr->loop.Loop();

    return ret_ptr;
}

int main()
{
    ServiceContextPtr svc_ptr = nullptr;

    if (!ConfigLoader::GetInstance()->Load("./config.json")) {
        printf("Failed to load configuration file.\n");
        return -1;
    }
    auto conf_ptr = ConfigLoader::GetInstance()->config_document_ptr();
    if (conf_ptr == NULL) {
        printf("Invalid configuration file.\n");
        return -1;
    }
    svc_ptr = create_service_context(conf_ptr);

    if (svc_ptr->loop.state() == EventLoop::kSigQuit) {
        return 0;
    }

    while (true) {
        conf_ptr = ConfigLoader::GetInstance()->config_document_ptr();
        if (conf_ptr == NULL) {
            printf("Invalid configuration file.\n");
            return -1;
        }
        svc_ptr.reset();
        svc_ptr = create_service_context(conf_ptr);
        if (svc_ptr->loop.state() == EventLoop::kSigQuit) {
            break;
        } else if (svc_ptr->loop.state() == EventLoop::kSigReload){
            continue;
        }
        usleep(1000000L);
    }

    return 0;
}
