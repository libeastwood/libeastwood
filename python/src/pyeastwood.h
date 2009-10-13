#ifndef EASTWOOD_PYTHON_PYEASTWOOD_H
#define EASTWOOD_PYTHON_PYEASTWOOD_H

#include <Python.h>

#ifdef WITH_THREAD
#include <pythread.h>
#define ACQUIRE_LOCK(obj) PyThread_acquire_lock(obj->lock, 1)
#define RELEASE_LOCK(obj) PyThread_release_lock(obj->lock)
#else
#define ACQUIRE_LOCK(obj)
#define RELEASE_LOCK(obj)
#endif

#endif // EASTWOOD_PYTHON_PYEASTWOOD_H

