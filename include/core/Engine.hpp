// Engine: Manages the core systems of the application, including the main loop, window, and Vulkan renderer.
// It is the central place where the main components interact.

#pragma once

class WindowHandler;
class VulkanRenderer;

class Engine
{
public:
    Engine();
    ~Engine();

    void run();

private:
    void init();
    void mainLoop();
    void cleanup();

    WindowHandler *m_windowHandler;
    VulkanRenderer *m_renderer;
    bool m_isRunning;
};
