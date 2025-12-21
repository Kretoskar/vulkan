#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include "Types.h"

enum : u16
{
    LOG_TEMPLATE_LENGTH = 256
};

#ifdef FF_DEBUG
    #define LOG_ERROR(...) \
    { \
        char buffer[LOG_TEMPLATE_LENGTH]; \
        sprintf_s(buffer, "[Error] [%s] ", __FUNCTION__); \
        FF::Logger::GetInstance().Log(buffer, FF::LogVerbosity::Error, __VA_ARGS__); \
    }
    
    #define LOG_WARNING(...) \
    { \
        char buffer[LOG_TEMPLATE_LENGTH]; \
        sprintf_s(buffer, "[Warning] [%s] ", __FUNCTION__); \
        FF::Logger::GetInstance().Log(buffer, FF::LogVerbosity::Warning, __VA_ARGS__); \
    }
    
    #define LOG_MESSAGE(...) \
    { \
        char buffer[LOG_TEMPLATE_LENGTH]; \
        sprintf_s(buffer, "[Message] [%s] ", __FUNCTION__); \
        FF::Logger::GetInstance().Log(buffer, FF::LogVerbosity::Message, __VA_ARGS__); \
    }

#else
    #define LOG_ERROR(...) {}
    #define LOG_WARNING(...) {}
    #define LOG_MESSAGE(...) {}
#endif

namespace FF
{
    enum LogVerbosity : u8
    {
        Assert = 0,
        Error = 1,
        Warning = 2,
        Message = 3,
    };

    class Logger
    {
    public:
        static Logger& GetInstance()
        {
            static Logger instance;
            return instance;
        }
        
    private:
        Logger()
        {
            if (logToFile)
            {
                logFile.open(logFilePath);
            }
        }

        // called at program termination
        ~Logger()
        {
            if (logFile.is_open())
            {
                logFile.close();
            }
        }
        
    public:
        Logger(Logger const&) = delete;
        void operator=(Logger const&) = delete;
        
        void SetLogLevel(uint16_t inLogLevel)
        {
            logLevel = inLogLevel;
        }

        template <typename... Args>
        void Log(char logTemplate[LOG_TEMPLATE_LENGTH], LogVerbosity inLogLevel, Args ... args)
        {
            if (inLogLevel <= logLevel)
            {
                char buffer[logLettersMaxCount];
                sprintf_s(buffer, args ...);

                if (inLogLevel < 1)
                {
                    printf("%s %s \n", logTemplate, buffer);
                }
                else
                {
                    std::cout << logTemplate << buffer << '\n';
                }

                if (logToFile)
                {
                    logFile << logTemplate << buffer << '\n';
                }

                if (loggedLineBufferCurrCount >= loggedLineBufferMaxCount)
                {
                    for (int i = 0; i < loggedLineBufferMaxCount - 1; i++)
                    {
                        logLinesVerbosity[i] = logLinesVerbosity[i + 1];
                        std::swap(logLines[i], logLines[i + 1]);
                    }

                    logLinesVerbosity[loggedLineBufferMaxCount - 1] = inLogLevel;
                    strcpy_s(logLines[loggedLineBufferMaxCount - 1], buffer);
                }
                else
                {
                    logLinesVerbosity[loggedLineBufferCurrCount] = inLogLevel;
                    strcpy_s(logLines[loggedLineBufferCurrCount], buffer);
                    loggedLineBufferCurrCount++;
                }
            }
        }

        uint16_t logLevel = 5;
        bool logToFile = true;

        static constexpr const char* logFilePath = "../log/log.txt";
        static constexpr uint16_t loggedLineBufferMaxCount = 256;
        static constexpr uint16_t logLettersMaxCount = 1024;
        
        char logLines[loggedLineBufferMaxCount][logLettersMaxCount] = {};
        LogVerbosity logLinesVerbosity[loggedLineBufferMaxCount] = {};
        uint16_t loggedLineBufferCurrCount = 0;

        std::ofstream logFile;
    };
}