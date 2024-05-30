/// umarhunter

#ifndef OPSYS_PROJECT_OPSYS_H
#define OPSYS_PROJECT_OPSYS_H

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <unordered_map>
#include <set>
#include <chrono>
#include <algorithm>

#include "utility.cpp"
#include "PCB.h"

class OpSys {
private:
    int numberOfDisks;
    unsigned long long amountOfRAM;
    unsigned int pageSize;
    std::deque<int> readyQueue;
    MemoryUsage memoryUsage{};
    int nextPID;
    int runningPID;
    std::vector<std::deque<FileReadRequest>> diskQueues{};
    std::unordered_map<int, PCB> allPCB{};
public:
    OpSys(int numberOfDisks, unsigned long long amountOfRAM, unsigned int pageSize);
    void NewProcess();
    bool SimFork();
    void SimExit();
    void releaseMemory(int exitingPID);
    void checkParentStatus(int exitingPID, int parentPID, PCB& exitingProcess);
    void SimWait();
    void checkZombie(int parentPID, PCB& parentProcess);
    void TimerInterrupt();
    void DiskReadRequest(int diskNumber, std::string fileName);
    void DiskJobCompleted(int diskNumber);
    void AccessMemoryAddress(unsigned long long address);
    bool pageFound(int targetPID, unsigned long long pageNumber);
    void load_to_RAM(int pageNumber);

    int GetCPU();
    std::deque<int> GetReadyQueue();
    MemoryUsage GetMemory();
    FileReadRequest GetDisk(int diskNumber);
    std::deque<FileReadRequest> GetDiskQueue(int diskNumber);
};

#endif //OPSYS_PROJECT_OPSYS_H
