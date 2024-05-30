//
// Created by Umar on 4/4/2024.
//

#ifndef OPSYS_PROJECT_PCB_H
#define OPSYS_PROJECT_PCB_H

#include <string>
#include <vector>

/// The process control block of every process running in OpSys
struct PCB {
    PCB(int PID = 0, std::string pState="", int parentPID=0, bool zombie=false);
    int PID;
    std::string pState;
    int parentPID;
    std::vector<int> childrenPID;
    bool zombie{};
};


#endif //OPSYS_PROJECT_PCB_H
