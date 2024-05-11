// scheduler.h
//	Data structures for the thread dispatcher and scheduler.
//	Primarily, the list of threads that are ready to run.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "copyright.h"
#include "list.h"
#include "thread.h"

// The following class defines the scheduler/dispatcher abstraction --
// the data structures and operations needed to keep track of which
// thread is running, and which threads are ready but not running.

// shihtl> 現在是哪種模式蠻重要的，主要是要判斷 RR
enum SchedulerType {
    RR,    // Round Robin
    SJF,
    Priority
};

class Scheduler {
  public:
    Scheduler();     // Initialize list of ready threads
    ~Scheduler();    // De-allocate ready list

    void ReadyToRun(Thread* thread);
    // Thread can be dispatched.
    Thread* FindNextToRun();    // Dequeue first thread on the ready
                                // list, if any, and return thread.
    void Run(Thread* nextThread, bool finishing);
    // Cause nextThread to start running
    void CheckToBeDestroyed();    // Check if thread that had been
                                  // running needs to be deleted
    void Print();                 // Print contents of ready list

    // shihtl> 為了讓 alarm.cc 知道現在是不是 RR 特別寫的，不確定有沒有其他地方其實可以寫
    SchedulerType getSchedulerType() {
        return schedulerType;
    };

    //<REPORT>
    void UpdatePriority();
    //<REPORT>

    // SelfTest for scheduler is implemented in class Thread

  private:
    SchedulerType schedulerType;
    Thread* toBeDestroyed;    // finishing thread to be destroyed
                              // by the next thread that runs

    //<REPORT>
    // shihtl> 啊他們就三層不同策略的 queue 啊，有需要寫啥嗎
    SortedList<Thread*>* L1ReadyQueue;
    SortedList<Thread*>* L2ReadyQueue;
    List<Thread*>* L3ReadyQueue;
    //<REPORT>
};

#endif    // SCHEDULER_H
