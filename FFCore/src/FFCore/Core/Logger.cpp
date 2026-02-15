#include "FFCore/Core/Logger.h"

namespace FF
{
    void Logger::Start()
    {
        bool expected = false;
        // if started == expected, then started = true
        if (started.compare_exchange_strong(expected, true))
        {
            stopRequested.store(false);
            worker = std::thread(&Logger::WorkerLoop, this);
        }
    }

    void Logger::Stop()
    {
        if (!started.load())
        {
            return;
        }

        // this is checked in WorkerLoop
        stopRequested.store(true);
        cv.notify_one();

        if (worker.joinable())
        {
            worker.join();
        }

        started.store(false);
    }

    void Logger::SetLogLevel(uint16_t inLogLevel)
    {
        logLevel.store(inLogLevel, std::memory_order_relaxed);
    }

    void Logger::SetLogToFile(bool enabled)
    {
        logToFile.store(enabled, std::memory_order_relaxed);
        cv.notify_one();
    }

    void Logger::Enqueue(LogVerbosity level, const char* prefix, const char* formattedMessage)
    {
        if (!started.load(std::memory_order_acquire))
        {
            Start();
        }

        if (static_cast<uint16_t>(level) > logLevel.load(std::memory_order_relaxed))
        {
            return;
        }

        LogItem item{};
        item.level = level;
        strcpy_s(item.prefix, prefix);
        strcpy_s(item.message, formattedMessage);

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            queue.push_back(item);
        }
        
        cv.notify_one();
    }

    void Logger::EnsureFileOpenIfNeeded()
    {
        if (logToFile.load(std::memory_order_relaxed))
        {
            if (!logFile.is_open())
            {
                logFile.open(logFilePath, std::ios::out | std::ios::app);
            }
        }
        else
        {
            if (logFile.is_open())
            {
                logFile.close();
            }
        }
    }

    void Logger::PushToRingBuffer(LogVerbosity level, const char* msgOnly)
    {
        std::lock_guard<std::mutex> lock(ringMutex);

        if (loggedLineBufferCurrCount >= loggedLineBufferMaxCount)
        {
            for (i32 i = 0; i < (i32)loggedLineBufferMaxCount - 1; ++i)
            {
                logLinesVerbosity[i] = logLinesVerbosity[i + 1];
                std::swap(logLines[i], logLines[i + 1]);
            }
            logLinesVerbosity[loggedLineBufferMaxCount - 1] = level;
            strcpy_s(logLines[loggedLineBufferMaxCount - 1], msgOnly);
        }
        else
        {
            logLinesVerbosity[loggedLineBufferCurrCount] = level;
            strcpy_s(logLines[loggedLineBufferCurrCount], msgOnly);
            loggedLineBufferCurrCount++;
        }
    }

    void Logger::WorkerLoop()
    {
        EnsureFileOpenIfNeeded();

        // local copy of queue to keep locking to minimum 
        std::deque<LogItem> local;

        while (true)
        {
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                cv.wait(lock, [&]
                {
                    return stopRequested.load(std::memory_order_relaxed) || !queue.empty();
                });
                
                local.swap(queue);
            }

            EnsureFileOpenIfNeeded();

            for (const LogItem& item : local)
            {
                // console output (single thread => no interleaving)
                if (item.level == Assert)
                {
                    std::printf("%s%s\n", item.prefix, item.message);
                }
                else
                {
                    std::cout << item.prefix << item.message << '\n';
                }

                // File output
                if (logFile.is_open())
                {
                    logFile << item.prefix << item.message << '\n';
                }

                PushToRingBuffer(item.level, item.message);
            }

            local.clear();

            if (stopRequested.load(std::memory_order_relaxed))
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                if (queue.empty())
                {
                    break;
                }
            }
        }
        
        if (logFile.is_open())
        {
            logFile.close();
        }
    }
}
