#include "config_loader.h"
#include <string>

#include <iostream>

ConfigLoader* ConfigLoader::config_loader_ptr_ = NULL;

std::mutex ConfigLoader::mutex_;

bool ConfigLoader::Load(const std::string& conf_path)
{
    std::ifstream fin(conf_path);  
    if (!fin.is_open()) {
        printf("open file error.\n");
        return false;
    }
    std::string temp;  
    std::string str_file = "";  
    while(getline(fin, temp))     
    {  
        str_file = str_file + temp + std::string("\n");  
    }  

    //parse to json 
    auto config_ptr = std::make_shared<rapidjson::Document>();
    config_ptr->Parse<0>(str_file.c_str());  
    UpdateConfigPtr(config_ptr);

    return true;
}