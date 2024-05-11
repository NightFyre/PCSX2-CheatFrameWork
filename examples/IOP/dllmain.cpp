#pragma once
#include <CDK.h>

static bool g_running;

#define fn_start 0x0             //  should be function start address 

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
        ///  Get Debug Registers
        PlayStation2::PCSX2::g_psxRegs = reinterpret_cast<PlayStation2::psxRegisters*>((PlayStation2::Memory::GetAddr(PlayStation2::PCSX2::o_psxRegs) - 0x20C));   //    similar to cpuRegs, the found offset is displaced and must be brought back to origin to access the data.

        /// Reset Recompiler
        PlayStation2::PCSX2::ResetIOP();             //  Reset IOP to re/capture events

        g_running = true;

        //  Loop
        do
        {
            //  Exit Module
            if (PlayStation2::Tools::GetKeyState(VK_END, 500))
                g_running = false;

            //  Recompile IOP , can be used to capture a functions compilation. May need to trigger an event of sorts in game. YMMV
            if (PlayStation2::Tools::GetKeyState(VK_HOME, 500))
                PlayStation2::PCSX2::ResetIOP();

            if (fn_start <= 0)
                continue;

            //  Capture IOP Instruction Interpretation
            if (PlayStation2::PCSX2::g_psxRegs->pc == fn_start)
            {
                using namespace PlayStation2;

                //  Capture Register Data
                const auto pc = PCSX2::g_psxRegs->pc;           //  program counter
                const auto code = PCSX2::g_psxRegs->code;       //  ~
                const auto GPR = PCSX2::g_psxRegs->GPR;         //  registers

                //  Log Data
                Console::cLogMsg("[+] PCSX2::PS2::IOP::fnName()\npc:\t%d\ncode:\t%d\nGPR.sp:\t%d\n\n", 
                    EConsoleColors::yellow,
                    pc,                         //  
                    code,                       //  
                    GPR.sp                      //  
                );
            }

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