#pragma once
#include <CDK.h>

/**
 * Name: PlayStation2 - PCSX2
 * Version: 1.0.0
 * Author: NightFyre
*/

namespace PlayStation2
{
    //----------------------------------------------------------------------------------------------------
    //									CORE
    //-----------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------------------------
    bool InitCDK(const std::string& moduleName, bool bDefaultInstances)
    {
        bool result{ false };

        HANDLE pHand = GetModuleHandleA(moduleName.c_str());
        if (!pHand)
            return result;

        if (bDefaultInstances)
        {
            CGlobals::g_console = Console::GetDefaultInstance();
            
            CGlobals::g_engine = Engine::GetDefaultInstance();

            CGlobals::g_memory = Memory::GetDefaultInstance();

            Console::SetViewState(true);
            Console::LogMsg("[+] PCSX2 Cheat Device Initialized!\n- PCSX2:\t0x%llX\n- PS2:\t\t0x%llX\n\n",
                (__int64)pHand,
                PS2Memory::GetModuleBase()
            );
        }

        return true;
    }

    ///---------------------------------------------------------------------------------------------------
    // Template Initialization function compatible with Nightly Builds
    bool InitCDK() { return InitCDK("pcsx2-qt.exe", true); }

    ///---------------------------------------------------------------------------------------------------
    void ShutdownCDK() { Console::DestroyConsole(); }

    //----------------------------------------------------------------------------------------------------
    //									CGLOBALS
    //-----------------------------------------------------------------------------------

#pragma region  //  CGLOBALS

    //----------------------------------------------------------------------------------------------------
    //  STATICS
    Console*                CGlobals::g_console;
    Engine*                 CGlobals::g_engine;
    Memory*                 CGlobals::g_memory;

#pragma endregion

    //----------------------------------------------------------------------------------------------------
    //									CONSOLE
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
    bool                    Console::m_isVisible{ false };
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

    ///---------------------------------------------------------------------------------------------------
    void Console::SetViewState(bool x) { ShowWindow(GetConsoleWindow(), x ? SW_SHOW : SW_HIDE); m_isVisible = x; }

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
        
        va_list args;												            //	declare arguments
        va_start(args, text);										            //	get arguments
        LogMsgEx(m_pOutStream, m_pHandle, text, EConsoleColors::DEFAULT, args); //	print
        va_end(args);												            //	clear arguments
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
    //									ENGINE
    //-----------------------------------------------------------------------------------

#pragma region  //  ENGINE

    Engine* Engine::m_instance = new Engine();

    ///---------------------------------------------------------------------------------------------------
    Engine* Engine::GetDefaultInstance() { return m_instance; }

#pragma endregion

    //----------------------------------------------------------------------------------------------------
	//									MEMORY
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
        pInfo.m_WindowTitle     = std::string(szWindowTitle, lstrlenA(szWindowTitle));
        pInfo.m_ClassName       = std::string(szClassName, lstrlenA(szClassName));
        pInfo.m_ModulePath      = std::string(szModulePath, lstrlenA(szModulePath));
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

    ///---------------------------------------------------------------------------------------------------
    unsigned int Memory::GetVtblOffset(void* czInstance, const char* dwModule) { return ((*(unsigned int*)czInstance) - dwGameBase); }

    ///---------------------------------------------------------------------------------------------------
    int Memory::GetVtblIndex(void* fncPtr, void* vTblAddr) { return (((__int64)fncPtr - (__int64)vTblAddr) / sizeof(void*)) - 1; }

    ///---------------------------------------------------------------------------------------------------
    __int64 Memory::SignatureScan(const char* sig)
    {
        // Retrieve module headers
        const auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(dwGameBase);
        const auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(dwGameBase + dosHeader->e_lfanew);

        // Calculate the size of the image
        const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;

        // Convert signature pattern to byte array
        auto patternBytes = SignatureToBytes(sig);

        // Get pointer to the start of the module
        const auto scanBytes = reinterpret_cast<std::uint8_t*>(dwGameBase);

        // Length of the pattern
        const auto patternSize = patternBytes.size();
        const auto patternData = patternBytes.data();

        // Loop through the module memory
        for (uintptr_t i = 0; i < sizeOfImage - patternSize; ++i) 
        {
            bool found = true;
            // Compare each byte of the module memory with the pattern
            for (size_t j = 0; j < patternSize; ++j) {
                
                // If byte doesn't match or is wildcard ('?'), move to the next byte
                if (scanBytes[i + j] != patternData[j] && patternData[j] != -1) 
                {
                    found = false;
                    break;
                }
            }
            
            // If pattern is found, return the address
            if (found)
                return reinterpret_cast<uintptr_t>(&scanBytes[i]);
        }
        
        // Return NULL if pattern is not found
        return NULL;
    }

    ///---------------------------------------------------------------------------------------------------
    std::vector<int> Memory::SignatureToBytes(const char* pattern) 
    {
        std::vector<int> bytes;
        const char* start = pattern;
        const char* end = pattern + std::strlen(pattern);

        for (const char* current = start; current < end; ++current) 
        {
            if (*current == '?') 
            {
                ++current;
                
                if (*current == '?')
                    ++current;
                
                bytes.push_back(-1); // Placeholder for wildcard '?'
            }
            else
                bytes.push_back(std::strtoul(current, const_cast<char**>(&current), 16)); // Convert hex string to integer
        }
        return bytes;
    }


#pragma endregion

    //----------------------------------------------------------------------------------------------------
    //									PS2Memory
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
	//									TOOLS
	//-----------------------------------------------------------------------------------

#pragma region  //  TOOLS

    bool Tools::GetKeyState(WORD v, SHORT delta)
    {
        static int lastTick = 0;
        
        bool result = ((GetAsyncKeyState(v) & 0x8000) && (GetTickCount64() - lastTick) > delta);
        
        if (result)
            lastTick = GetTickCount64();
        
        return result;
    }

    bool Tools::GamepadGetButtonState(WORD v)
    {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        DWORD result = XInputGetState(0, &state);
        if (result == ERROR_SUCCESS)
        {
            if ((state.Gamepad.wButtons & v) == v)
                return true;
        }
        return false;
    }

#pragma endregion

    //----------------------------------------------------------------------------------------------------
    //									TOOLS::CPUTimer
    //-----------------------------------------------------------------------------------

#pragma region  //  TOOLS::CPUTimer

    Tools::CPUTimer::CPUTimer() { Start(); }

    void Tools::CPUTimer::Start() { m_start = clock(); }

    void Tools::CPUTimer::Stop() { m_end = clock(); }

    void Tools::CPUTimer::Reset() { Start(); }

    double Tools::CPUTimer::GetElapsedTime(clock_t time, ETimings t) const
    {
        double res = static_cast<double>(time - m_start) / CLOCKS_PER_SEC;
        switch (t)
        {
            case ETimings::ETiming_MS: return res * 1000;
            case ETimings::ETiming_S: return res;
            case ETimings::ETiming_M: return res / 60;
            case ETimings::ETiming_HR: return res / 3600;
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


    //----------------------------------------------------------------------------------------------------
    //									PCSX2
    //-----------------------------------------------------------------------------------

#pragma region  //  PCSX2

    //----------------------------------------------------------------------------------------------------
    //  STATICS
    //  Class Offsets
    //  EmuThread*                  PCSX2::g_emu_thread;        //  NOT USED FOR NOW
    //  GSRenderer*                 PCSX2::g_gs_renderer;       //  NOT USED FOR NOW
    unsigned int                    PCSX2::o_gs_device{ 0x3FA2728 };
    GSDevice*                       PCSX2::g_gs_device;
    unsigned int                    PCSX2::o_GSUpdateDisplayWindow;
    unsigned int                    PCSX2::o_GSDevice_GetRenderAPI;
    unsigned int                    PCSX2::o_psxRecompileInstruction{ 0x269D80 };
    unsigned int                    PCSX2::o_recompileNextInstruction{ 0x291CA0 };
    unsigned int                    PCSX2::o_recResetEE{ 0x2942C0 };
    cpuRegisters*                   PCSX2::g_cpuRegs;
    __int32                         PCSX2::g_cpupc{ 0x0 };
    unsigned int                    PCSX2::o_cpuRegs{ 0x2EA8F2C };
    psxRegisters*                   PCSX2::g_psxRegs;
    __int32                         PCSX2::g_psxpc{ 0x0 };
    unsigned int                    PCSX2::o_psxRegs{ 0x2EA809C };
    unsigned int                    PCSX2::g_discordPresemt{ 0xE9090 };
    PCSX2::IDXGISwapChainPresent    PCSX2::oDXGISwapChainPresent;   //  value automatically handled
    PCSX2::ResizeBuffers            PCSX2::oIDXGIResizeBuffers;   //  value automatically handled
    ID3D11RenderTargetView*         PCSX2::D11RenderTarget;         //  value automatically handled
    ID3D11DeviceContext*            PCSX2::D11DeviceCtx;            //  value automatically handled
    IDXGISwapChain*                 PCSX2::pSwapChain;              //  
    ID3D11Device*                   PCSX2::D3D11Device;             //  value automatically handled
    WNDPROC                         PCSX2::oWndProc;                //  value automatically handled
    HWND                            PCSX2::RenderWindow;            //  value automatically handled
    PCSX2::RenderGUI                PCSX2::fn_Render;				//	called in hkPresent
    bool                            PCSX2::isImGuiInit;             //  value automatically handled
    bool                            PCSX2::isMenuShown;

    ///  PCSX2 IConsoleWriter
    PCSX2::IConsoleWriter_WriteLn_stub PCSX2::WriteLn               = reinterpret_cast<PCSX2::IConsoleWriter_WriteLn_stub>(Memory::SignatureScan("48 83 EC ? 48 89 C8 48 89 54 24 ? 4C 89 44 24 ? 4C 89 4C 24 ? 4C 8D 4C 24 ? 4C 89 4C 24 ? B9 ? ? ? ? 31 D2 49 89 C0 E8 ? ? ? ? 90 48 83 C4 ? C3 CC CC CC CC CC CC CC CC CC CC CC 48 83 EC"));
    PCSX2::IConsoleWriter_cWriteLn_stub PCSX2::WriteColorLn         = reinterpret_cast<PCSX2::IConsoleWriter_cWriteLn_stub>(Memory::SignatureScan("48 83 EC ? 48 89 D0 89 CA 4C 89 44 24 ? 4C 89 4C 24 ? 4C 8D 4C 24 ? 4C 89 4C 24 ? B9 ? ? ? ? 49 89 C0 E8 ? ? ? ? 90 48 83 C4 ? C3 48 83 EC"));
    PCSX2::IConsoleWriter_Warning_stub PCSX2::WriteWarning          = reinterpret_cast<PCSX2::IConsoleWriter_Warning_stub>(Memory::SignatureScan("48 83 EC ? 48 89 C8 48 89 54 24 ? 4C 89 44 24 ? 4C 89 4C 24 ? 4C 8D 4C 24 ? 4C 89 4C 24 ? B9 ? ? ? ? BA ? ? ? ? 49 89 C0 E8 ? ? ? ? 90 48 83 C4 ? C3 CC CC CC CC CC CC CC CC 56 57"));
    PCSX2::IConsoleWriter_Error_stub PCSX2::WriteError              = reinterpret_cast<PCSX2::IConsoleWriter_Warning_stub>(Memory::SignatureScan("48 83 EC ? 48 89 C8 48 89 54 24 ? 4C 89 44 24 ? 4C 89 4C 24 ? 4C 8D 4C 24 ? 4C 89 4C 24 ? B9 ? ? ? ? BA ? ? ? ? 49 89 C0 E8 ? ? ? ? 90 48 83 C4 ? C3 CC CC CC CC CC CC CC CC 41 57 41 56 41 55"));
    PCSX2::IConsoleWriterDev_WriteLn_stub PCSX2::DevWriteLn         = reinterpret_cast<PCSX2::IConsoleWriterDev_WriteLn_stub>(Memory::SignatureScan("48 83 EC ? 48 89 C8 48 89 54 24 ? 4C 89 44 24 ? 4C 89 4C 24 ? 4C 8D 4C 24 ? 4C 89 4C 24 ? B9 ? ? ? ? 31 D2 49 89 C0 E8 ? ? ? ? 90 48 83 C4 ? C3 CC CC CC CC CC CC CC CC CC CC CC 41 57"));
    PCSX2::IConsoleWriterDev_cWriteLn_stub PCSX2::DevColorWriteLn   = reinterpret_cast<PCSX2::IConsoleWriterDev_cWriteLn_stub>(Memory::SignatureScan("48 83 EC ? 48 89 D0 89 CA 4C 89 44 24 ? 4C 89 4C 24 ? 4C 8D 4C 24 ? 4C 89 4C 24 ? B9 ? ? ? ? 49 89 C0 E8 ? ? ? ? 90 48 83 C4 ? C3 56"));
    PCSX2::IConsoleWriterDev_Warning_stub PCSX2::DevWriteWarning    = reinterpret_cast<PCSX2::IConsoleWriterDev_Warning_stub>(Memory::SignatureScan("48 83 EC ? 48 89 C8 48 89 54 24 ? 4C 89 44 24 ? 4C 89 4C 24 ? 4C 8D 4C 24 ? 4C 89 4C 24 ? B9 ? ? ? ? BA ? ? ? ? 49 89 C0 E8 ? ? ? ? 90 48 83 C4 ? C3 CC CC CC CC CC CC CC CC 56 48 81 EC"));
    PCSX2::IConsoleWriterDev_Error_stub PCSX2::DevWriteError        = reinterpret_cast<PCSX2::IConsoleWriterDev_Error_stub>(Memory::SignatureScan("48 83 EC ? 48 89 C8 48 89 54 24 ? 4C 89 44 24 ? 4C 89 4C 24 ? 4C 8D 4C 24 ? 4C 89 4C 24 ? B9 ? ? ? ? BA ? ? ? ? 49 89 C0 E8 ? ? ? ? 90 48 83 C4 ? C3 CC CC CC CC CC CC CC CC 41 57 41 56 56"));
    
    
    //-----------------------------------------------------------------------------------
    void PCSX2::ResetEE()
    {
        static __int64 pAddr = Memory::SignatureScan("80 3D ? ? ? ? ? 75 ? C6 05 ? ? ? ? ? C6 05 ? ? ? ? ? 80 3D");
        if (!pAddr)
            return;

        static recResetEE_stub fn = reinterpret_cast<recResetEE_stub>(pAddr);

        fn();

        Console::cLogMsg("[+] EE/iR5900 Recompiler Reset.\n", EConsoleColors::dark_gray);
    }

    //-----------------------------------------------------------------------------------
    void PCSX2::ResetIOP()
    {
        static __int64 pAddr = Memory::SignatureScan("41 57 41 56 41 55 41 54 56 57 55 53 48 83 EC ? 66 0F 7F 7C 24 ? 66 0F 7F 74 24 ? 49 BE");
        if (!pAddr)
            return;

        static recResetIOP_stub fn = reinterpret_cast<recResetIOP_stub>(pAddr);

        fn();

        Console::cLogMsg("[+] IOP/iR3000A Recompiler Reset.\n", EConsoleColors::dark_gray);
    }

    //-----------------------------------------------------------------------------------
    //  
    LRESULT PCSX2::WndProc(const HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {

#if DEARIMGUI

        //  Handle window message events when the window is shown
        if (isMenuShown)
        {
            ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);

            return true;
        }
#endif

        return CallWindowProcA(oWndProc, hwnd, msg, wparam, lparam);
    }

    //-----------------------------------------------------------------------------------
    //  
    HRESULT PCSX2::hkResizeBuffers(IDXGISwapChain* p, UINT bufferCount, UINT Width, UINT Height, DXGI_FORMAT fmt, UINT scFlags)
    {
        //  Get new data & release render target
        pSwapChain = p;
        D11RenderTarget->Release();
        D11RenderTarget = nullptr;

        //  get fn result
        HRESULT result = oIDXGIResizeBuffers(p, bufferCount, Width, Height, fmt, scFlags);
        
        // Get new render target
        ID3D11Texture2D* backBuffer;
        p->GetBuffer(0, __uuidof(ID3D11Texture2D*), (LPVOID*)&backBuffer);
        if (backBuffer)
        {
            D3D11Device->CreateRenderTargetView(backBuffer, 0, &D11RenderTarget);
            backBuffer->Release();
        }

#if DEARIMGUI

        //  Reset ImGui 
        if (isImGuiInit)
        {
            ImGuiIO& io = ImGui::GetIO();
            io.DisplaySize = ImVec2(static_cast<float>(Width), static_cast<float>(Height));
        }

#endif

        return result;
    }

    //-----------------------------------------------------------------------------------
    //  
    HRESULT PCSX2::hkPresent(IDXGISwapChain* p, UINT sync, UINT flags)
    {
#if DEARIMGUI
        
        //  Check for linked function
        if (fn_Render && isImGuiInit)
        {
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            fn_Render();

            ImGui::Render();
            D11DeviceCtx->OMSetRenderTargets(1, &D11RenderTarget, NULL);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        }

#endif

        return oDXGISwapChainPresent(p, sync, flags);
    }

    //-----------------------------------------------------------------------------------
    //  
    void PCSX2::RegisterRenderFunction(PCSX2::RenderGUI p) { fn_Render = p; }

    //-----------------------------------------------------------------------------------
    //  
    void PCSX2::ClearRenderFunction() { fn_Render = 0; }

#pragma endregion

    //----------------------------------------------------------------------------------------------------
    //									GSDevice
    //-----------------------------------------------------------------------------------

#pragma region  //  GSDevice

    //-----------------------------------------------------------------------------------
    RenderAPI GSDevice::GetRenderAPI()
    {
        return Memory::CallVFunction<RenderAPI>(PCSX2::g_gs_device, 9);
    }

#pragma endregion

    //----------------------------------------------------------------------------------------------------
    //									GSDevice::GSDevice11
    //-----------------------------------------------------------------------------------

#pragma region  //  GSDevice::GSDevice12

    bool GSDevice11::isValidSwapChain() { return m_swap_chain != nullptr; }
    
    IDXGISwapChain* GSDevice11::GetSwapChain() { return m_swap_chain; }
    
    ID3D11Device* GSDevice11::GetDevice() { return m_dev; }

    //-----------------------------------------------------------------------------------
    DXGI_SWAP_CHAIN_DESC GSDevice11::GetDesc()
    {
        DXGI_SWAP_CHAIN_DESC desc;
        if (this->m_swap_chain != nullptr)
            this->m_swap_chain->GetDesc(&desc);     //  Directly call d3d11.dll method
        return desc;
    }

#pragma endregion

    //----------------------------------------------------------------------------------------------------
    //									GSDevice::GSDevice12
    //-----------------------------------------------------------------------------------

#pragma region  //  GSDevice::GSDevice11

    bool GSDevice12::isValidSwapChain() { return m_swap_chain != nullptr; }

    IDXGISwapChain* GSDevice12::GetSwapChain() { return m_swap_chain; }

#pragma endregion

}