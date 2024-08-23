#include "core/system/window/WindowHandler.hpp"

#include "core/system/window/MacOsWindowUtils.hpp"

#define VK_USE_PLATFORM_MACOS_MVK
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#include <stdexcept>

WindowHandler::WindowHandler(const int width, const int height, const char *title)
    : m_width(width), m_height(height), m_title(title), m_window(nullptr) {}

WindowHandler::~WindowHandler() {}

void WindowHandler::init()
{
    // Initializes the GLFW library
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW\n");
    }

    // No OpenGL context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // Don't allow to resize window
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // Create window
    m_window = glfwCreateWindow(m_width, m_height, "Vulkan", nullptr, nullptr);
    if (!m_window)
    {
        throw std::runtime_error("Failed to create GLFW Window\n");
    }
}

void WindowHandler::GetWindowFramebufferSize(int *width, int *height)
{
    glfwGetFramebufferSize(m_window, width, height);
}

void *WindowHandler::GetMetalLayer()
{
    // Retrieve the Cocoa window handle (NSWindow*)
    void *windowHandle = glfwGetCocoaWindow(m_window);
    if (!m_window)
    {
        throw std::runtime_error("Failed to retrieve NSWindow handle!");
    }

    // Get the content view and make it Metal-compatible
    void *viewHandle = getViewFromNSWindowPointer(windowHandle);
    if (!viewHandle)
    {
        throw std::runtime_error("Failed to retrieve NSView handle!");
    }

    return getMetalLayerFromView(viewHandle);
}

void WindowHandler::pollEvents()
{
    glfwPollEvents();
}

bool WindowHandler::shouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

bool WindowHandler::isVulkanSupported() const
{
    return glfwVulkanSupported();
}

void WindowHandler::cleanUp()
{
    if (m_window != nullptr)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}
