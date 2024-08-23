// main.cpp: The entry point for your application that starts the Application class.

#include "app/App.hpp"
#include <iostream>

int main()
{
    try
    {
        App app;
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
