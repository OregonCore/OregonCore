/*
    Cross-platform thread handling
    Copyright (C) 2005 Andrew Zabolotny

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef MTHREAD_H
#define MTHREAD_H

#include "Base.h"
#ifndef WIN32
#include <pthread.h>
#else
#include <windows.h>
//#include "Process.h"
#define WIN32_THREAD_STACK_SIZE 0x10000
#endif

enum ThreadPriority
{
    IDLE,
    LOWER,
    LOW,
    NORMAL,
    HIGH,
    HIGHER,
    REALTIME
};

class MThread: public Base
{
    public:
        static MThread *Start (void (*routine) (void *arg), void *arg);
        MThread ();
        ~MThread ();
        bool SetPriority (ThreadPriority prio);

        void (*routine) (void *arg);
        void *arg;

    #ifdef WIN32
        HANDLE th;
        ULONG id;
    #else
        pthread_t tid;
    #endif

};

class MMutex :  public Base
{
    public:

    #ifdef WIN32
        HANDLE sem;
    #else
        pthread_mutex_t mutex;
        static pthread_mutexattr_t attr;
        static int attr_refcount;
    #endif
        static MMutex *Create ();
        MMutex ();
        virtual ~MMutex ();
        virtual bool Lock ();
        virtual bool TryLock ();
        virtual void Unlock ();
};
#endif                                                      // MTHREAD_H

