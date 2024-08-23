#pragma once

class GLFWwindow;

class WindowHandler
{
public:
    WindowHandler(const int width, const int height, const char *title);
    ~WindowHandler();

    void init();
    void cleanUp();

    // Get the window size in pixels
    void GetWindowFramebufferSize(int *width, int *height);

    void *GetMetalLayer();

    void pollEvents();

    bool shouldClose() const;

    bool isVulkanSupported() const;

    GLFWwindow *getWindow() const { return m_window; }

private:
    GLFWwindow *m_window;
    int m_width;  // in screen coordinates
    int m_height; // in screen coordinates
    const char *m_title;
};
