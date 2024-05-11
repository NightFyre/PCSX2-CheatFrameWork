#pragma once
#include <CDK.h>
#include "lib/PlayStation2-Game-SDKs/SOCOM U.S Navy Seals/SOCOM1_package.h"

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
        g_running = true;

        //  Loop
        do
        {
            //  Exit Module
            if (PlayStation2::Tools::GetKeyState(VK_END, 500))
                g_running = false;

            //  Get Local Player Instance
            PlayStation2::SOCOM::CZSeal* pLocalPlayer = PlayStation2::SOCOM::CZSeal::GetDefaultInstance();
            if (pLocalPlayer)
            {

                //  Obtain Player Array
                if (PlayStation2::Tools::GetKeyState(VK_HOME, 500))
                {

                    DWORD i = 0;
                    PlayStation2::Tools::CPUTimer t;
                    std::vector<PlayStation2::SOCOM::CZSeal*> players;
                    PlayStation2::SOCOM::CZMatchData::GetPlayers(&players);
                    for (auto ent : players)
                    {
                        i++;
                    
                        if (!ent->IsValid())
                            continue;

                        PlayStation2::Console::LogMsg("[CDK][%.3f][SOCOM::GetPlayers][%d/%d][HP: %.0f] %s\n", t.GetElapsedTime(clock()), i, players.size(), ent->GetHealth(), ent->GetName().c_str());
                    
                    }
                 
                    PlayStation2::Console::LogMsg("[CDK][%.3f][SOCOM::GetPlayers] DONE\n\n", t.Stop(PlayStation2::Tools::CPUTimer::ETimings::ETiming_MS));
                }
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