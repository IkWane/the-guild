#include <iostream>
#include "FileManager.hpp"
#include "json.hpp"
#include <map>

int main()
{
    std::unordered_map<std::string, int> tag_map;
    for (auto path : FileManager::getFilesInDirectory("data/characteristics"))
    {
        if (path.ends_with(".json"))
        {
            nlohmann::json json_file = FileManager::loadJson(path);
            for (nlohmann::json json : json_file)
            {
                if (json.contains("weaknesses"))
                {
                    for (std::string tag : json["weaknesses"])
                    {
                        tag_map[tag] += 1;
                    }
                }
                if (json.contains("strengths"))
                {
                    for (std::string tag : json["strengths"])
                    {
                        tag_map[tag] += 1;
                    }
                }
            }
        }
        
    }
    
    std::unordered_map<std::string, int>::iterator it = tag_map.begin();
    while (it != tag_map.end())
    {
        std::cout << it->first << " : " << it->second << std::endl;
        ++it;
    }
    
    return 0;
}