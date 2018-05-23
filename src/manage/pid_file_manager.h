#ifndef _PIDFILE_MANAGER_H_
#define _PIDFILE_MANAGER_H_

#include <memory>
#include <string>

class PidFileManager
{
public:
    PidFileManager(const std::string& name, const std::string& version);
    ~PidFileManager();


private:
    void MakePidFile();
    void RemovePidFile();

private:
    const static std::string kRunPath;
    std::string pid_;
    std::string name_;
    std::string version_;
};

#endif
