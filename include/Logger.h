#ifndef _INCLUDE_FileLogger_H
#define _INCLUDE_FileLogger_H

#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <filesystem>
#include <thread>

class FileLogger {
public:
    // Singleton instance
    static FileLogger& instance(const std::string& filename = "NWDAF.log", size_t maxFileSize = 5 * 1024 * 1024) {
        static FileLogger logger(filename, maxFileSize);
        return logger;
    }

    void info(const std::string& msg, const std::string& funcName = "") {
        log("INFO", msg, funcName);
    }

    void error(const std::string& msg, const std::string& funcName = "") {
        log("ERR", msg, funcName);
    }

private:
    std::ofstream ofs;
    std::mutex mMtx;
    std::string baseFilename;
    size_t maxSize; // max size in bytes for rolling
    int fileIndex = 0;

    FileLogger(const std::string& filename, size_t maxFileSize)
        : baseFilename(filename), maxSize(maxFileSize)
    {
        openFile();
    }

    ~FileLogger() {
        if (ofs.is_open()) ofs.close();
    }

    FileLogger(const FileLogger&) = delete;
    FileLogger& operator=(const FileLogger&) = delete;

    void log(const std::string& level, const std::string& msg, const std::string& funcName) {
        std::lock_guard<std::mutex> lk(mMtx);

        auto now = std::chrono::system_clock::now();
        auto tt = std::chrono::system_clock::to_time_t(now);
        auto tm = *std::localtime(&tt);

        std::ostringstream logLine;
        logLine << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] "
            << "[" << level << "] "
            << "[Thread-" << std::this_thread::get_id() << "] ";
        if (!funcName.empty()) logLine << "[" << funcName << "] ";
        logLine << msg << std::endl;

        ofs << logLine.str();
        ofs.flush();

        if (std::filesystem::file_size(baseFilename) >= maxSize) {
            rollFile();
        }
    }

    void openFile() {
        ofs.open(baseFilename, std::ios::app);
        if (!ofs.is_open()) {
            throw std::runtime_error("Failed to open log file: " + baseFilename);
        }
    }

    void rollFile() {
        ofs.close();
        std::string newName = baseFilename + "." + std::to_string(++fileIndex);
        std::filesystem::rename(baseFilename, newName);
        openFile();
    }
};

// -------------------- Function scope logger --------------------
class LogScope {
public:
    LogScope(const std::string& funcName)
        : mFuncName(funcName),
        mStartTime(std::chrono::high_resolution_clock::now())
    {
        FileLogger::instance().info("Entering function", mFuncName);
    }

    ~LogScope() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - mStartTime).count();
        FileLogger::instance().info("Exiting function (Duration: " + std::to_string(duration) + " ms)", mFuncName);
    }

private:
    std::string mFuncName;
    std::chrono::high_resolution_clock::time_point mStartTime;
};

// -------------------- Macros --------------------
#define LOG_INFO_FILE(msg)   FileLogger::instance().info(msg, __FUNCTION__)
#define LOG_ERROR_FILE(msg)  FileLogger::instance().error(msg, __FUNCTION__)
#define LOG_SCOPE()          LogScope _logScopeInstance(__FUNCTION__)

#endif // _INCLUDE_FileLogger_H
