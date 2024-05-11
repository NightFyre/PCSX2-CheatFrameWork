#pragma once
#include <CDK.h>
static bool g_running;

//---------------------------------------------------------------------------------------------------
//  fwd declare rendering helpers
void HUD();
void MENU();
void InitImGui();
void RenderImGui();
LRESULT WndProc(const HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

//---------------------------------------------------------------------------------------------------
//  Initialize Client Thread
DWORD WINAPI MainThread(LPVOID hInstance)
{

#if _DEBUG

    MessageBoxA(NULL, "Initializing PCSX2 Cheat Device", "DEBUG BUILD", MB_OK | MB_ICONINFORMATION);

#endif    

    //  initialize pcsx2 cheat dev kit
    if (PlayStation2::InitCDK("pcsx2-qt.exe", true))
    {
        //  Obtain global instance of GSDevice
        PlayStation2::PCSX2::g_gs_device = reinterpret_cast<PlayStation2::GSDevice*>(*(__int64*)(PlayStation2::Memory::GetAddr(PlayStation2::PCSX2::o_gs_device))); //  offset because have not implemented reading assembly op code for variable , must be shrunk
        if (PlayStation2::PCSX2::g_gs_device)
        {
            //  Get RenderAPI in use
            if (PlayStation2::GSDevice::GetRenderAPI() == PlayStation2::RenderAPI::D3D11)
            {
                // Get GSDevice11*
                auto d3d11 = static_cast<PlayStation2::GSDevice11*>(PlayStation2::PCSX2::g_gs_device);

                if (d3d11)
                {
                    //  Get SwapChain & Hook
                    PlayStation2::PCSX2::pSwapChain = d3d11->GetSwapChain();

                    if (PlayStation2::PCSX2::pSwapChain)
                    {
                    
                        PlayStation2::Memory::hkVFunction(PlayStation2::PCSX2::pSwapChain, PlayStation2::DXGI::IDXGI_PRESENT, PlayStation2::PCSX2::oDXGISwapChainPresent, PlayStation2::PCSX2::hkPresent);
                        PlayStation2::Memory::hkVFunction(PlayStation2::PCSX2::pSwapChain, PlayStation2::DXGI::IDXGI_RESIZE_BUFFERS, PlayStation2::PCSX2::oIDXGIResizeBuffers, PlayStation2::PCSX2::hkResizeBuffers);
                        InitImGui();

                    }
                }
            }
            else
                MessageBoxA(NULL, "[ERROR] D3D11 NOT PRESENT", "Couldn't detect D3D11 as the active renderer.\nPlease switch to D3D11 before proceeding.", MB_OK | MB_ICONEXCLAMATION);
        }

        g_running = true;

        //  Loop
        do
        {
            //  Exit Module
            if (PlayStation2::Tools::GetKeyState(VK_END, 500))
                g_running = false;

        } while (g_running);
        
        //  Remove Hook
        if (PlayStation2::PCSX2::pSwapChain)
        {
            //  Restore WndProc
            SetWindowLongPtrA(PlayStation2::PCSX2::RenderWindow, GWLP_WNDPROC, (LONG_PTR)PlayStation2::PCSX2::oWndProc);

            //  Restore Hooks
            PlayStation2::Memory::hkRestoreVFunction(PlayStation2::PCSX2::pSwapChain, PlayStation2::DXGI::IDXGI_PRESENT, PlayStation2::PCSX2::oDXGISwapChainPresent);
            PlayStation2::Memory::hkRestoreVFunction(PlayStation2::PCSX2::pSwapChain, PlayStation2::DXGI::IDXGI_RESIZE_BUFFERS, PlayStation2::PCSX2::oIDXGIResizeBuffers);
        }

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

//---------------------------------------------------------------------------------------------------
//  initialize dear imgui
//  @ERROR: ImGui is not capturing keyboard navigation ???
void InitImGui()
{
    if (SUCCEEDED(PlayStation2::PCSX2::pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&PlayStation2::PCSX2::D3D11Device)))
    {
        DXGI_SWAP_CHAIN_DESC sd;
        PlayStation2::PCSX2::pSwapChain->GetDesc(&sd);
        PlayStation2::PCSX2::RenderWindow = sd.OutputWindow;
        PlayStation2::PCSX2::D3D11Device->GetImmediateContext(&PlayStation2::PCSX2::D11DeviceCtx);

        ID3D11Texture2D* buffer;
        PlayStation2::PCSX2::pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&buffer);
        PlayStation2::PCSX2::D3D11Device->CreateRenderTargetView(buffer, NULL, &PlayStation2::PCSX2::D11RenderTarget);
        PlayStation2::PCSX2::oWndProc = (WNDPROC)SetWindowLongPtrA(PlayStation2::PCSX2::RenderWindow, GWLP_WNDPROC, (LONG_PTR)PlayStation2::PCSX2::WndProc);
        buffer->Release();

        ImGui::CreateContext();
        ImGui::StyleColorsClassic();
        ImGuiIO& io = ImGui::GetIO();
        ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard;
        io.LogFilename = nullptr;
        io.IniFilename = nullptr;
        ImGui_ImplWin32_Init(PlayStation2::PCSX2::RenderWindow);
        ImGui_ImplDX11_Init(PlayStation2::PCSX2::D3D11Device, PlayStation2::PCSX2::D11DeviceCtx);

        PlayStation2::PCSX2::isImGuiInit = true;
        PlayStation2::PCSX2::RegisterRenderFunction(RenderImGui);
    }
}

//---------------------------------------------------------------------------------------------------
//  render gui window
void RenderImGui()
{

    //  Listen for Menu Show Key
    if (PlayStation2::Tools::GetKeyState(VK_INSERT, 500))
        PlayStation2::PCSX2::isMenuShown ^= 1;

    //  Render GUI Menu
    if (PlayStation2::PCSX2::isMenuShown)
        MENU();

    //  Render Heads Up Display
    HUD();

}

//---------------------------------------------------------------------------------------------------
//  render gui window
enum EMemoryDisplayType : int
{
    EBYTE = 1,
    EWORD = 2,
    EDWORD = 4,
    EFLOAT = 6,
    EQWORD = 8
};
static int alignment{ 4 };              //  4 - 24
static int mDisplayType{ 0 };           //  DisplayType: Byte, 2Byte, 4Byte, 8Byte, 
static int DisplayType{ EBYTE };        //  DisplayType: Byte, 2Byte, 4Byte, 8Byte, 
static int MemReadSize{ 0x1024 };       //  1MB
static const char* items[] = { "BYTE", "2 BYTE", "4 BYTE" /*, "8 BYTE"*/, "FLOAT" };

void MENU()
{
    //   Render ImGui
    ImGui::SetNextWindowSize(ImVec2(480.f, 480.f), ImGuiCond_Once);
    if (!ImGui::Begin("[ PCSX2 ] Cheat Device", nullptr))
    {
        ImGui::End();
        return;
    }

    ImVec2 wndwPos = ImGui::GetWindowPos();
    ImVec2 wndwSize = ImGui::GetWindowSize();
    ImVec2 mousePos = ImGui::GetMousePos();
    ImGui::Text("Window Pos: { %.0f, %.0f }", wndwPos.x, wndwPos.y);
    ImGui::Text("Window Size: { %.0f, %.0f }", wndwSize.x, wndwSize.y);
    ImGui::Text("Mouse Cursor Pos: { %.0f, %.0f }", mousePos.x, mousePos.y);

    // Toggle Cheat Device console visibility
    static bool bShowConsole{ true };
    if (ImGui::Checkbox("SHOW CONSOLE", &bShowConsole))
        PlayStation2::Console::SetViewState(bShowConsole);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    //  Display PCSX2 Process Information
    ImGui::Text("PCSX2ModuleBase: 0x%llX", PlayStation2::Memory::GetModuleBase());
    ImGui::Text("PS2ModuleBase: 0x%llX", PlayStation2::PS2Memory::GetModuleBase());
    ImGui::Text("D3D11 SwapChain: 0x%llX", PlayStation2::PCSX2::pSwapChain);
    ImGui::Text("GSDevice: 0x%llX", PlayStation2::PCSX2::g_gs_device);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    //  EXAMPLE OUTPUT: "00007FF76C0000000: 00 00 00 00"    //  1 Byte x 4
    if (ImGui::CollapsingHeader("MEMORY VIEWER", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_DefaultOpen))
    {
        //  ImGui::InputInt("Memory Read Size:", &MemReadSize, 1, 1);   //  @TODO: Keyboard input not detected ??
        ImGui::SetNextItemWidth(75.f);
        if (ImGui::Combo("##DisplayType:", &mDisplayType, items, IM_ARRAYSIZE(items)))
        {
            switch (mDisplayType)
            {
            case 0: DisplayType = EBYTE; break;
            case 1: DisplayType = EWORD; break;
            case 2: DisplayType = EDWORD; break;
            case 3: DisplayType = EFLOAT; break;
            case 4: DisplayType = EQWORD; break;
            }
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(75.f);
        if (ImGui::InputInt("##Alignment:", &alignment, 1, 0, ImGuiInputTextFlags_CharsDecimal))
        {
            if (alignment <= 0)
                alignment = 1;
        }
        ImGui::BeginChild("##MEMORY", ImVec2(ImGui::GetContentRegionAvail().x, 200.f), ImGuiChildFlags_Border);
        {
            for (int i = 0; i < MemReadSize; i += alignment)
            {
                static __int64 base = PlayStation2::PS2Memory::GetModuleBase();

                if (!base)
                {

                    ImGui::Text("Failed to get module base address.");

                    break;
                }


                ImGui::Text("%p: ", (unsigned long long)(base + i));

                ImGui::SameLine();

                for (int j = 0; j < alignment; j++)
                {

                    switch (DisplayType)
                    {
                    case EBYTE: ImGui::Text("%02X ", PlayStation2::PS2Memory::ReadLong<unsigned __int8>(base + i + j)); break;
                    case EWORD: ImGui::Text("%04X ", PlayStation2::PS2Memory::ReadLong<unsigned __int16>(base + i + j)); j += 2; break;
                    case EDWORD: ImGui::Text("%08X ", PlayStation2::PS2Memory::ReadLong<unsigned __int32>(base + i + j)); j += 4; break;
                    case EFLOAT: ImGui::Text("%08X ", PlayStation2::PS2Memory::ReadLong<float>(base + i + j)); j += 4; break;
                    case EQWORD: ImGui::Text("%08X ", PlayStation2::PS2Memory::ReadLong<unsigned __int64>(base + i + j)); break;
                    }
                    ImGui::SameLine();

                }

                ImGui::NewLine();

            }
        }
        ImGui::EndChild();
    
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("EXIT CHEAT DEVICE", ImVec2(ImGui::GetContentRegionAvail().x, 20.f)))
        g_running = false;

    ImGui::End();
}

//---------------------------------------------------------------------------------------------------
//  render gui window
void HUD()
{

    auto io = ImGui::GetIO();
    ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetWindowSize(ImGui::GetMainViewport()->Size);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, NULL);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.00f, 0.00f, 0.00f, 0.00f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.00f, 0.00f, 1.00f, 0.00f));
    if (!ImGui::Begin("[ PCSX2 ] Cheat Device Heads Up Display", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs))
    {
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar();
        ImGui::End();
        return;
    }
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar();

    auto ImDraw = ImGui::GetBackgroundDrawList();
    auto draw_size = ImGui::GetMainViewport()->Size;
    auto center = ImVec2({ draw_size.x * .5f, draw_size.y * .5f });
    auto top_center = ImVec2({ draw_size.x * .5f, draw_size.y * 0.0f });

    const char* text = "[ PCSX2 ] CHEAT DEVICE";
    float textSize = ImGui::CalcTextSize(text).x;
    ImVec2 textPosition = ImVec2(top_center.x - (textSize * 0.5f), top_center.y);
    ImDraw->AddText(ImGui::GetFont(), 16.f, textPosition, IM_COL32_WHITE, text, text + strlen(text), 800, 0);

    ImGui::End();

}