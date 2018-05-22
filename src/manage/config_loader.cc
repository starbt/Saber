#include "config_loader.h"
#include <string>

#include <iostream>

ConfigLoader* ConfigLoader::config_loader_ptr_ = NULL;

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
    config_document_ptr_ = std::make_shared<rapidjson::Document>();
    config_document_ptr_->Parse<0>(str_file.c_str());  
    //rapidjson::Value &node1 = (*config_document_ptr_)["author"];  
    //std::cout << "author: " << node1.GetString() << std::endl;  

    return true;
}