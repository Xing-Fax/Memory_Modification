#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>
#include<TlHelp32.h>

/// <summary>
/// 根据进程ID得到进程窗口句柄
/// </summary>
/// <param name="nID">进程PID</param>
/// <returns>返回窗口句柄(十六进制)</returns>
HANDLE GetProcessHandle(int nID)
{
    return OpenProcess(PROCESS_ALL_ACCESS, FALSE, nID);
}

/// <summary>
/// 根据窗口名称得到进程ID
/// </summary>
/// <param name="procName">窗口名称</param>
/// <returns>返回进程PID(十进制)</returns>
DWORD GetProcId(const char* procName)
{
    DWORD procId = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);
        if (Process32First(hSnap, &procEntry))
        {
            do
            {
                if (lstrcmpi(procEntry.szExeFile, procName) == 0) {
                    procId = procEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnap, &procEntry));
        }
    }
    CloseHandle(hSnap);
    return procId;
}

/// <summary>
/// 通过进程ID和窗口名称得到基地址
/// </summary>
/// <param name="procId">进程ID</param>
/// <param name="modName">窗口名称</param>
/// <returns>返回基地址(十六进制)</returns>
uintptr_t GetModuleBaseAddress(DWORD procId, const char* modName)
{
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, procId);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry))
        {
            do
            {
                if (!_stricmp(modEntry.szModule, modName))
                {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    else {
        std::cout << "INVALID_HANDLE_VALUE returned" << std::endl;
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}

int main()
{
    system("start Minecraft:");

    DWORD procId = GetProcId("Minecraft.Windows.exe");

    std::cout << "进程ID= " << procId << std::endl;

    uintptr_t baseAddr = GetModuleBaseAddress(procId, "Minecraft.Windows.exe");

    std::cout << "基地址= " << baseAddr << std::endl;

    HANDLE handle = GetProcessHandle(procId);

    std::cout << "句  柄= " << handle << std::endl;
    //写入内容
    char Buffer[1] = { 0xEB };
    //偏移量
    int Offset = 18206166;

    WriteProcessMemory(handle, LPVOID(baseAddr + Offset), Buffer, 1ui64, 0i64);

    return 0;
}