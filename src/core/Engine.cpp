#include "core/Engine.hpp"

#include "core/system/window/WindowHandler.hpp"
#include "core/renderer/VulkanRenderer.hpp"

Engine::Engine()
    : m_isRunning(false), m_windowHandler(nullptr), m_renderer(nullptr) {}

Engine::~Engine() {}

void Engine::run()
{
    init();
    mainLoop();
    cleanup();
}

void Engine::init()
{
    // Initialize Window
    m_windowHandler = new WindowHandler(800, 600, "Vulkan App");
    m_windowHandler->init();

    // Check if Vulkan is supported (it is a check made in GLFW, it is done by WindownHandler to contain the GLFW code encapsulated)
    if (!m_windowHandler->isVulkanSupported())
    {
        throw std::runtime_error("GLFW: Vulkan is not Supported\n");
    }

    // Initialize Vulkan Renderer
    m_renderer = new VulkanRenderer(m_windowHandler);
    m_renderer->initVulkan();

    m_isRunning = true;
}

void Engine::mainLoop()
{
    while (m_isRunning)
    {
        m_windowHandler->pollEvents();

        if (m_windowHandler->shouldClose())
        {
            m_isRunning = false;
        }

        // m_renderer->drawFrame();
    }
}

void Engine::cleanup()
{
    if (m_renderer != nullptr)
    {
        m_renderer->cleanup();
        delete m_renderer;
        m_renderer = nullptr;
    }

    if (m_windowHandler != nullptr)
    {
        m_windowHandler->cleanUp();
        delete m_windowHandler;
        m_windowHandler = nullptr;
    }
}
