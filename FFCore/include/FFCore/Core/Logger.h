#pragma once

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <deque>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include "Types.h"

namespace FF
{
    inline constexpr u16 LOG_TEMPLATE_LENGTH = 256;
    
    enum LogVerbosity : u8
    {
        Assert  = 0,
        Error   = 1,
        Warning = 2,
        Message = 3,
    };

    struct LogItem
    {
        LogVerbosity level;
        char prefix[LOG_TEMPLATE_LENGTH];
        char message[1024];
    };

    class Logger
    {
    public:
        static Logger& GetInstance()
        {
            static Logger instance;
            return instance;
        }

        Logger(Logger const&) = delete;
        void operator=(Logger const&) = delete;
        
        void Start();
        void Stop();
        
        void SetLogLevel(uint16_t inLogLevel);
        void SetLogToFile(bool enabled);
        
        void Enqueue(LogVerbosity level, const char* prefix, const char* formattedMessage);
        
        static constexpr const char* logFilePath = "../log/log.txt";
        static constexpr uint16_t loggedLineBufferMaxCount = 256;
        static constexpr uint16_t logLettersMaxCount = 1024;

        template <typename... Args>
        void EnqueueFmt(LogVerbosity level, const char* prefix, const char* fmt, Args... args)
        {
            char msg[logLettersMaxCount];
            _snprintf_s(msg, _countof(msg), _TRUNCATE, fmt, args...);
            Enqueue(level, prefix, msg);
        }

    private:
        Logger() = default;

        ~Logger()
        {
            Stop();
        }

        void EnsureFileOpenIfNeeded();

        void PushToRingBuffer(LogVerbosity level, const char* msgOnly);

        void WorkerLoop();
        
        std::atomic<u16> logLevel{ 5 };
        std::atomic<bool> logToFile{ true };
        
        std::atomic<bool> started{ false };
        std::atomic<bool> stopRequested{ false };
        std::thread worker;
        
        std::mutex queueMutex;
        std::condition_variable cv;
        std::deque<LogItem> queue;
        
        std::ofstream logFile;
        
        std::mutex ringMutex;
        char logLines[loggedLineBufferMaxCount][logLettersMaxCount] = {};
        LogVerbosity logLinesVerbosity[loggedLineBufferMaxCount] = {};
        u16 loggedLineBufferCurrCount = 0;
    };
}


#ifdef FF_DEBUG
    #define LOG_ERROR(fmt, ...) do { \
        char prefix[FF::LOG_TEMPLATE_LENGTH]; \
        sprintf_s(prefix, "[Error] [%s] ", __FUNCTION__); \
        FF::Logger::GetInstance().EnqueueFmt(FF::LogVerbosity::Error, prefix, fmt, ##__VA_ARGS__); \
    } while(0);

    #define LOG_WARNING(fmt, ...) do { \
        char prefix[FF::LOG_TEMPLATE_LENGTH]; \
        sprintf_s(prefix, "[Warning] [%s] ", __FUNCTION__); \
        FF::Logger::GetInstance().EnqueueFmt(FF::LogVerbosity::Warning, prefix, fmt, ##__VA_ARGS__); \
    } while(0);

    #define LOG_MESSAGE(fmt, ...) do { \
        char prefix[FF::LOG_TEMPLATE_LENGTH]; \
        sprintf_s(prefix, "[Message] [%s] ", __FUNCTION__); \
        FF::Logger::GetInstance().EnqueueFmt(FF::LogVerbosity::Message, prefix, fmt, ##__VA_ARGS__); \
    } while(0);
#else
    #define LOG_ERROR(...)   do {} while(0);
    #define LOG_WARNING(...) do {} while(0);
    #define LOG_MESSAGE(...) do {} while(0);
#endif
