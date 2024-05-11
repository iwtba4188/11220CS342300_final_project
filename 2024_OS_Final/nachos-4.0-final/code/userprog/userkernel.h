// userkernel.h
//	Global variables for the Nachos kernel, for the assignment
//	supporting running user programs.
//
//	The kernel supporting user programs is a version of the
//	basic multithreaded kernel.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef USERKERNEL_H
#define USERKERNEL_H

#include "kernel.h"
#include "filesys.h"
#include "machine.h"
#include "synchdisk.h"

class SynchConsoleInput;
class SynchConsoleOutput;
class SynchDisk;
class UserProgKernel : public ThreadedKernel {
  public:
    UserProgKernel(int argc, char** argv);
    // Interpret command line arguments
    ~UserProgKernel();    // deallocate the kernel

    void Initialize();    // initialize the kernel

    void Run();    // do kernel stuff

    void SelfTest();    // test whether kernel is working

    //<REPORT> Done
    // shihtl> 1-1: 初始化所有在 `exefile` 裡面的 threads
    void InitializeAllThreads();

    // shihtl> 我猜 synchConsoleIn/Out 可以不用寫 comment
    SynchConsoleInput* synchConsoleIn;
    // shihtl> 底層調用 `sprintf` 把 int 變成 char *，然後讓輸出 interrupt 去排隊輸出到 console
    SynchConsoleOutput* synchConsoleOut;

    // shihtl> 將對應的 thread 初始化
    int InitializeOneThread(char* name, int priority, int burst_time);
    //<REPORT> Done


    bool PhysicalPageUsed[NumPhysPages];
    int availablePhysicalPages() {
        int availablePageCount = 0;
        for (int i = 0; i < NumPhysPages; i++) {
            if (PhysicalPageUsed[i] == FALSE) availablePageCount++;
        }
        return availablePageCount;
    }
    Thread* getThread(int threadID) {
        return t[threadID];
    }

    // These are public for notational convenience.
    Machine* machine;
    FileSystem* fileSystem;


#ifdef FILESYS
    SynchDisk* synchDisk;
#endif    // FILESYS

  private:
    bool debugUserProg;    // single step user program
    Thread* t[30];
    char* execfile[30];

    //<REPORT> Done
    // shihtl> 各 index 的 thread 對應的 priority
    int threadPriority[30];
    // shihtl> 各 index 的 thread 對應剩下的 burst_time
    int threadRemainingBurstTime[30];

    // shihtl> 總共有多少 thread，在這份作業裡面基本上會跟 execfileNum 一樣
    int threadNum;

    // shihtl> 總共有多少要呼叫的 file
    int execfileNum;
    //<REPORT> Done
    char* consoleIn;
    char* consoleOut;
};

#endif    // USERKERNEL_H
