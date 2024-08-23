#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <mutex>

enum class LogLevel
{
    INFO,
    WARNING,
    ERROR,
    DEBUG
};

class Logger
{
public:
    static Logger &getInstance();

    // Delete copy constructor and assignment operator to ensure singleton
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    // Public methods to log messages
    void log(LogLevel level, const std::string &message);
    void setLogFile(const std::string &filename);
    void setLogLevel(LogLevel level);
    void cleanUp();

private:
    Logger(); // Private constructor for singleton pattern
    ~Logger();

    void logToConsole(LogLevel level, const std::string &message);
    void logToFile(LogLevel level, const std::string &message);

    std::ofstream m_logFile;
    LogLevel m_logLevel;
    std::mutex m_mutex; // To ensure thread-safety
};
