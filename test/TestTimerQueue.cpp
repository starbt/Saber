#include "../src/net/EventLoop.h" 
#include <iostream>
#include <string>
#include <functional>

void print(std::string msg) 
{
    std::cout << msg << std::endl;
}

int main() 
{
    EventLoop loop;
    loop.runAfter(1, std::bind(print, "1 second"));
    loop.runAfter(1.5, std::bind(print, "1.5 second"));
    loop.runAfter(2, std::bind(print, "2 second"));
    loop.runEvery(2,  std::bind(print, "every 2 second"));
    
    loop.loop();
    return 0;
}