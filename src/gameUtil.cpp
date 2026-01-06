#include "gameUtil.hpp"

// Wraps integer a around n (0 to n-1)
int gameUtil::wrapAround(int a, int n)
{
    if (a >= n)
        return 0;
    else if (a < 0)
        return n-1;
    return a;  
}

int gameUtil::sigmoid(int x, int steepness)
{
    return 1 / (1 + exp(-steepness * x));
}

// Reads a string from ncurses input until Enter is pressed
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

// Presents a horizontal choice menu to the player and returns the selected option index
int gameUtil::chooseOption(int optionsLen, std::string options[], int defaultChoice, bool giveResult)
{
    int previous_selected = -1;
    int selected = defaultChoice;
    int x, y;
    getyx(stdscr, y, x);
    while (true)
    {
        if (previous_selected != selected)
        {
            previous_selected = selected;
            move(y, 0);
            clrtoeol();
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

// Presents a horizontal choice menu to the player using a JSON array of options and returns the selected option index
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

// Converts a snake_case string to Normal Case (spaces and optional capitalization)
std::string gameUtil::snakeToNormal(std::string &str, bool upperFirst)
{
    std::string newStr = "";
    for (auto &c : str)
    {
        if (c == '_')
        {
            newStr += ' ';
        } 
        else 
        {
            newStr += c;
        }
    }
    if (upperFirst)
    {
        newStr[0] = std::toupper(newStr[0]);
    }
    return newStr;
}

// Loads a JSON config file and extracts keys and weights into separate vectors
// Note that the JSON file is expected to have entries with a "weight" field
void gameUtil::loadJsonConfig(std::string path, nlohmann::json &data, std::vector<std::string> &keys, std::vector<int> &weights)
{
    data = FileManager::loadJson(path.c_str());
    Debug::dbg << "Loading JSON config from: " << path << "\n";
    weights = std::vector<int>();
    keys = std::vector<std::string>();
    for (auto& [key, val] : data.items())
    {
        weights.push_back(val["weight"].get<int>());
        keys.push_back(key);
    }
}

// Fits a string to a given length by truncating or padding with fillChar (default is space)
std::string gameUtil::fitStr(std::string str, int length, char fillChar)
{
    if (str.length() > length)
    {
        return str.substr(0, length);
    }
    else
    {
        while (str.length() < length)
        {
            str += fillChar;
        }
        return str;
    }
}

// Renders multiple text-based cards side by side in the ncurses window
// Each card is represented as a vector of strings (lines)
// Cards that don't fit in the current row are passed to a new row recursively
void gameUtil::renderCards(std::vector<std::vector<std::string>> cardsLines)
{
    Debug::dbg << "Trying to render " << cardsLines.size() << " cards\n";
    int totalWidth = (cardsLines[0][0].length() + 2) * cardsLines.size();
    int maxX, maxY;
    getmaxyx(stdscr, maxY, maxX);
    std::vector<std::vector<std::string>> extraCardsLines;
    int maxCardsPerRow = std::max(1, maxX / int(cardsLines[0][0].length() + 2));
    int extraCards = cardsLines.size() - maxCardsPerRow;
    Debug::dbg << "Total width of cards: " << totalWidth << ", max screen width: " << maxX << ", extra cards : " << extraCards << "\n";
    extraCardsLines = std::vector<std::vector<std::string>>();
    for (int i = 0; i < extraCards; i++)
    {
        extraCardsLines.push_back(cardsLines.back());
        cardsLines.pop_back();
    }
    int maxHeight = 0;
    for (auto &cardLines : cardsLines)
    {
        if (cardLines.size() > maxHeight)
        {
            maxHeight = cardLines.size();
        }
    }
    Debug::dbg << "Rendering " << cardsLines.size() << " cards with max height " << maxHeight << "\n";
    for (int i = 0; i < maxHeight; i++)
    {
        for (auto &cardLines : cardsLines)
        {
            if (i < cardLines.size())
            {
                printw("%s  ", cardLines[i].c_str());
            }
            else
            {
                printw("%s  ", gameUtil::fitStr("", cardLines[0].length()).c_str());
            }
        }
        printw("\n");
    }
    if (extraCardsLines.size() > 0)
    {
        printw("\n");
        renderCards(extraCardsLines);
    }
}
