#pragma once
#include "../pch.h"

/**
 * Name: PlayStation2 - PCSX2
 * Version: 1.0.0
 * Author: NightFyre
*/

namespace PlayStation2
{
    //----------------------------------------------------------------------------------------------------
    //										CORE
    //-----------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------------------------
    bool InitSDK(const std::string& moduleName, unsigned int gDevice)
    {
        bool result{ false };

        HANDLE pHand = GetModuleHandleA(moduleName.c_str());
        if (!pHand)
            return result;

        CGlobals::g_gs_device = reinterpret_cast<GSDevice*>(*(__int64*)(Memory::GetAddr(gDevice)));
        if (!CGlobals::g_gs_device)
            return false;

        CGlobals::g_console = Console::GetDefaultInstance();
        CGlobals::g_engine = Engine::GetDefaultInstance();
        CGlobals::g_memory = Memory::GetDefaultInstance();

        Console::LogMsg("[+] PCSX2 Framework Client Initialized!\nmodBase:\t0x%llX\nPS2ModBase:\t0x%llX\ng_gs_device:\t0x%llX\nRenderAPI:\t%d\n", 
            (__int64)pHand,
            PS2Memory::GetModuleBase(), 
            CGlobals::g_gs_device, 
            GSDevice::GetRenderAPI()
        );
        return true;
    }

    ///---------------------------------------------------------------------------------------------------
    // Template Initialization function
    //  NOTE: offsets will not always be correct
    //  NOTE2: PCSX2 v1.7.5617
    bool InitSDK() { return InitSDK("pcsx2-qt.exe", PCSX2::o_gs_device); }

    ///---------------------------------------------------------------------------------------------------
    void ShutdownSDK() { }

    //----------------------------------------------------------------------------------------------------
    //										CGLOBALS
    //-----------------------------------------------------------------------------------

#pragma region  //  CGLOBALS

    //----------------------------------------------------------------------------------------------------
    //  STATICS
    GSRenderer*             CGlobals::g_gs_renderer;
    GSDevice*               CGlobals::g_gs_device;
    EmuThread*              CGlobals::g_emu_thread;
    Console*                CGlobals::g_console;
    Engine*                 CGlobals::g_engine;
    Memory*                 CGlobals::g_memory;

#pragma endregion

    //----------------------------------------------------------------------------------------------------
    //										CONSOLE
    //-----------------------------------------------------------------------------------

#pragma region  //  CONSOLE

    ///---------------------------------------------------------------------------------------------------
    //  STATICS
    FILE*                   Console::m_pInStream;
    FILE*                   Console::m_pOutStream;
    FILE*                   Console::m_pErrStream;
    HANDLE                  Console::m_pHandle;
    HWND	                Console::m_pWndw;
    HANDLE                  Console::m_pPipe;
    bool                    Console::m_isConsoleAllocated{ false };
    bool                    Console::m_isVisible{ true };
    std::mutex              Console::m_mutex;
    Console*                Console::m_instance = new Console();

    ///---------------------------------------------------------------------------------------------------
    Console::Console()
    {
        if (m_isConsoleAllocated)
            return;

        AllocConsole();														//  Allocate console for output
        m_pHandle = GetStdHandle(STD_OUTPUT_HANDLE);					    //  Store handle to console
        m_pWndw = GetConsoleWindow();									    //  Store WindowHandle to console
        freopen_s(&m_pInStream, "CONIN$", "r", stdin);	                    //  Establish input stream
        freopen_s(&m_pOutStream, "CONOUT$", "w", stdout);	                //  Establish ouput stream
        freopen_s(&m_pErrStream, "CONOUT$", "w", stderr);	                //  Establish error stream
        ShowWindow(m_pWndw, m_isVisible ? SW_SHOW : SW_HIDE);		        //	Show console window based on visible state
        m_isConsoleAllocated = true;
    }

    ///---------------------------------------------------------------------------------------------------
    Console::Console(const char* title)
    {
        if (m_isConsoleAllocated)
            return;

        AllocConsole();														//  Allocate console for output
        m_pHandle = GetStdHandle(STD_OUTPUT_HANDLE);					    //  Store handle to console
        m_pWndw = GetConsoleWindow();									    //  Store WindowHandle to console
        freopen_s(&m_pInStream, "CONIN$", "r", stdin);	                    //  Establish input stream
        freopen_s(&m_pOutStream, "CONOUT$", "w", stdout);	                //  Establish ouput stream
        freopen_s(&m_pErrStream, "CONOUT$", "w", stderr);	                //  Establish error stream
        ShowWindow(m_pWndw, m_isVisible ? SW_SHOW : SW_HIDE);		        //	Show console window based on visible state
        SetConsoleTitleA(title);                                            //  Set console window title
        m_isConsoleAllocated = true;
    }

    ///---------------------------------------------------------------------------------------------------
    Console::~Console() { DestroyConsole(); }

    ///---------------------------------------------------------------------------------------------------
    Console* Console::GetDefaultInstance() { return m_instance; }

    ///---------------------------------------------------------------------------------------------------
    void Console::DestroyConsole()
    {
        if (!m_isConsoleAllocated)
            return;

        m_isConsoleAllocated = false;                                       //
        m_isVisible = true;                                                 //
        fclose(m_pInStream);                                                //  
        fclose(m_pOutStream);                                               //  
        fclose(m_pErrStream);                                               //  
        DestroyWindow(m_pWndw);                                             //  
        FreeConsole();
    }	
    
    //---------------------------------------------------------------------------------------------------
    void Console::LogMsgEx(FILE* stream, HANDLE pHand, const char* msg, EConsoleColors color, va_list args)
    {
        if (!m_isConsoleAllocated)                                          //  only write when console is allocated
            return;

        std::lock_guard<std::mutex> lock(m_mutex);                          //  thread safety
        SetConsoleTextAttribute(pHand, color);					            //	Set output text color
        vfprintf(stream, msg, args);								        //	print
        SetConsoleTextAttribute(pHand, EConsoleColors::DEFAULT);	        //	Restore output text color to default
    }

    //---------------------------------------------------------------------------------------------------
    void Console::LogMsg(const char* text, ...)
    {
        if (!m_isConsoleAllocated)
            return;
        
        va_list args;												        //	declare arguments
        va_start(args, text);										        //	get arguments
        LogMsgEx(m_pOutStream, m_pHandle, text, EConsoleColors::DEFAULT, args);//	print
        va_end(args);												        //	clear arguments
    }

    //---------------------------------------------------------------------------------------------------
    void Console::cLogMsg(const char* text, EConsoleColors color, ...)
    {
        if (!m_isConsoleAllocated)
            return;

        va_list args;												        //	declare arguments
        va_start(args, color);										        //	get arguments
        LogMsgEx(m_pOutStream, m_pHandle, text, color, args);		        //	print
        va_end(args);												        //	clear arguments
    }

#pragma endregion

    //----------------------------------------------------------------------------------------------------
    //										ENGINE
    //-----------------------------------------------------------------------------------

#pragma region  //  ENGINE

    Engine* Engine::m_instance = new Engine();

    ///---------------------------------------------------------------------------------------------------
    //  D3D Template Hook
    bool Engine::D3D11HookPresent(IDXGISwapChain* p, void* ofnc, void* nFnc)
    {
        if (GSDevice::GetRenderAPI() != RenderAPI::D3D11)
            return false;

        // Get GS Device
        auto d3d11 = static_cast<GSDevice11*>(CGlobals::g_gs_device);
        if (!d3d11)
            return false;

        //  Get SwapChain
        p = d3d11->GetSwapChain();
        if (!p)
            return false;

        // Hook
        hkVFunction(p, 8, ofnc, nFnc);

        return true;
    }

    ///---------------------------------------------------------------------------------------------------
    void Engine::D3D11UnHookPresent(IDXGISwapChain* p, void* ofnc)
    {
        if (!p)
            return;

        hkRestoreVFunction(p, 8, ofnc);
        p = nullptr;
        ofnc = 0;
    }

    ///---------------------------------------------------------------------------------------------------
    Engine* Engine::GetDefaultInstance() { return m_instance; }

#pragma endregion

    //----------------------------------------------------------------------------------------------------
	//										MEMORY
	//-----------------------------------------------------------------------------------

#pragma region  //  MEMORY

    //----------------------------------------------------------------------------------------------------
    //  STATICS
    __int64                     Memory::dwGameBase;
    __int64                     Memory::dwEEMem;
    __int64                     Memory::BasePS2MemorySpace;
    ProcessInfo                 Memory::Process;                                        
    bool                        Memory::m_isInitialized;                
    Memory*                     Memory::m_instance = new Memory();

    //----------------------------------------------------------------------------------------------------
    //  CONSTRUCTORS
    Memory::Memory()
    {
        if (m_isInitialized)
            return;

        if (ObtainProcessInfo(Process))
        {
            Memory::dwGameBase          = (__int64)Process.m_ModuleBase;
            Memory::dwEEMem             = (__int64)GetProcAddress((HMODULE)Process.m_ModuleHandle, "EEmem");
            Memory::BasePS2MemorySpace  = *(__int64*)dwEEMem;
            m_isInitialized = true;
        }
    }
    Memory::~Memory() {}

    ///---------------------------------------------------------------------------------------------------
    //	[MEMORY]
    Memory* Memory::GetDefaultInstance() { return m_instance; }

    ///---------------------------------------------------------------------------------------------------
    __int64 Memory::GetModuleBase() { return dwGameBase; }

    ///---------------------------------------------------------------------------------------------------
    __int64 Memory::GetAddr(unsigned int offset) { return dwGameBase + offset; }

    ///---------------------------------------------------------------------------------------------------
    bool Memory::ObtainProcessInfo(ProcessInfo& pInfo)
    {
        // Get initial process info
        pInfo.m_ModulePID       = GetCurrentProcessId();
        pInfo.m_ModuleHandle    = GetModuleHandle(NULL);
        pInfo.m_ModuleBase      = (uintptr_t)GetModuleHandle(NULL);

        // Get the path of the module
        char szModulePath[MAX_PATH];
        if (!GetModuleFileNameA((HMODULE)pInfo.m_ModuleHandle, szModulePath, MAX_PATH))
        {
            CloseHandle(pInfo.m_ModuleHandle);
            return FALSE;
        }

        // Get the main window of the current process
        HWND hWnd = GetForegroundWindow();
        if (!hWnd)
        {
            CloseHandle(pInfo.m_ModuleHandle);
            return FALSE;
        }

        // Get the window title and class name of the main window
        char szWindowTitle[MAX_PATH];
        char szClassName[MAX_PATH];
        GetWindowTextA(hWnd, szWindowTitle, MAX_PATH);
        GetClassNameA(hWnd, szClassName, MAX_PATH);

        // Get the size of the main window
        RECT rect;
        GetWindowRect(hWnd, &rect);
        int nWidth = rect.right - rect.left;
        int nHeight = rect.bottom - rect.top;

        // Set the fields of the ProcessInfo struct
        pInfo.m_GameWindow      = hWnd;
        pInfo.m_WindowTitle     = szWindowTitle;
        pInfo.m_ClassName       = szClassName;
        pInfo.m_ModulePath      = szModulePath;
        pInfo.m_WindowWidth     = nWidth;
        pInfo.m_WindowHeight    = nHeight;
        pInfo.m_WindowSize      = Vector2((float)nWidth, (float)nHeight);
        return TRUE;
    }

    ///---------------------------------------------------------------------------------------------------
    //	[MEMORY]
    // Resolves Pointer Chain from input Shorthand RAW PS2 Format Address
    // <returns></returns>
    __int64 Memory::ResolvePtrChain(__int64 addr, std::vector<unsigned int> offsets)
    {
        //  --
        //  uintptr_t addr = (*(int32_t*)GetPS2Addr(RAW_PS2_OFFSET)) + Memory::BasePS2MemorySpace;
        //  if (offsets.empty())
        //      return addr;
        //  
        //  // --- untested (possibly not needed anyways, should work)
        //  // GetClassPtr and relevent functions within each class will make this useless
        //  for (int i = 0; i < offsets.size(); i++)
        //  {
        //      addr = *(int32_t*)addr;
        //      addr += (offsets[i] + Memory::BasePS2MemorySpace);
        //  }
        //  return addr;
        return 0;
    }

    ///---------------------------------------------------------------------------------------------------
    //	[MEMORY]
    // Write Assembly patches to desired location
    // <returns>TRUE if operation is a success, otherwise result is FALSE</returns>
    // Note: resolve any offsets prior to running this function.
    bool Memory::BytePatch(__int64 Address, BYTE* bytes, unsigned int size)
    {
        DWORD oldprotect;
        auto status = VirtualProtect(reinterpret_cast<void*>(Address), size, PAGE_EXECUTE_READWRITE, &oldprotect);
        if (!status)
            return FALSE;

        memcpy(reinterpret_cast<void*>(Address), bytes, size);
        VirtualProtect(reinterpret_cast<void*>(Address), size, oldprotect, &oldprotect);
        return TRUE;
    }

    ///---------------------------------------------------------------------------------------------------
    //	[MEMORY]
    // Write NOP opcodes for desired size at the set destination
    // <returns>TRUE if operation is a success, otherwise result is FALSE</returns>
    bool Memory::NopBytes(__int64 Address, unsigned int size)
    {
        DWORD oldprotect;
        auto status = VirtualProtect(reinterpret_cast<void*>(Address), size, PAGE_EXECUTE_READWRITE, &oldprotect);
        if (!status)
            return FALSE;

        memset(reinterpret_cast<void*>(Address), 0x00, size);
        VirtualProtect(reinterpret_cast<void*>(Address), size, oldprotect, &oldprotect);
        return TRUE;
    }

#pragma endregion

    //----------------------------------------------------------------------------------------------------
    //									    PS2Memory
    //-----------------------------------------------------------------------------------

#pragma region  //  PS2Memory

    ///---------------------------------------------------------------------------------------------------
    __int64 PS2Memory::GetModuleBase() { return Memory::BasePS2MemorySpace; }

    ///---------------------------------------------------------------------------------------------------
    //	[MEMORY]
    // Converts shortened RAW PS2 format address to x64 address
    // EXAMPLE:
    // - Original RAW  : 20123456  
    // - Shortened RAW : 0x123456
    // - Base Memory   : 7FF660000000
    // - Result        : 7FF660000000 + 0x123456 = 7FF660123456
    __int64 PS2Memory::GetAddr(__int32 offset) { return Memory::BasePS2MemorySpace + offset; }

    ///---------------------------------------------------------------------------------------------------
    //	[MEMORY]
    // Resolves Pointer Chain from input Shorthand RAW PS2 Format Address
    // <returns></returns>
    __int64 PS2Memory::ResolvePtrChain(__int32 RAW_PS2_OFFSET, std::vector<__int32> offsets)
    {
        //  --
        uintptr_t addr = (*(int32_t*)GetAddr(RAW_PS2_OFFSET)) + Memory::BasePS2MemorySpace;
        if (offsets.empty())
            return addr;

        // --- untested (possibly not needed anyways, should work)
        // GetClassPtr and relevent functions within each class will make this useless
        for (int i = 0; i < offsets.size(); i++)
        {
            addr = *(int32_t*)addr;
            addr += (offsets[i] + Memory::BasePS2MemorySpace);
        }
        return addr;
    }

#pragma endregion

    //----------------------------------------------------------------------------------------------------
	//										TOOLS
	//-----------------------------------------------------------------------------------

#pragma region  //  TOOLS


#pragma endregion

    //----------------------------------------------------------------------------------------------------
    //									TOOLS::CPUTimer
    //-----------------------------------------------------------------------------------

#pragma region  //  TOOLS::CPUTimer

    Tools::CPUTimer::CPUTimer() { Start(); }

    void Tools::CPUTimer::Start() { m_start = clock(); }

    void Tools::CPUTimer::Stop() { m_end = clock(); }

    void Tools::CPUTimer::Reset() { Start(); }

    double Tools::CPUTimer::GetElapsedTime(clock_t time, ETimings t = ETimings::ETiming_MS) const
    {
        double res = static_cast<double>(time - m_start) / CLOCKS_PER_SEC;
        switch (t)
        {
            case ETimings::ETiming_MS: return res * 1000;
            case ETimings::ETiming_S: return res;
            case ETimings::ETiming_M: return res / 60;
            case ETimings::ETiming_HR: return res / 3600;
            default: return res;
        }
    }

    double Tools::CPUTimer::Stop(ETimings t)
    {
        m_end = clock();
        return GetElapsedTime(m_end, t);
    }

#pragma endregion


    //----------------------------------------------------------------------------------------------------
    //									TOOLS::Math3D
    //-----------------------------------------------------------------------------------

#pragma region  //  TOOLS::Math3D

    ///---------------------------------------------------------------------------------------------------
    //	[TOOL]
    //	Gets distance from Position A to Position B
    // <returns>Vector3</returns>
    float Tools::Math3D::GetDistanceTo3DObject(Vector3 POS, Vector3 POS2)
    {
        float x = (POS2.x - POS.x);
        float y = (POS2.y - POS.y);
        float z = (POS2.z - POS.z);
        float distance = std::sqrt(x * x + y * y + z * z);
        return (distance);
    }

#pragma endregion

}