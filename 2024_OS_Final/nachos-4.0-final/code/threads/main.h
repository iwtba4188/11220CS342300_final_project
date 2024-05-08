// main.h
//	This file defines the Nachos global variables; it
//	will change with later assignments.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef MAIN_H
#define MAIN_H

#include "copyright.h"
#include "debug.h"

#ifdef NETWORK    // THREADS && USER_PROGRAM && NETWORK
#include "netkernel.h"
#define KernelType NetKernel
class NetKernel;
#else
#ifdef USER_PROGRAM    // THREADS && USER_PROGRAM
#include "userkernel.h"
#define KernelType UserProgKernel
class UserProgKernel;
#else
#include "kernel.h"    // THREADS
#define KernelType ThreadedKernel
class ThreadedKernel;
#endif
#endif

extern KernelType* kernel;
extern Debug* debug;

#endif    // MAIN_H
