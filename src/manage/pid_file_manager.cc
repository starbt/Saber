#include "pid_file_manager.h"
#include "config_loader.h"
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <string>
#include <string.h>

const std::string PidFileManager::kRunPath = "/home/xcv/pidfile";

PidFileManager::PidFileManager(const std::string& name, const std::string& version)
  : pid_(std::to_string(static_cast<int>(getpid()))),
    name_(name),
    version_(version)
{
    MakePidFile();
}

PidFileManager::~PidFileManager()
{
    RemovePidFile();
}

void PidFileManager::MakePidFile()
{
    if (access(kRunPath.c_str(), F_OK) != 0) {
        if (mkdir(kRunPath.c_str(), 0755) != 0 ) {
            printf("make dir faild.\n");
        }
    }
    std::string filename = name_ + std::string("_") + version_ + std::string("_") +  std::string(".") + pid_;
    std::string absolute_filename = kRunPath + std::string("/") + filename;
    std::ofstream out_file(absolute_filename, std::fstream::out);
    if (!out_file) {
        printf("open pid file faild.\n");
        return;
    }
    out_file << pid_;
    out_file.close();
}

void PidFileManager::RemovePidFile()
{
    std::string filename = name_ + std::string("_") + version_ + std::string("_")  + std::string(".") + pid_;
    std::string absolute_filename = kRunPath + std::string("/") + filename;
    if (unlink(absolute_filename.c_str()) != 0) {
        printf("faild to delete pidfile.\n");
    }
}

