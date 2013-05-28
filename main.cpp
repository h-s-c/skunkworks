// Public Domain

#include "framework/framework.hpp"

#include <iostream>
#include <stdexcept>

#include <zmq.hpp>

int main(int argc, char* argv[])
{
    try
    {
        Framework framework;
        framework.Loop();
        return 0;
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << "Runtime error: " << e.what() << std::endl;
    }
    catch(const zmq::error_t& ze)
    {
       std::cerr << "ZMQ error: " << ze.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr  << "General exception: " <<e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr  << "Unhandled exception!" << std::endl;
    }
    return 1;
}
