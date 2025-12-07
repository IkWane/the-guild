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

int gameUtil::chooseOption(int optionsLen, std::string options[], int defaultChoice, bool giveResult)
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
        if (key == DEFAULT_KEY_ESCAPE && getch() == DEFAULT_KEY_ARROW_PREFIX)
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

    int x, y;
    getyx(stdscr, y, x);
    move(y, 0);
    clrtoeol();
    if (giveResult && selected != -1)
    {
        printw("> %s", options[selected].c_str());
        move(y+1, 0);
    }

    refresh();
    
    return selected;
}

int gameUtil::chooseOption(nlohmann::json options, int defaultChoice, bool giveResult)
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
                    printw(" > %s < ", options[i].get<std::string>().c_str());
                }
                else
                {
                    printw("   %s   ", options[i].get<std::string>().c_str());
                }
            }
            refresh();
        }

        int key = getch();
        if (key == DEFAULT_KEY_ESCAPE && getch() == DEFAULT_KEY_ARROW_PREFIX)
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

    int x, y;
    getyx(stdscr, y, x);
    move(y, 0);
    clrtoeol();
    if (giveResult && selected != -1)
    {
        printw("> %s", options[selected].get<std::string>().c_str());
        move(y+1, 0);
    }
    refresh();
    
    return selected;
}