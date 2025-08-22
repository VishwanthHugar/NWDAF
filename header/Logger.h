#ifndef _INCLUDE_FileLogger_H
#define _INCLUDE_FileLogger_H

#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <chrono>
#include <iomanip>
#include <ctime>

class FileLogger {
public:
    // Singleton instance
    static FileLogger& instance(const std::string& filename = "NWDAF.log") {
        static FileLogger logger(filename);
        return logger;
    }

    // Thread-safe info log with caller function
    void info(const std::string& msg, const std::string& funcName = "") {
        log("INFO", msg, funcName);
    }

    // Thread-safe error log with caller function
    void error(const std::string& msg, const std::string& funcName = "") {
        log("ERR", msg, funcName);
    }

private:
    std::ofstream ofs;
    std::mutex mMtx;

    FileLogger(const std::string& filename) {
        ofs.open(filename, std::ios::app);
        if (!ofs.is_open()) {
            throw std::runtime_error("Failed to open log file: " + filename);
        }
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

        ofs << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] "
            << "[" << level << "] ";
        if (!funcName.empty()) ofs << "[" << funcName << "] ";
        ofs << msg << std::endl;
    }
};

// -------------------- Macros for convenience --------------------
#define LOG_INFO_FILE(msg)  FileLogger::instance().info(msg, __FUNCTION__)
#define LOG_ERROR_FILE(msg) FileLogger::instance().error(msg, __FUNCTION__)


#endif // _INCLUDE_FileLogger_H
