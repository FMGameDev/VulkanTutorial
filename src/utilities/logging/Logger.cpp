#include "utilities/logging/Logger.hpp"

#include <vulkan/vulkan.hpp>
#include <ctime>

Logger::Logger() : m_logLevel(LogLevel::INFO) {}

Logger::~Logger() {}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

Logger &Logger::getInstance()
{
    static Logger instance;
    return instance;
}

void Logger::setLogFile(const std::string &filename)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_logFile.is_open())
    {
        m_logFile.close();
    }
    m_logFile.open(filename, std::ios::out | std::ios::app);
    if (!m_logFile)
    {
        throw std::runtime_error("Failed to open log file: " + filename);
    }
}

void Logger::setLogLevel(LogLevel level)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_logLevel = level;
}

void Logger::log(LogLevel level, const std::string &message)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (level < m_logLevel)
    {
        return; // Skip logs below the current log level
    }

    logToConsole(level, message);
    logToFile(level, message);
}

void Logger::logToConsole(LogLevel level, const std::string &message)
{
    std::string prefix;

    switch (level)
    {
    case LogLevel::INFO:
        prefix = "[INFO] ";
        break;
    case LogLevel::WARNING:
        prefix = "[WARNING] ";
        break;
    case LogLevel::ERROR:
        prefix = "[ERROR] ";
        break;
    case LogLevel::DEBUG:
        prefix = "[DEBUG] ";
        break;
    }

    std::cout << prefix << message << std::endl;
}

void Logger::logToFile(LogLevel level, const std::string &message)
{
    if (m_logFile.is_open())
    {
        std::string prefix;
        switch (level)
        {
        case LogLevel::INFO:
            prefix = "[INFO] ";
            break;
        case LogLevel::WARNING:
            prefix = "[WARNING] ";
            break;
        case LogLevel::ERROR:
            prefix = "[ERROR] ";
            break;
        case LogLevel::DEBUG:
            prefix = "[DEBUG] ";
            break;
        }

        // Write timestamp to log
        std::time_t now = std::time(nullptr);
        char timeStamp[100];
        std::strftime(timeStamp, sizeof(timeStamp), "[%Y-%m-%d %H:%M:%S] ", std::localtime(&now));

        m_logFile << timeStamp << prefix << message << std::endl;
    }
}

void Logger::cleanUp()
{
    if (m_logFile.is_open())
    {
        m_logFile.close();
    }
}
