#include "WindowManager.hpp"
#include "Debug.hpp"
#include "magicNumbers.hpp"

WindowManager::WindowManager()
{
    Debug::dbg << "Initializing window and text variable\n";
    window = initscr();
    text = std::vector<std::string>();
    Debug::dbg << "Getting window size\n";
    getmaxyx(window, max_y, max_x);
    max_x -= WINDOW_PADDING_RIGHT;
    max_y -= WINDOW_PADDING_DOWN;
    current_line = 0;
    Debug::dbg << "Configuring ncurses\n";
    cbreak();
    noecho();
    nodelay(window, TRUE);
    curs_set(0);
}
void WindowManager::writeNewLine(std::string _text)
{
    #ifdef WINDOW_RENDER_DEBUG
    Debug::dbg << "Writing new line to window manager: " << _text << "\n";
    #endif
    text.push_back("  " + _text);
    current_line = text.size() - 1;
}
void WindowManager::writePos(int y, std::string _text)
{
    #ifdef WINDOW_RENDER_DEBUG
    Debug::dbg << "Replacing line " << y << " with : " << _text << "\n";
    #endif
    text[y] = "";
    for (int i = 0; i < WINDOW_PADDING_DOWN; i++)
    {
        text[y] += " ";
    }
    
    text[y] += _text;
    current_line = y;
}

void WindowManager::removeLastLine()
{
    text.pop_back();
}

void WindowManager::updateWindow()
{
    #ifdef WINDOW_RENDER_DEBUG
    Debug::dbg << "Updating window display\n";
    #endif
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
    #ifdef WINDOW_RENDER_DEBUG
    Debug::dbg << "Window update complete\n";
    #endif
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
    #ifdef WINDOW_RENDER_DEBUG
    Debug::dbg << "Clearing window contents\n";
    #endif
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
