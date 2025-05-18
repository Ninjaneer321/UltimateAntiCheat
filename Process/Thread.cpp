//By AlSch092 @ Github
#include "Thread.hpp"

/*
    Thread::BeginExecution - Managed thread creation & start
    return `true` on success, `false` on failure
*/
bool Thread::BeginExecution(LPTHREAD_START_ROUTINE toExecute, LPVOID lpOptionalParam, bool shouldRunForever, bool shouldDetach)
{
    if (toExecute != NULL)
    {
        this->ExecutionAddress = (DWORD_PTR)toExecute;
        this->OptionalParam = lpOptionalParam;
        this->ShouldRunForever = shouldRunForever;

        try
        {
            // Use a lambda to wrap the LPTHREAD_START_ROUTINE for std::thread
            this->t = std::thread([toExecute, lpOptionalParam]() { toExecute(lpOptionalParam); });

            this->handle = t.native_handle(); // Get the native handle
            this->Id = GetThreadId(this->handle); //the std::thread's id won't be the same as windows thread ids, so use the native handle to get the tid which we can use with winapi

            if (shouldDetach)
                t.detach();

            Tick = std::chrono::steady_clock::now();

            return true;
        }
        catch (const std::system_error& e)
        {
            Logger::logf(Err, "std::thread failed @ BeginExecution: %s\n", e.what());
            this->Id = NULL;
            return false;
        }
    }
    else
    {
        return false;
    }
}
bool Thread::IsThreadRunning(HANDLE threadHandle)
{
    if (threadHandle == NULL)
        return false;

    DWORD exitCode;

    if (GetExitCodeThread(threadHandle, &exitCode) != 0)
    {
        return (exitCode == STILL_ACTIVE);
    }

    Logger::logf(Err, " GetExitCodeThread failed @ IsThreadRunning: %d\n", GetLastError());
    return false;
}

/*
    IsThreadSuspended - checks if a thread is currently suspended by suspending it
    has its drawbacks since it suspends threads very briefly, but it works fine
*/
bool Thread::IsThreadSuspended(DWORD tid)
{
    if (tid == 0)
        return false;

    bool suspended = false;
    DWORD suspendCount = 0;

    HANDLE threadHandle = OpenThread(THREAD_SUSPEND_RESUME, FALSE, tid);

    if (threadHandle == INVALID_HANDLE_VALUE || threadHandle == 0)
        return false;

    __try
    {
        suspendCount = SuspendThread(threadHandle); //warning: invalid handle will throw exception here
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }

    if (suspendCount == (DWORD)-1)
    {
        return false;
    }
    else if (suspendCount > 0) //already suspended by someone else
    {
        ResumeThread(threadHandle);
        suspended = true;
    }
    else
    {
        ResumeThread(threadHandle);
    }

    return suspended;
}