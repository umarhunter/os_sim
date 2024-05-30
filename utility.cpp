//
// Created by educa on 4/5/2024.
//
#include <string>
#include <vector>
#include <chrono>

struct FileReadRequest
{
    int  PID{0};
    std::string fileName{""};
};

struct MemoryItem
{
    unsigned long long pageNumber;
    unsigned long long frameNumber;
    int PID; // PID of the process using this frame of memory
    std::chrono::steady_clock::time_point lastAccessed; // contains timestamp for last access
};

using MemoryUsage = std::vector<MemoryItem>;

constexpr int NO_PROCESS{ 0 };
