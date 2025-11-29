#include "gameUtil.hpp"


int gameUtil::wrapAround(int a, int n)
{
    if (a >= n)
        return 0;
    else if (a < 0)
        return n-1;
    return a;  
}

std::string gameUtil::readStr()
{
    std::string input;

    nocbreak();
    echo();

    int ch = getch();

    while (ch != '\n')
    {
        input.push_back(ch);
        ch = getch();
    }

    cbreak();
    echo();
    
    return input;
}

int gameUtil::chooseOption(int optionsLen, std::string options[], int defaultChoice)
{
    int previous_selected = -1;
    int selected = defaultChoice;
    while (true)
    {
        if (previous_selected != selected)
        {
            previous_selected = selected;
            printw("\r");
            for (int i = 0; i < optionsLen; i++)
            {
                if (selected == i)
                {
                    printw(" > %s < ", options[i].c_str());
                }
                else
                {
                    printw("   %s   ", options[i].c_str());
                }
            }
            refresh();
        }

        int key = getch();
        if (key == 27 && getch() == 91)
        {
            key = getch();
            switch (key)
            {
                case DEFAULT_KEY_LEFT:
                    selected -= 1;
                    selected = wrapAround(selected, optionsLen);
                    break;
                case DEFAULT_KEY_RIGHT:
                    selected += 1;
                    selected = wrapAround(selected, optionsLen);
                    break;
                default:
                    break;
            }
        }
        else if (key == DEFAULT_KEY_ENTER)
        {
            break;
        }
        else if (key == DEFAULT_KEY_ESCAPE)
        {
            selected = -1;
            break;
        }
    }

    printw("\r");
    for (int i = 0; i < optionsLen; i++)
    {
        printw("   ");
        for (int j = 0; j < options[i].length(); j++)
        {
            printw(" ");
        }
        printw("   ");
    }
    printw("\r");
    refresh();
    
    return selected;
}

int gameUtil::chooseOption(nlohmann::json options, int defaultChoice)
{
    int previous_selected = -1;
    int selected = defaultChoice;
    
    while (true)
    {
        if (previous_selected != selected)
        {
            previous_selected = selected;
            printw("\r");
            for (int i = 0; i < options.size(); i++)
            {
                if (selected == i)
                {
                    printw(" > %s < ", options.at(i).get<std::string>().c_str());
                }
                else
                {
                    printw("   %s   ", options.at(i).get<std::string>().c_str());
                }
            }
            refresh();
        }

        int key = getch();
        if (key == 27 && getch() == 91)
        {
            key = getch();
            switch (key)
            {
                case DEFAULT_KEY_LEFT:
                    selected -= 1;
                    selected = wrapAround(selected, options.size());
                    break;
                case DEFAULT_KEY_RIGHT:
                    selected += 1;
                    selected = wrapAround(selected, options.size());
                    break;
                default:
                    break;
            }
        } 
        else if (key == DEFAULT_KEY_ENTER)
        {
            break;
        }
        else if (key == DEFAULT_KEY_ESCAPE)
        {
            selected = -1;
            break;
        }
    }

    printw("\r");
    for (auto& param : options.items())
    {
        printw("   ");
        std::string e = param.value().get<std::string>();
        for (int j = 0; j < e.length(); j++)
        {
            printw(" ");
        }
        printw("   ");
    }
    printw("\r");
    refresh();
    
    return selected;
}

int gameUtil::giveDialog(nlohmann::json dialog)
{
    if (dialog.contains("text"))
    {
        printw("%s\n", dialog["text"].get<std::string>().c_str());
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

int gameUtil::numberDialog(nlohmann::json dialog)
{
    printw("%s\n", dialog["text"].get<std::string>().c_str());
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

std::string gameUtil::textEntryDialog(nlohmann::json dialog)
{
    printw("%s\n", dialog["text"].get<std::string>().c_str());
    std::string input = readStr();
    return input;
}

int gameUtil::getDiceRoll(int max)
{
    return RNG::get().uniformInt(1, max);
}