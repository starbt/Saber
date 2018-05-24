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
#include <mutex>
#include <atomic>

class ConfigLoader
{
public:
    static ConfigLoader* GetInstance()
    { 
       if (config_loader_ptr_ == NULL) {
            std::lock_guard<std::mutex> lock(mutex_);
            if (config_loader_ptr_ == NULL) {
                config_loader_ptr_ = new ConfigLoader();
            }
       }
       return config_loader_ptr_;
    }

    bool Load(const std::string& conf_path);

    //get the config ptr 
    std::shared_ptr<rapidjson::Document> config_document_ptr()
    { 
        auto index = cur_index_.load(std::memory_order_relaxed);
        return config_document_ptr_[index];
    }

    void UpdateConfigPtr(std::shared_ptr<rapidjson::Document>  ptr)
    {
        int cur_index = cur_index_.load(std::memory_order_relaxed);
        int next_index = (cur_index + 1) % 2;
        config_document_ptr_[next_index] = ptr;
        cur_index_.store(next_index, std::memory_order_relaxed);
    }

private:
    ConfigLoader() 
     : cur_index_(0)
    {
        config_document_ptr_[0] = NULL;
        config_document_ptr_[1] = NULL;
    } 
    virtual ~ConfigLoader() = default;

private:
    static ConfigLoader* config_loader_ptr_;
    std::shared_ptr<rapidjson::Document> config_document_ptr_[2];
    std::atomic<int> cur_index_;
    static std::mutex mutex_;
};

#endif
