#include <iostream>
#include "FileManager.hpp"
#include "json.hpp"
#include "Debug.hpp"

// Gathers all the tags used in the data/characteristics JSON files
int main()
{
    Debug::initDebug("logs/gatherer_log.txt");
    std::unordered_map<std::string, int> tag_map;
    for (auto path : FileManager::getFilesInDirectory("data/characteristics"))
    {
        if (path.ends_with(".json"))
        {
            nlohmann::json json_file = FileManager::loadJson(path);
            for (auto &it : json_file.items())
            {
                if (it.value().contains("weaknesses"))
                {
                    for (std::string tag : it.value()["weaknesses"])
                    {
                        tag_map[tag] += 1;
                    }
                }
                if (it.value().contains("strengths"))
                {
                    for (std::string tag : it.value()["strengths"])
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