#include <string>
#include <iostream>
#include <ostream>
#include <fstream>

class Debug {
public:
    static void initDebug(const char* path);
    static Debug dbg;
    
    template<typename T> void operator<<(T t)
    {
        std::ofstream file(path.c_str());
        file << t;
        file.close();
    }
private:
    std::string path;
    Debug();
};
