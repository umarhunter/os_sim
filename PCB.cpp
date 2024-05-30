//
// Created by educa on 4/4/2024.
//

#include "PCB.h"

/// Default constructor sets all initial value of PCB struct
PCB::PCB(int PID, std::string pState, int parentPID, bool zombie) {
    this->PID = PID;
    this->pState = pState;
    this->parentPID = parentPID;
    this->zombie = zombie;
}
