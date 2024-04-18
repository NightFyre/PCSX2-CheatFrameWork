#pragma once
#include "pch.h"

static bool g_running;

//---------------------------------------------------------------------------------------------------
//  Initialize Client Thread
DWORD WINAPI MainThread(LPVOID hInstance)
{

#if _DEBUG

    MessageBoxA(NULL, "Initializing PCSX2 Cheat Device", "DEBUG BUILD", MB_OK | MB_ICONINFORMATION);

#endif    

    //  initialize pcsx2 cheat dev kit
    if (PlayStation2::InitCDK())
    {                
        PlayStation2::Tools::CPUTimer t;

        //  -   Get PCSX2 Module Base
        const auto pcsx2ModuleBase = PlayStation2::Memory::GetModuleBase();
        PlayStation2::Console::LogMsg("[+][%.3f][Memory::GetModuleBase] 0x%llX\n", t.GetElapsedTime(clock()), pcsx2ModuleBase);

        //  -   Read 8 bytes from module base address
        long long result = PlayStation2::Memory::ReadMemoryEx<long long>(pcsx2ModuleBase);
        PlayStation2::Console::LogMsg("[+][%.3f][Memory::ReadMemoryEx] 0x%llX\n", t.GetElapsedTime(clock()), result);

        //  -   Get EE Memory Module Base ( ps2 game ram )
        const auto eeModuleBase = PlayStation2::PS2Memory::GetModuleBase();
        PlayStation2::Console::LogMsg("[+][%.3f][PS2Memory::GetModuleBase] 0x%llX\n", t.GetElapsedTime(clock()), eeModuleBase);

        //  -   retrieve an offsets virtual address in memory
        long long offsetAddress = PlayStation2::PS2Memory::GetAddr(0x1000);
        PlayStation2::Console::LogMsg("[+][%.3f][PS2Memory::GetAddr] 0x%llX\n", t.GetElapsedTime(clock()), offsetAddress);

        //  -   read 8 bytes from the offset virtual address obtained in the previous step
        auto read_result_long = PlayStation2::PS2Memory::ReadLong<long long>(offsetAddress);
        PlayStation2::Console::LogMsg("[+][%.3f][PS2Memory::ReadLong] 0x%llX\n", t.GetElapsedTime(clock()), read_result_long);

        //  -   read 8 bytes from a virtual address in memory using a games offset
        auto read_result_short = PlayStation2::PS2Memory::ReadShort<__int32>(0x2000);
        PlayStation2::Console::LogMsg("[+][%.3f][PS2Memory::ReadShort] 0x%llX\n", t.GetElapsedTime(clock()), read_result_short);


        //  -   Obtain basic process information such as PID, handle, base address & window information
        PlayStation2::ProcessInfo pInfo;
        PlayStation2::Memory::ObtainProcessInfo(pInfo);
        PlayStation2::Console::LogMsg("[+][%.3f][Memory::ObtainProcessInfo]\n- PID:\t\t0x%X\n- PATH:\t\t%s\n- HWND:\t\t0x%X\n- TITLE:\t%s\n\n",
            t.Stop(PlayStation2::Tools::CPUTimer::ETimings::ETiming_MS),
            pInfo.m_ModulePID,                              //  
            pInfo.m_ModulePath.c_str(),                     //  
            pInfo.m_GameWindow,                             //  Might be console window
            pInfo.m_WindowTitle.c_str()                     //  Might be console window title
        );

        g_running = true;

        //  Loop
        do
        {
            //  Exit Module
            if (PlayStation2::Tools::GetKeyState(VK_END, 500))
                g_running = false;

        } while (g_running);
        
        //  cleanup
        PlayStation2::ShutdownCDK(); 
    }

    //  Exit
    FreeLibraryAndExitThread((HMODULE)hInstance, EXIT_SUCCESS);
    
    return EXIT_SUCCESS;
}

//---------------------------------------------------------------------------------------------------
//  EntryPoint
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  dwCallReason, LPVOID x)
{
    UNREFERENCED_PARAMETER(x);

    if (dwCallReason == DLL_PROCESS_ATTACH)
    {

        DisableThreadLibraryCalls(hModule);

        HANDLE pHand = CreateThread(0, 0, MainThread, hModule, 0, 0);

        if (pHand)
            CloseHandle(pHand);

    }

    return TRUE;
}