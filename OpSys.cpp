/// umarhunter

#include "OpSys.h"

OpSys::OpSys(int numberOfDisks, unsigned long long amountOfRAM, unsigned int pageSize)
        : numberOfDisks(numberOfDisks), amountOfRAM(amountOfRAM), pageSize(pageSize), nextPID(1), runningPID(0) {
    diskQueues.resize(numberOfDisks); // allocates a size for diskQueues, not really needed
}

/**
 * @brief Creates a new process in the simulated system.
 */
void OpSys::NewProcess() {
    if(readyQueue.empty()) {
        runningPID = nextPID;
    }
    else {
        readyQueue.push_back(nextPID);
    }
    PCB new_process(runningPID);
    new_process.PID = runningPID;

    allPCB[runningPID] = new_process;
    ++nextPID; // increment the PID for the next process
}

/**
 * @brief The currently running process forks a child.
 * @return true if the fork is successful.
 * @throws std::logic_error if there isn't any process currently running.
 */
bool OpSys::SimFork() {
    if (runningPID == 0) throw std::logic_error("CPU is idle"); // There isn't any process currently running

    // nextPID is the child, so add it to parent PCB
    allPCB[runningPID].childrenPID.push_back(nextPID);

    // create new PCB class object with appropriate states
    PCB new_child(nextPID, "ready", runningPID);

    MemoryItem new_child_process{0x00000000, 4, nextPID};
    memoryUsage.push_back(new_child_process);

    // the child needs to be processed
    readyQueue.push_back(nextPID);

    // increment PID for next process
    ++nextPID;

    return true;
}

/**
 * @brief Terminates the currently running process.
 * @throws std::logic_error if there isn't any process currently running.
 */
void OpSys::SimExit() {
    if (runningPID == 0) throw std::logic_error("CPU is idle"); // There isn't any process currently running

    int exitingPID = runningPID;
    PCB exitingProcess = allPCB[exitingPID]; // make a copy to use after deletion

    releaseMemory(exitingPID);


    // Check if the parent is waiting
    int parentPID = exitingProcess.parentPID;
    checkParentStatus(exitingPID, parentPID, exitingProcess);


    if(readyQueue.size() == 0){
        runningPID = 0; // CPU becomes idle+
    }
    else{
        if(runningPID == readyQueue[0])
            runningPID = 0;
        else
            runningPID = readyQueue[0];
        readyQueue.pop_front();
    }
}

/**
 * @brief release all memory with associated PID and terminate its descendants
 * @param exitingPID PID of process to be terminated
 * @param exitingProcess PCB of exiting process
 */
void OpSys::releaseMemory(int exitingPID) {
    auto iterator_index = memoryUsage.begin();
    while (iterator_index != memoryUsage.end()) {
        if (iterator_index->PID == exitingPID) {
            iterator_index = memoryUsage.erase(iterator_index);
        } else {
            ++iterator_index;
        }
    }


    // handle cascading termination so we can terminate all descendants
    std::queue<int> need_to_terminate;
    need_to_terminate.push(exitingPID);

    std::set<int> terminated_processes; // keeping track of terminated processes
    terminated_processes.insert(exitingPID);


    while (!need_to_terminate.empty()) {
        int pid = need_to_terminate.front();
        need_to_terminate.pop();

        PCB process = allPCB[pid]; // make a copy to use after deletion
        for (int childPID : process.childrenPID) {
            if (terminated_processes.find(childPID) == terminated_processes.end()) { // Check if not already terminated
                need_to_terminate.push(childPID);
                terminated_processes.insert(childPID);
            }
        }

        allPCB.erase(pid);
    }
}

/**
 * @brief
 * @param exitingPID PID of exiting process
 * @param parentPID PID of the exiting process' parent
 * @param exitingProcess PCB of exiting process
 */
void OpSys::checkParentStatus(int exitingPID, int parentPID, PCB& exitingProcess) {
    // if parent process exists & it's process state is "waiting" then add it to readyQ and adjust state
    if (parentPID != 0 && allPCB[parentPID].pState == "waiting") {
        allPCB[parentPID].pState = "ready";
        readyQueue.push_back(parentPID);
    } else if (parentPID != 0) {
        // If the parent isn't waiting, the process is a zombie
        std::vector<int> parentChildren = allPCB[parentPID].childrenPID; // all the parents children

        // std::remove moves all occurences to end of container and returns the starter iterator
        auto iterator_for_removal = std::remove(parentChildren.begin(), parentChildren.end(), exitingPID);

        // std::erase will delete everything starting from returned iterator till end of container
        parentChildren.erase(iterator_for_removal, parentChildren.end());

        exitingProcess.zombie = true; // set flag to true
        allPCB[exitingPID] = exitingProcess; // update PCB
    }
}

/**
 * @brief The process wants to pause and wait for any of its child processes to terminate.
 * @throws std::logic_error if there isn't any process currently running.
 */
void OpSys::SimWait() {
    if (runningPID == 0) throw std::logic_error("CPU is idle"); // There isn't any process currently running

    int parentPID = runningPID;
    PCB& parentProcess = allPCB[parentPID];
    checkZombie(parentPID, parentProcess);

    // now we can change the process states
    parentProcess.pState = "waiting";

    readyQueue.push_back(parentPID);
    runningPID = readyQueue[0]; // CPU becomes idle
}

/**
 * @brief checks if given PID is a zombie and reaps any that are found
 * @param parentPID PID of parent processs
 * @param parentProcess the PCB of the parent process
 */
void OpSys::checkZombie(int parentPID, PCB &parentProcess) {
    // Check if any child process is a zombie
    int size = parentProcess.childrenPID.size();
    for(int index = 0; index < size; ++index) {
        int childPID = parentProcess.childrenPID[index];
        PCB& childProcess = allPCB[childPID];

        if(childProcess.zombie) {
            // Reap the zombies!
            auto iterator_index = memoryUsage.begin(); // fast removal algorithm using iterators
            while (iterator_index != memoryUsage.end()) {
                if (iterator_index->PID == childPID) {
                    iterator_index = memoryUsage.erase(iterator_index);
                } else {
                    ++iterator_index;
                }
            }
            allPCB.erase(childPID);
            return;
        }

    }
}

/**
 * @brief Handles the timer interrupt signaling that the time slice of the currently running process is over.
 */
void OpSys::TimerInterrupt(){
    readyQueue.push_back(runningPID);
    runningPID = readyQueue[0];
    readyQueue.pop_front();
}

/**
 * @brief Currently running process requests to read the specified file from the disk with a given number.
 * @param diskNumber The number of the disk from which the file is to be read.
 * @param fileName The name of the file to be read from the disk.
 */
void OpSys::DiskReadRequest(int diskNumber, std::string fileName) {
    FileReadRequest request;
    request.fileName = fileName;
    request.PID = runningPID;
    diskQueues[diskNumber].push_back(request);

    if(readyQueue.size() == 0){
        runningPID = NO_PROCESS;
    }
    else{
        runningPID = readyQueue[0];
        readyQueue.pop_front();
    }
}

/**
 * @brief Handles the completion of a disk job on a specified disk.
 * @param diskNumber The number of the disk that has completed the job.
 */
void OpSys::DiskJobCompleted(int diskNumber) {
    // get the PID of the process that requested this disk job
    int PID = diskQueues[diskNumber].front().PID;


    // served process goes back to the ready queue
    if(readyQueue.empty()) {
        runningPID = PID;
    }
    else{
        readyQueue.push_back(PID);
    }
}

/**
 * @brief Currently running process wants to access the specified logical memory address.
 * @param address The logical memory address to be accessed.
 * @throws std::logic_error if there isn't any process currently running.
 */
void OpSys::AccessMemoryAddress(unsigned long long address) {
    if (runningPID == 0) throw std::logic_error("CPU is idle"); // There isn't any process currently running

    // Calculate the page number using the formula Shostak gave us
    unsigned long long pageNumber = address / pageSize;

    // Check if the page is already in RAM
    bool pageFound = OpSys::pageFound(runningPID, pageNumber);

    if (!pageFound) {
        OpSys::load_to_RAM(pageNumber);
    }
}

/**
 * @brief Checks whether or not a process was found in memory
 * @param targetPID the running process
 * @param pageNumber the pageNumber
 * @param memoryUsage vector containing the memory being used for every process
 * @return pageFound boolean flag
 */
bool OpSys::pageFound(int targetPID, unsigned long long int pageNumber) {
    bool pageFound{false};
    for (auto &memoryItem: memoryUsage) {
        if (memoryItem.PID == targetPID && memoryItem.pageNumber == pageNumber) {
            pageFound = true;

            // set the last accessed of every item to now
            memoryItem.lastAccessed = std::chrono::steady_clock::now();
            break;
        }
    }
    return pageFound;
}

/**
 * @brief loads page into RAM, checks if there's enough RAM
 * @param pageNumber
 */
void OpSys::load_to_RAM(int pageNumber) {
    // Check if there's enough RAM left or if we need to replace a page
    unsigned long long totalSize = memoryUsage.size() * pageSize;
    if (totalSize >= amountOfRAM) {
        // implement the Least Recently Used (LRU) page replacement algorithm
        auto index = std::min_element(memoryUsage.begin(),
                                      memoryUsage.end(),
                                      [](const MemoryItem& a,
                                         const MemoryItem& b)
                                      { return a.lastAccessed < b.lastAccessed; });
        memoryUsage.erase(index);
    }

    // create a new page & add it to memory
    MemoryItem newPage;
    newPage.PID = runningPID;
    newPage.pageNumber = pageNumber;

    // update std::chrono variable and set it to current time
    newPage.lastAccessed = std::chrono::steady_clock::now();
    memoryUsage.push_back(newPage);
}

/**
 * @brief Returns the PID of the process currently using the CPU.
 * @return PID of the process currently using the CPU. If CPU is idle, returns NO_PROCESS.
 */
int OpSys::GetCPU() {
    if(runningPID==0)
        return NO_PROCESS;
    return runningPID;
}

/**
 * @brief Returns the std::deque with PIDs of processes in the ready-queue.
 * @return std::deque<int> of PIDs of processes in the ready-queue.
 */
std::deque<int> OpSys::GetReadyQueue() {
    return readyQueue;
}

/**
 * @brief Returns MemoryUsage vector describing all currently used frames of RAM.
 * @return MemoryUsage vector of currently used frames of RAM.
 */
MemoryUsage OpSys::GetMemory() {
    return memoryUsage;
}

/**
 * @brief Returns an object with PID of the process served by specified disk and the name of the file read for that process.
 * @param diskNumber The number of the disk.
 * @return FileReadRequest object with the PID and file name of the served process. If the disk is idle, returns the default FileReadRequest object.
 * @throws std::out_of_range if the disk number is invalid.
 */
FileReadRequest OpSys::GetDisk(int diskNumber) {
    FileReadRequest request;
    if (diskNumber >= 0 && diskNumber < numberOfDisks && !diskQueues[diskNumber].empty()) {
        request = diskQueues[diskNumber].front();
        diskQueues[diskNumber].pop_back();
        return request;
    }
    else{
        return request;

    }
}

/**
 * @brief Returns the I/O-queue of the specified disk starting from the “next to be served” process.
 * @param diskNumber The number of the disk.
 * @return std::deque<FileReadRequest> of the specified disk's I/O-queue.
 * @throws std::out_of_range if the disk number is invalid.
 */
std::deque<FileReadRequest> OpSys::GetDiskQueue(int diskNumber) {
    if (diskNumber >= 0 && diskNumber < numberOfDisks)
        return diskQueues[diskNumber];
    else
        throw std::out_of_range("Invalid disk"); // invalid disk number
}
