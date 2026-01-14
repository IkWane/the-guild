#pragma once
#include <ncurses.h>
#include <string>
#include <vector>

class WindowManager
{
public:
    WindowManager();
    void writeNewLine(std::string _text = "");
    void writePos(int y, std::string _text);
    void removeLastLine();
    void updateWindow();
    ~WindowManager();
    WINDOW* getWindow();
    int waitForKeyPress()
    {
        nodelay(window, FALSE);
        int c = getch();
        nodelay(window, TRUE);
        return c;
    }
    int getInstantKeyPress()
    {
        nocbreak();
        int c = getch();
        cbreak();
        return c;
    }
    int current_line;
    int getLastLine();
    int getMaxX();
    int getMaxY();
private:
    WINDOW* window;
    void clearWindow();
    std::vector<std::string> text;
    int max_y, max_x;
};