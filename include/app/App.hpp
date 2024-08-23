// App: A higher-level class that controls the overall application flow. It initializes and runs the Engine.

#pragma once

#include "core/Engine.hpp"

class App
{
public:
    App();
    ~App();

    void run(); // Starts the application

private:
    void cleanUp();
    Engine engine;
    // Engine m_engine;
};
