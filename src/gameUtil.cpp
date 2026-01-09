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

float gameUtil::sigmoid(int x, float steepness)
{
    return 1 / (1 + exp(-steepness * x));
}

// Reads a string from ncurses input until Enter is pressed
std::string gameUtil::readStr(WindowManager &wm)
{
    std::string input;
    wm.writePos(wm.current_line, "_");
    wm.updateWindow();
    int key = wm.waitForKeyPress();

    while (key != '\n')
    {
        if (key == DEFAULT_KEY_BACKSPACE)
        {
            if (!input.empty())
            {
                input.pop_back();
            }
        }
        else if (key == DEFAULT_KEY_ESCAPE)
        {
            key = wm.waitForKeyPress();
            if (key == DEFAULT_KEY_SPECIAL_PREFIX)
            {
                key = wm.waitForKeyPress();
                switch (key)
                {
                    case DEFAULT_KEY_SCROLL_DOWN:
                    {
                        wm.current_line += 1;
                        wm.updateWindow();
                        break;
                    }
                    case DEFAULT_KEY_SCROLL_UP:
                    {
                        wm.current_line -= 1;
                        wm.updateWindow();
                        break;
                    }
                    default:
                    break;
                }
            }
            else if (key == DEFAULT_KEY_ESCAPE)
            {
                return "";
            }
        }
        else 
        {
            input.push_back(key);
        }
        
        wm.writePos(wm.getLastLine(), input + "_");
        wm.updateWindow();
        key = wm.waitForKeyPress();
    }
    
    return input;
}

// Presents a horizontal choice menu to the player and returns the selected option index
int gameUtil::chooseOption(WindowManager &wm, int optionsLen, std::string options[], int defaultChoice, bool giveResult)
{
    Debug::dbg << "Presenting horizontal choice menu with " << optionsLen << " options\n";
    int previous_selected = -1;
    int selected = defaultChoice;
    int line = wm.getLastLine();
    while (true)
    {
        if (previous_selected != selected)
        {
            previous_selected = selected;
            std::string text = "";
            for (int i = 0; i < optionsLen; i++)
            {
                if (selected == i)
                {
                    text += " > " + options[i] + " < ";
                }
                else
                {
                    text += "   " + options[i] + "   ";
                }
            }
            wm.writePos(line, text);
            wm.updateWindow();
        }

        int key = wm.waitForKeyPress();
        if (key == DEFAULT_KEY_ESCAPE)
        {
            key = wm.getInstantKeyPress();
            Debug::dbg << key << "\n";
            if (key == DEFAULT_KEY_SPECIAL_PREFIX) {
                key = wm.waitForKeyPress();
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
                    case DEFAULT_KEY_SCROLL_DOWN:
                    {
                        wm.current_line += 1;
                        wm.updateWindow();
                        break;
                    }
                    case DEFAULT_KEY_SCROLL_UP:
                    {
                        wm.current_line -= 1;
                        wm.updateWindow();
                        break;
                    }
                    default:
                        break;
                }
            }
            else if (key == -1)
            {
                selected = -1;
                break;
            }
        }
        else if (key == DEFAULT_KEY_ENTER)
        {
            break;
        }
    }
    if (giveResult && selected != -1)
    {
        wm.writePos(line, "> " + options[selected]);
    }
    if (!giveResult)
    {
        wm.removeLastLine();
    }

    wm.updateWindow();
    
    return selected;
}

// Presents a horizontal choice menu to the player using a JSON array of options and returns the selected option index
int gameUtil::chooseOption(WindowManager &wm, nlohmann::json options, int defaultChoice, bool giveResult)
{
    std::string strOptions[options.size()];

    for (int i = 0; i < options.size(); i++)
    {
        strOptions[i] = options[i].get<std::string>();
    }

    return chooseOption(wm, int(options.size()), strOptions, defaultChoice, giveResult);
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
void gameUtil::renderCards(WindowManager &wm, std::vector<std::vector<std::string>> cardsLines)
{
    Debug::dbg << "Trying to render " << cardsLines.size() << " cards\n";
    int maxWidth;
    for (auto &cardLines : cardsLines)
    {
        maxWidth = std::max(maxWidth, static_cast<int>(cardLines[0].length()));
    }
    
    int totalWidth = (maxWidth + 2) * cardsLines.size();
    int maxX = wm.getMaxX();
    int maxY = wm.getMaxY();

    std::vector<std::vector<std::string>> extraCardsLines;

    int maxCardsPerRow = std::max(1, maxX / int(cardsLines[0][0].length() + 2));
    int extraCards = std::max(0, int(cardsLines.size() )- maxCardsPerRow);

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
        std::string output = "";
        for (auto &cardLines : cardsLines)
        {
            if (i < cardLines.size())
            {
                output += cardLines[i] + "  ";
            }
            else
            {
                output += gameUtil::fitStr("", cardLines[0].length()) + "  ";
            }
        }
        wm.writeNewLine(output);
    }
    wm.updateWindow();

    if (extraCardsLines.size() > 0)
    {
        wm.writeNewLine();
        renderCards(wm, extraCardsLines);
    }
}
