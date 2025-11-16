#include "magicNumbers.hpp"
#include "json.hpp"
#include <RNG.hpp>

namespace gameUtil
{
    int wrapAround(int a, int n)
    {
        if (a >= n)
            return 0;
        else if (a < 0)
            return n-1;
        return a;  
    }

    int chooseOption(int optionsLen, std::string options[], int defaultChoice = 0)
    {
        int previous_selected = -1;
        int selected = defaultChoice;
        while (true)
        {
            if (previous_selected != selected)
            {
                previous_selected = selected;
                std::cout << "\r";
                for (int i = 0; i < optionsLen; i++)
                {
                    if (selected == i)
                    {
                        std::cout << " > " << options[i] << " < ";
                    }
                    else
                    {
                        std::cout << "   " << options[i] << "   ";
                    }
                }
                std::cout << std::flush;
            }

            int key = getch();
            if (key == 224)
            {
                key = getch();
                switch (key)
                {
                    case KEY_LEFT:
                        selected -= 1;
                        selected = wrapAround(selected, optionsLen);
                        break;
                    case KEY_RIGHT:
                        selected += 1;
                        selected = wrapAround(selected, optionsLen);
                        break;
                    default:
                        break;
                }
            } 
            else if (key == KEY_ENTER)
            {
                break;
            }
            else if (key == KEY_ESCAPE)
            {
                selected = -1;
                break;
            }
        }

        std::cout << "\r";
        for (int i = 0; i < optionsLen; i++)
        {
            std::cout << "   ";
            for (int j = 0; j < options[i].length(); j++)
            {
                std::cout << " ";
            }
            std::cout << "   ";
        }
        std::cout << "\r" << std::flush;
        
        return selected;
    }

    int chooseOption(nlohmann::json options, int defaultChoice = 0)
    {
        int previous_selected = -1;
        int selected = defaultChoice;
        
        while (true)
        {
            if (previous_selected != selected)
            {
                previous_selected = selected;
                std::cout << "\r";
                for (int i = 0; i < options.size(); i++)
                {
                    if (selected == i)
                    {
                        std::cout << " > " << options.at(i).get<std::string>() << " < ";
                    }
                    else
                    {
                        std::cout << "   " << options.at(i).get<std::string>() << "   ";
                    }
                }
                std::cout << std::flush;
            }

            int key = getch();
            if (key == 224)
            {
                key = getch();
                switch (key)
                {
                    case KEY_LEFT:
                        selected -= 1;
                        selected = wrapAround(selected, options.size());
                        break;
                    case KEY_RIGHT:
                        selected += 1;
                        selected = wrapAround(selected, options.size());
                        break;
                    default:
                        break;
                }
            } 
            else if (key == KEY_ENTER)
            {
                break;
            }
            else if (key == KEY_ESCAPE)
            {
                selected = -1;
                break;
            }
        }

        std::cout << "\r";
        for (auto& param : options.items())
        {
            std::cout << "   ";
            std::string e = param.value().get<std::string>();
            for (int j = 0; j < e.length(); j++)
            {
                std::cout << " ";
            }
            std::cout << "   ";
        }
        std::cout << "\r" << std::flush;
        
        return selected;
    }

    int giveDialog(nlohmann::json dialog)
    {
        if (dialog.contains("text"))
        {
            std::cout << dialog["text"].get<std::string>() << std::endl;
        }
        if (dialog.contains("choices"))
        {
            if (dialog.contains("defaultChoice"))
            {
                return chooseOption(dialog["choices"], dialog["defaultChoice"].get<int>());
            }
            
            return chooseOption(dialog["choices"]);
        }     
        return -1;
    }

    int numberDialog(nlohmann::json dialog)
    {
        std::cout << dialog["text"].get<std::string>() << std::endl;
        std::string list[] = {"<<<", "<<", "<", "", ">", ">>", ">>>"};
        int selected = 0;
        int number = 0;
        while (selected != 3)
        {
            list[3] = std::to_string(number);
            selected = chooseOption(7, list, selected);
            switch (selected)
            {
            case -1:
                selected = 0;
                break;
            case 0:
                number -= 100;
                break;

            case 1:
                number -= 10;
                break;
            
            case 2:
                number -= 1;
                break;
            
            case 4:
                number += 1;
                break;
            
            case 5:
                number += 10;
                break;
            
            case 6:
                number += 100;
                break;
            
            default:
                break;
            }
        }
        return number;
    }

    std::string textEntryDialog(nlohmann::json dialog)
    {
        std::cout << dialog["text"].get<std::string>() << std::endl;
        std::string input;
        std::getline(std::cin, input);
        return input;
    }

    int getDiceRoll(int max)
    {
        return RNG::get().uniformInt(1, max);
    }
} // namespace gameUtil