#include "WindowManager.hpp"

WindowManager::WindowManager()
{
    Debug::dbg << "Initializing window and text variable\n";
    window = initscr();
    text = std::vector<std::string>();
    Debug::dbg << "Getting window size\n";
    getmaxyx(window, max_y, max_x);
    current_line = 0;
    Debug::dbg << "Configuring ncurses\n";
    cbreak();
    noecho();
    nodelay(window, TRUE);
    curs_set(0);
}
void WindowManager::writeNewLine(std::string _text)
{
    // Debug::dbg << "Writing new line to window manager: " << _text << "\n";
    text.push_back(_text);
    current_line = text.size() - 1;
}
void WindowManager::writePos(int y, std::string _text)
{
    text[y] = _text;
    current_line = y;
}

void WindowManager::removeLastLine()
{
    text.pop_back();
}

void WindowManager::updateWindow()
{
    // Debug::dbg << "Updating window display\n";
    clearWindow();
    int max = std::min(getLastLine(), max_y-1);
    if (current_line < max)
    {
        current_line = max;
    }
    else if (current_line > getLastLine())
    {
        current_line = getLastLine();
    }
    
    for (int i = current_line - max_y + 1; i <= current_line; i++)
    {
        if (i < 0 || i >= text.size())
        {
            continue;
        }
        printw("%s\n", text[i].c_str());
    }
    refresh();
    // Debug::dbg << "Window update complete\n";
}
int WindowManager::getLastLine()
{
    return text.size()-1;
}
int WindowManager::getMaxX()
{
    return max_x;
}
int WindowManager::getMaxY()
{
    return max_y;
}
void WindowManager::clearWindow()
{
    // Debug::dbg << "Clearing window contents\n";
    erase();
    move(0,0);
}
WindowManager::~WindowManager()
{
    Debug::dbg << "Destroying window manager\n";
    endwin();
}

WINDOW *WindowManager::getWindow()
{
    return window;
}
