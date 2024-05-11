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
        PlayStation2::PCSX2::g_cpuRegs = reinterpret_cast<PlayStation2::cpuRegisters*>((PlayStation2::Memory::GetAddr(PlayStation2::PCSX2::o_cpuRegs) - 0x2AC));    //  [0x2AC is g_cpuRegs.code] The offset for cpuRegs found in recompileNextInstruction is displaced to the code offset in the structure
        
        /// Reset Recompiler
        PlayStation2::PCSX2::ResetEE();             //  Reset EE so that we can re/capture events

        g_running = true;

        //  Loop
        do
        {
            //  Exit Module
            if (PlayStation2::Tools::GetKeyState(VK_END, 500))
                g_running = false;
         
            //  if (PlayStation2::PCSX2::o_recResetEE <= 0)
            //      continue;

            //  Recompile EE , can be used to capture a functions compilation. May need to trigger an event of sorts in game. YMMV
            if (PlayStation2::Tools::GetKeyState(VK_HOME, 500))
                PlayStation2::PCSX2::ResetEE();

            if (fn_start <= 0)
                continue;

            //  Capture EE Instruction Interpretation
            if (PlayStation2::PCSX2::g_cpuRegs->pc == fn_start)
            {
                using namespace PlayStation2;

                //  Capture Register Data
                const auto pc = PCSX2::g_cpuRegs->pc;           //  program counter
                const auto code = PCSX2::g_cpuRegs->code;       //  ~
                const auto GPR = PCSX2::g_cpuRegs->GPR;         //  registers

                //  Log Data
                Console::cLogMsg("[+] PCSX2::PS2::EE::fnName()\npc:\t%d\ncode:\t%d\nGPR.sp:\t%d\n\n",
                    EConsoleColors::yellow,
                    pc,                         //  
                    code,                       //  
                    GPR.sp.n[0]                 //  
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