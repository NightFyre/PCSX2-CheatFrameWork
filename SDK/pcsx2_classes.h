#pragma once

/**
 * Name: PlayStation2 - PCSX2
 * Version: 0.0.1
 * Author: NightFyre
*/

#pragma pack(push, 0x01)
namespace PlayStation2
{
    class PCSX2
    {	
    public:     //  ~



    private:    //  ~
        uintptr_t hk_OnLeftDClick   = NULL; //  Signature("48 8B 05 ? ? ? ? 80 B8 ? ? ? ? ? 74 0C").Scan().As<uint64_t>();
        uintptr_t hk_ResetEE        = NULL; //  Signature("80 3D ? ? ? ? ? 74 13 B8 ? ? ? ? 86").Scan().As<uint64_t>();	// 80 3D 09 18 E8 0C 00 74 13 B8 01 00 00 00 86 05 DF 0B E8 0C C6 05 F6 17 E8 0C 01
        void __cdecl recResetEE(uintptr_t Address); //  calls recResetEE
    };

    class GSRenderer
    {
    };

    class GSRendererHW
    {
    };

    class GSDevice
    {
    private:
        //  - No member variables can be placed in this class due to spacing.
        //  -- instead functions will be used to obtain variables as this does not change the size of the class or inherited classes.

    public:
        RenderAPI                       GetRenderAPI();         //  Obtain the current render api. Should be used to get the current class instance of GDevice
    };

    /*
        GSDevice11 : DirectX11 Rendering API
        - 0x0 = vfTable
    */

    class GSDevice11 : public GSDevice
    {
    private:
        char                            pad_0008[56];	        //0x0000
        unsigned int                    m_window_info;	        //0x0040
        char                            pad_0044[12];	        //0x0044
        HWND                            wndw_hwnd;	            //0x0050
        char                            pad_0058[32];	        //0x0058
        class GSTexture11*              m_texture;	            //0x0078
        char                            pad_0080[80];	        //0x0080
        class IDXGIFactory*             m_dxgi_factory;	        //0x00D0	
        class ID3D11Device*             m_dev;	                //0x00D8
        class ID3D11DeviceContext*      m_ctx;                  //0x00E0
        char                            pad_00E8[8];	        //0x00E8
        class IDXGISwapChain*           m_swap_chain;	        //0x00F0	
        class ID3D11RenderTargetView*   m_swap_chain_rtv;	    //0x00F0	
        char                            pad_00F8[60];	        //0x00F8
        bool                            N00000BBA;	            //0x0134
        bool                            RequestedExclusiveFullscreenMode;	//0x0135
        bool                            N00000BBF;	            //0x0136
        bool                            m_is_exclusive_fullscreen;	//0x0137

    public:
        bool                            isValidSwapChain();
        IDXGISwapChain*                 GetSwapChain();
        ID3D11Device*                   GetDevice();
        DXGI_SWAP_CHAIN_DESC            GetDesc();

    private:
        virtual void                    vf_CreateSurface();
        virtual void                    vf_Function1();
        virtual void                    vf_Function2();
        virtual void                    vf_DoFXAA();
        virtual void                    vf_Function4();
        virtual void                    vf_Function5();
        virtual void                    vf_Function6();
        virtual void                    vf_Create();
        virtual void                    vf_Function8();
        virtual void                    vf_Function9();
        virtual void                    vf_Function10();
        virtual void                    vf_Function11();
        virtual void                    vf_UpdateWindow();
        virtual void                    vf_ResizeWindow();
        virtual void                    vf_Function14();
        virtual void                    vf_Function15();
        virtual void                    vf_Function16();
        virtual void                    vf_Function17();
        virtual void                    vf_GetHostRefreshRate();
        virtual void                    vf_GetDriverInfo();
        virtual void                    vf_Function20();
        virtual void                    vf_Function21();
        virtual void                    vf_Function22();
        virtual void                    vf_Function23();
        virtual void                    vf_Function24();
        virtual void                    vf_Function25();
        virtual void                    vf_Function26();
        virtual void                    vf_Function27();
        virtual void                    vf_Function28();
        virtual void                    vf_Function29();
        virtual void                    vf_Function30();
        virtual void                    vf_Function31();
        virtual void                    vf_Function32();
        virtual void                    vf_RenderHW();
        virtual void                    vf_Function34();
    };  //Size: 0x0138

    /*
        GSDevice12 : DirectX12 Rendering API
        - 0x0 = vfTable
    */
    class GSDevice12 : public GSDevice
    {
    private:
        char                            pad_0008[56];	        //0x0000
        __int64                         m_window_info;	        //0x0040
        char                            pad_0048[8];	        //0x0048
        __int64                         window_hwnd;	        //0x0050
        char                            pad_0058[1264];	        //0x0058
        class IDXGIFactory*             m_dxgi_factory;	        //0x0548
        class IDXGISwapChain*           m_swap_chain;	        //0x0550
        char                            pad_0558[40];	        //0x0558

    public:
        bool isValidSwapChain();
        IDXGISwapChain* GetSwapChain();
    };	//Size: 0x055C


    /*
        GSDeviceOGL : OpenGL Rendering API
        - 0x0 = vfTable
    */
    class GSDeviceOGL : public GSDevice
    {

    };

    /*
        GSDeviceVK : Vulkan Rendering API
        - 0x0 = vfTable
    */
    class GSDeviceVK : public GSDevice
    {

    };

}
#pragma pack(pop)