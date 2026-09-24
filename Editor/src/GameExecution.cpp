#include "GameExecution.h"

namespace clc
{
    bool ExecutionSystem::Initialize()
    {
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        mProgramPath = L"..\\bin\\Debug\\Spinblade.exe";

        return true;
    }
    void ExecutionSystem::Shutdown()
    {
        if (mExecuting)
            StopExecution();
    }

    bool ExecutionSystem::IsExecuting() const { return mExecuting; }

    void ExecutionSystem::StartExecution()
    {
        CreateProcess(mProgramPath.c_str(), 0, 0, 0, 0, 0, 0, 0, &si, &pi);

        mExecuting = true;
    }

    void ExecutionSystem::StopExecution()
    {
        CloseHandle(pi.hThread);
        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);

        mExecuting = false;
    }
}