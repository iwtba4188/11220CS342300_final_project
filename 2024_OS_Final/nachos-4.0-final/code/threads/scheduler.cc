// scheduler.cc
//	Routines to choose the next thread to run, and to dispatch to
//	that thread.
//
// 	These routines assume that interrupts are already disabled.
//	If interrupts are disabled, we can assume mutual exclusion
//	(since we are on a uniprocessor).
//
// 	NOTE: We can't use Locks to provide mutual exclusion here, since
// 	if we needed to wait for a lock, and the lock was busy, we would
//	end up calling FindNextToRun(), and that would put us in an
//	infinite loop.
//
// 	Very simple implementation -- no priorities, straight FIFO.
//	Might need to be improved in later assignments.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "debug.h"
#include "scheduler.h"
#include "list.h"    // shihtl> 不知道實際執行時會不會循環引入，但不加 vscode 抓不到 QQ
#include "main.h"

//----------------------------------------------------------------------
// Scheduler::Scheduler
// 	Initialize the list of ready but not running threads.
//	Initially, no ready threads.
//----------------------------------------------------------------------

// shihtl> NOTE: 放東西到 queue 裡面都是 push back，拿東西都是 remove front

//<TODO> Done
// Declare sorting rule of SortedList for L1 & L2 ReadyQueue
// Hint: Funtion Type should be "static int"
static int cmpL1(Thread* x, Thread* y) {
    // shihtl> L1 是 preemptive remaining time first
    if (x->getRemainingBurstTime() < y->getRemainingBurstTime()) return -1;
    else if (x->getRemainingBurstTime() == y->getRemainingBurstTime() && x->getPriority() > y->getPriority()) return -1;
    else return 1;
}

static int cmpL2(Thread* x, Thread* y) {
    // shihtl> L2 是 FCFS，看 threadID 決定順序，越少越優先執行
    if (x->getID() < y->getID()) return -1;
    // shihtl> thread id 不可能相等，則 else 是指 x->getID() > y->getID()
    else return 1;
}
//<TODO> Done

Scheduler::Scheduler() {
    //	schedulerType = type;
    // readyList = new List<Thread *>;
    //<TODO> Done
    // Initialize L1, L2, L3 ReadyQueue
    L1ReadyQueue = new SortedList<Thread*>(cmpL1);
    L2ReadyQueue = new SortedList<Thread*>(cmpL2);
    L3ReadyQueue = new List<Thread*>();
    //<TODO> Done
    toBeDestroyed = NULL;
}

//----------------------------------------------------------------------
// Scheduler::~Scheduler
// 	De-allocate the list of ready threads.
//----------------------------------------------------------------------

Scheduler::~Scheduler() {
    //<TODO> Done
    // Remove L1, L2, L3 ReadyQueue
    delete L1ReadyQueue;
    delete L2ReadyQueue;
    delete L3ReadyQueue;
    //<TODO> Done
    // delete readyList;
}

//----------------------------------------------------------------------
// Scheduler::ReadyToRun
// 	Mark a thread as ready, but not running.
//	Put it on the ready list, for later scheduling onto the CPU.
//
//	"thread" is the thread to be put on the ready list.
//----------------------------------------------------------------------

void Scheduler::ReadyToRun(Thread* thread) {
    ASSERT(kernel->interrupt->getLevel() == IntOff);
    // DEBUG(dbgThread, "Putting thread on ready list: " << thread->getName());

    Statistics* stats = kernel->stats;
    //<TODO> Done 大概啦
    // According to priority of Thread, put them into corresponding ReadyQueue.
    // After inserting Thread into ReadyQueue, don't forget to reset some values.
    // TODO
    // shihtl> 到底是需要 reset 哪些東西QQ
    // Hint: L1 ReadyQueue is preemptive SRTN(Shortest Remaining Time Next).
    // When putting a new thread into L1 ReadyQueue, you need to check whether preemption or not.
    // shihtl> Note that insert to a SortedList will insert an item onto the list in sorted order.

    int levelInsert = 0;
    thread->setStatus(READY);

    if (thread->getPriority() >= 100) {    // shihtl> L1 ReadyQueue
        levelInsert = 1;
        L1ReadyQueue->Insert(thread);
    } else if (thread->getPriority() >= 50) {    // shihtl> L2 ReadyQueue
        levelInsert = 2;
        L2ReadyQueue->Insert(thread);
    } else {    // shihtl> L3 ReadyQueue
        levelInsert = 3;
        L3ReadyQueue->Append(thread);
    }
    // shihtl> 2.3.(a)
    DEBUG(dbgMLFQ, "[InsertToQueue] Tick[" << kernel->stats->totalTicks << "]: Thread[" << thread->getID() << "] is inserted into queue L[" << levelInsert << "]");

    // shihtl> 不要瘋到連一開始的都搶
    if (levelInsert == 1 && kernel->currentThread->getID() != 0 &&
        (thread->getRemainingBurstTime() < kernel->currentThread->getRemainingBurstTime() || thread->getPriority() > kernel->currentThread->getPriority())) {    // shihtl> preemption occur
        // kernel->currentThread->setStatus(READY);
        // L1ReadyQueue->Insert(kernel->currentThread);
        // Run(thread, false);    // shihtl > 搶到了開始跑，但還沒跑完
        // DEBUG('z', "&^&^&^&^&^ Ready to Run Preemptive!");
        // kernel->currentThread->Yield();    // shihtl> 後來發現有 Yeild 可以用
        kernel->interrupt->YieldOnReturn();
    }
    //<TODO> Done 大概啦
    // readyList->Append(thread);
}

//----------------------------------------------------------------------
// Scheduler::FindNextToRun
// 	Return the next thread to be scheduled onto the CPU.
//	If there are no ready threads, return NULL.
// Side effect:
//	Thread is removed from the ready list.
//----------------------------------------------------------------------

Thread* Scheduler::FindNextToRun() {
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    /*if (readyList->IsEmpty()) {
    return NULL;
    } else {
        return readyList->RemoveFront();
    }*/

    //<TODO> Done
    // a.k.a. Find Next (Thread in ReadyQueue) to Run
    Thread* NextThread = NULL;
    // shihtl> 先找 L1
    if (!L1ReadyQueue->IsEmpty()) {
        NextThread = L1ReadyQueue->Front();
        L1ReadyQueue->RemoveFront();
        schedulerType = SJF;

        // shihtl> 2.3.(b)
        DEBUG(dbgMLFQ, "[RemoveFromQueue] Tick [" << kernel->stats->totalTicks << "]: Thread [" << NextThread->getID() << "] is removed from queue L[1]");
    }
    // shihtl> 再找 L2
    else if (!L2ReadyQueue->IsEmpty()) {
        NextThread = L2ReadyQueue->Front();
        L2ReadyQueue->RemoveFront();
        schedulerType = Priority;    // shihtl> 雖然好像說他是用 Priority 好像哪裡怪怪的，但就這樣吧

        // shihtl> 2.3.(b)
        DEBUG(dbgMLFQ, "[RemoveFromQueue] Tick [" << kernel->stats->totalTicks << "]: Thread [" << NextThread->getID() << "] is removed from queue L[2]");
    }
    // shihtl> 最後是 L3
    else if (!L3ReadyQueue->IsEmpty()) {
        NextThread = L3ReadyQueue->Front();
        L3ReadyQueue->RemoveFront();
        schedulerType = RR;

        // shihtl> 2.3.(b)
        DEBUG(dbgMLFQ, "[RemoveFromQueue] Tick [" << kernel->stats->totalTicks << "]: Thread [" << NextThread->getID() << "] is removed from queue L[3]");
    }

    return NextThread;
    //<TODO> Done
}

//----------------------------------------------------------------------
// Scheduler::Run
// 	Dispatch the CPU to nextThread.  Save the state of the old thread,
//	and load the state of the new thread, by calling the machine
//	dependent context switch routine, SWITCH.
//
//      Note: we assume the state of the previously running thread has
//	already been changed from running to blocked or ready (depending).
// shihtl> 這邊 Note 重要!! 已經預設在跑新的 thread 時，舊的 thread 已經被撈到 ready 或是 blocked
// Side effect:
//	The global variable kernel->currentThread becomes nextThread.
//
//	"nextThread" is the thread to be put into the CPU.
//	"finishing" is set if the current thread is to be deleted
//		once we're no longer running on its stack
//		(when the next thread starts running)
//----------------------------------------------------------------------

void Scheduler::Run(Thread* nextThread, bool finishing) {
    Thread* oldThread = kernel->currentThread;

    //	cout << "Current Thread" <<oldThread->getName() << "    Next Thread"<<nextThread->getName()<<endl;

    ASSERT(kernel->interrupt->getLevel() == IntOff);

    // // shihtl> 這裡有改，新增判斷有沒有運行完畢
    // if (finishing || (oldThread->getRemainingBurstTime() <= 0)) {    // mark that we need to delete current thread
    if (finishing) {    // mark that we need to delete current thread
        ASSERT(toBeDestroyed == NULL);
        toBeDestroyed = oldThread;
    }

#ifdef USER_PROGRAM                    // ignore until running user programs
    if (oldThread->space != NULL) {    // if this thread is a user program,

        oldThread->SaveUserState();    // save the user's CPU registers
        oldThread->space->SaveState();
    }
#endif

    oldThread->CheckOverflow();    // check if the old thread
                                   // had an undetected stack overflow

    kernel->currentThread = nextThread;    // switch to the next thread
    nextThread->setStatus(RUNNING);        // nextThread is now running

    // DEBUG(dbgThread, "Switching from: " << oldThread->getName() << " to: " << nextThread->getName());

    // This is a machine-dependent assembly language routine defined
    // in switch.s.  You may have to think
    // a bit to figure out what happens after this, both from the point
    // of view of the thread and from the perspective of the "outside world".

    cout << "Switching from: " << oldThread->getID() << " to: " << nextThread->getID() << endl;
    SWITCH(oldThread, nextThread);

    // we're back, running oldThread

    // interrupts are off when we return from switch!
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    DEBUG(dbgThread, "Now in thread: " << kernel->currentThread->getID());

    // // shihtl> 2.3.(e)
    // DEBUG(dbgMLFQ, "[ContextSwitch] Tick [" << kernel->stats->totalTicks << "]: Thread [" << nextThread->getID() << "] is now selected for execution, thread [" << oldThread->getID()
    //                                         << "] is replaced, and it has executed [" << oldThread->getRunTime() << "] ticks");

    // shihtl> 把舊的那個跑完的 thread 嘎了
    CheckToBeDestroyed();    // check if thread we were running
                             // before this one has finished
                             // and needs to be cleaned up

#ifdef USER_PROGRAM
    if (oldThread->space != NULL) {       // if there is an address space
        oldThread->RestoreUserState();    // to restore, do it.
        oldThread->space->RestoreState();
    }
#endif
}

//----------------------------------------------------------------------
// Scheduler::CheckToBeDestroyed
// 	If the old thread gave up the processor because it was finishing,
// 	we need to delete its carcass.  Note we cannot delete the thread
// 	before now (for example, in Thread::Finish()), because up to this
// 	point, we were still running on the old thread's stack!
//----------------------------------------------------------------------

void Scheduler::CheckToBeDestroyed() {
    if (toBeDestroyed != NULL) {
        DEBUG(dbgThread, "toBeDestroyed->getID(): " << toBeDestroyed->getID());
        delete toBeDestroyed;
        toBeDestroyed = NULL;
    }
}

//----------------------------------------------------------------------
// Scheduler::Print
// 	Print the scheduler state -- in other words, the contents of
//	the ready list.  For debugging.
//----------------------------------------------------------------------
void Scheduler::Print() {
    cout << "Ready list contents:\n";
    // readyList->Apply(ThreadPrint);
    L1ReadyQueue->Apply(ThreadPrint);
    L2ReadyQueue->Apply(ThreadPrint);
    L3ReadyQueue->Apply(ThreadPrint);
}

// <TODO> Done

// Function 1. Function definition of sorting rule of L1 ReadyQueue
// shihtl> 最上面不是已經有了嗎

// Function 2. Function definition of sorting rule of L2 ReadyQueue
// shihtl> 最上面不是已經有了嗎

// Function 3. Scheduler::UpdatePriority()
// Hint:
// 1. ListIterator can help.
// 2. Update WaitTime and priority in Aging situations
// 3. After aging, Thread may insert to different ReadyQueue

// shihtl> !!!??? 根據 Alarm::CallBack() 猜測每 100 ticks 執行一次
void Scheduler::UpdatePriority() {

    // shihtl> L1 就算 Priority 爆表也不會影響到 threads 執行的順序，所以可以不用管他（0613更: 還是管一下好了）
    ListIterator<Thread*> L1Iterator(L1ReadyQueue);
    while (!L1Iterator.IsDone()) {
        Thread* thisThread = L1Iterator.Item();
        thisThread->setWaitTime(thisThread->getWaitTime() + 100);

        // shihtl> aging
        if (thisThread->getWaitTime() > 400) {
            // shihtl> 2.3.(c)
            DEBUG(dbgMLFQ, "[UpdatePriority] Tick[" << kernel->stats->totalTicks << "]: Thread[" << thisThread->getID() << "] changes its priority from [" << thisThread->getPriority() << "] to ["
                                                    << min(thisThread->getPriority() + 10, 149) << "]");
            thisThread->setPriority(min(thisThread->getPriority() + 10, 149));    // shihtl> 怕爆優先級
            thisThread->setWaitTime(thisThread->getWaitTime() - 400);
        }

        L1Iterator.Next();
    }

    // shihtl> 所以從 L2 開始更新 Priority
    ListIterator<Thread*> L2Iterator(L2ReadyQueue);
    while (!L2Iterator.IsDone()) {
        Thread* thisThread = L2Iterator.Item();
        thisThread->setWaitTime(thisThread->getWaitTime() + 100);

        // shihtl> aging
        if (thisThread->getWaitTime() > 400) {
            // shihtl> 2.3.(c)
            DEBUG(dbgMLFQ, "[UpdatePriority] Tick[" << kernel->stats->totalTicks << "]: Thread[" << thisThread->getID() << "] changes its priority from [" << thisThread->getPriority() << "] to ["
                                                    << thisThread->getPriority() + 10 << "]");
            thisThread->setPriority(thisThread->getPriority() + 10);
            thisThread->setWaitTime(thisThread->getWaitTime() - 400);

            // shihtl> move to higher priority ready queue if possible
            // shihtl> 這一 part 跟 Scheduler::ReadyToRun() 有 87% 像
            // shihtl> [TEMP REMOVED] START
            // if (thisThread->getPriority() >= 100) {    // shihtl> enter L1 ReadyQueue
            // shihtl> 2.3.(b)
            DEBUG(dbgMLFQ, "[RemoveFromQueue] Tick [" << kernel->stats->totalTicks << "]: Thread [" << thisThread->getID() << "] is removed from queue L[2]");
            L2ReadyQueue->Remove(thisThread);    // shihtl> 和 Scheduler::ReadyToRun() 不一樣的是，要記得把它拿掉
            ReadyToRun(thisThread);
            // }
            // shihtl> [TEMP REMOVED] END
        }

        L2Iterator.Next();
    }

    // shihtl> 換處理 L3 的部分
    ListIterator<Thread*> L3Iterator(L3ReadyQueue);
    while (!L3Iterator.IsDone()) {
        Thread* thisThread = L3Iterator.Item();
        thisThread->setWaitTime(thisThread->getWaitTime() + 100);

        // shihtl> aging
        if (thisThread->getWaitTime() > 400) {
            // shihtl> 2.3.(c)
            DEBUG(dbgMLFQ, "[UpdatePriority] Tick[" << kernel->stats->totalTicks << "]: Thread[" << thisThread->getID() << "] changes its priority from [" << thisThread->getPriority() << "] to ["
                                                    << thisThread->getPriority() + 10 << "]");
            thisThread->setPriority(thisThread->getPriority() + 10);
            thisThread->setWaitTime(thisThread->getWaitTime() - 400);

            // shihtl> move to higher priority ready queue if possible
            // shihtl> 這一 part 跟 Scheduler::ReadyToRun() 有 87% 像
            // shihtl> [TEMP REMOVED] START
            // if (thisThread->getPriority() >= 50) {    // shihtl> enter L2 ReadyQueue
            // shihtl> 2.3.(b)
            DEBUG(dbgMLFQ, "[RemoveFromQueue] Tick [" << kernel->stats->totalTicks << "]: Thread [" << thisThread->getID() << "] is removed from queue L[3]");
            L3ReadyQueue->Remove(thisThread);    // shihtl> 和 Scheduler::ReadyToRun() 不一樣的是，要記得把它拿掉

            ReadyToRun(thisThread);
            // }
            // shihtl> [TEMP REMOVED] END
        }

        L3Iterator.Next();
    }

    // kernel->scheduler->Print();
}

// <TODO> Done