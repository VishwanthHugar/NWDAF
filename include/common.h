#ifndef _INCLUDE_COMMON_H_
#define _INCLUDE_COMMON_H_

#include <string>
#include <chrono>
#include <iostream>
#include <atomic>
#include <memory>
#include <vector>

// Types
struct RawEvent {
    std::string source;
    std::string cellId;
    double metric; // e.g., utilization
    std::chrono::system_clock::time_point ts;
};

struct Feature {
    std::string cellId;
    double avgUtil;
    int windowSec;
    std::chrono::system_clock::time_point ts;
};

struct Prediction {
    std::string cellId;
    bool congested;
    double probability;
    std::chrono::system_clock::time_point ts;
};

// Simple logger (replace with proper)
inline void log_info(const std::string& s) {
    std::cout << "[INFO] " << s << std::endl;
}
inline void log_error(const std::string& s) {
    std::cerr << "[ERR] " << s << std::endl;
}

#endif //_INCLUDE_COMMON_H_