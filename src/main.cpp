#include <iostream>
#include <string>
#include <conio.h>
#include <math.h>
#include "gameUtil.hpp"
#include "FileManager.hpp"
#include "TheGuild.hpp"


int main()
{
    RNG::get(0);

    printf("Welcome to The Guild !\n");

    nlohmann::json dialogs = FileManager::loadJson("dialogs.json");
    
    gameUtil::giveDialog(dialogs["Diag 1"]);
    
    
    TheGuild guild = TheGuild("savefile.json");
    std::cout << "Current Guild gold: " << guild.gold << std::endl;
    guild.gold += gameUtil::numberDialog("How much do you want to deposit to your guild's gold?");
    std::cout << "Guild gold: " << guild.gold << std::endl;
    guild.saveGuild("savefile.json");

    return 0;
}