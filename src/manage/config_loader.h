#ifndef SABER_CONFIGLOADER_HH_
#define SABER_CONFIGLOADER_HH_

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"  
#include "rapidjson/writer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/stringbuffer.h"

#include <fstream>
#include <memory>
#include <string>

class ConfigLoader
{
public:
    static ConfigLoader* GetInstance()
    {
        if (config_loader_ptr_ == NULL) {
            config_loader_ptr_ = new ConfigLoader();
        }
        return config_loader_ptr_;
    }

    bool Load(const std::string& conf_path);
    
    std::shared_ptr<rapidjson::Document> config_document_ptr()
    { return config_document_ptr_; }


private:
    ConfigLoader() { }
    virtual ~ConfigLoader() = default;
private:
    static ConfigLoader* config_loader_ptr_;
    std::shared_ptr<rapidjson::Document> config_document_ptr_;
};

#endif
