// Public Domain

#include "framework/framework.hpp"

#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[])
{
    try
    {
        Framework framework;
        framework.Loop();
        return 0;
    }
    catch(std::runtime_error& e)
    {
        std::cerr << "Runtime error: " << e.what() << std::endl;
    }
    return 1;
}
