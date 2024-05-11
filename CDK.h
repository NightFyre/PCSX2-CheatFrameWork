// -------------------------------------------------------------- \\
//						PCSX2 CHEAT DEVICE						  \\
// -------------------------------------------------------------- \\
// Name: PlayStation2 - PCSX2, Version: 1.0.0
#define CDK_VERSION		"v1.0.0"			//	? ? ? 
#define PCSX2_VERSION	"v1.5617"			//	all static offsets are set in accordance to the module version noted here

#define DEARIMGUI_MATH	false				//	
#define MINHOOK			false				//	

/*
	
	// [SECTION] Defines
	// [SECTION] Header Includes
	// [SECTION] Core Methods
	// [SECTION] Core Structures
	// [SECTION] Core Device Classes
	// [SECTION] PCSX2 Structures
	// [SECTION] PCSX2 Classes & Methods
	
*/

#pragma once
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <mutex>
#include <atomic>
#include <chrono>

//	GAMEPAD
#include <XInput.h>
#pragma comment(lib, "XInput.lib")

//	RENDERING APIS
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <dxgi1_4.h>

#if DEARIMGUI_MATH
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include <ImGui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx11.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace PlayStation2
{

	//----------------------------------------------------------------------------------------------------
	//									[SECTION] CORE METHODS
	//-----------------------------------------------------------------------------------

#pragma region	//	CORE METHODS

	// --------------------------------------------------
	// # Global functions
	// --------------------------------------------------
	bool InitCDK(const std::string& moduleName);
	bool InitCDK(const std::string& moduleName, bool bDefaultInstances = true);
	//	bool InitCDK(const std::string& moduleName, unsigned int gRenderer);
	bool InitCDK();
	void ShutdownCDK();

#pragma endregion

	//----------------------------------------------------------------------------------------------------
	//									[SECTION] CORE STRUCTS & ENUMS
	//-----------------------------------------------------------------------------------

#pragma region	//	CORE STRUCTS

	struct Vector2
	{
		float x, y;
		Vector2() : x(0), y(0) {}
		Vector2(float x, float y) : x(x), y(y) {}


		// Addition operator
		Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }

		// Subtraction operator
		Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }

		// Scalar multiplication operator
		Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }

		//	Scalar division operator
		Vector2 operator/(float scalar) const { return Vector2{ x / scalar, y / scalar }; }

		//	float* return operator
		Vector2 operator=(float* other) const { return Vector2{ other[0], other[1] }; }

		// Dot product operator
		float operator*(const Vector2& other) const { return x * other.x + y * other.y; }

		//	
		Vector2& operator*=(const Vector2& other)
		{
			x += other.x;
			y += other.y;
			return *this;
		}

		// Scalar multiplication and assignment operator
		Vector2 operator*=(float scalar) {
			x *= scalar;
			y *= scalar;
			return *this;
		}

		// Length (magnitude) of the vector
		float length() const { return std::sqrt(x * x + y * y); }

		// Normalize the vector
		void normalize()
		{
			float len = length();
			if (len > 0) {
				x /= len;
				y /= len;
			}
		}
	};

	struct Vector3
	{
		float x, y, z;
		Vector3() : x(0), y(0), z(0) {}
		Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

		// Addition operator
		Vector3 operator+(const Vector3& other) const { return Vector3{ x + other.x, y + other.y, z + other.z }; }

		// Subtraction operator
		Vector3 operator-(const Vector3& other) const { return Vector3{ x - other.x, y - other.y, z - other.z }; }

		// Scalar multiplication operator
		Vector3 operator*(float scalar) const { return Vector3{ x * scalar, y * scalar, z * scalar }; }

		//	Scalar division operator
		Vector3 operator/(float scalar) const { return Vector3{ x / scalar, y / scalar, z / scalar }; }

		// Dot product operator
		float operator*(const Vector3& other) const { return x * other.x + y * other.y + z * other.z; }

		// Cross product operator
		Vector3 operator^(const Vector3& other) const { return Vector3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x); }

		//	
		Vector3 operator=(float* other) const { return Vector3{ other[0], other[1], other[2] }; }

		//	
		Vector3& operator*=(const Vector3& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			return *this;
		}

		bool operator==(Vector3 other) const { return other.x == x && other.y == y && other.z == z; }

		// Length (magnitude) of the vector
		float length() const { return std::sqrt(x * x + y * y + z * z); }

		// Normalize the vector
		void normalize()
		{
			float len = length();
			if (len > 0) {
				x /= len;
				y /= len;
				z /= len;
			}
		}
	};

	struct Vector4
	{
		float x, y, z, w;
		Vector4() : x(0), y(0), z(0), w(w) {}
		Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

		// Addition operator
		Vector4 operator+(const Vector4& other) const { return Vector4(x + other.x, y + other.y, z + other.z, w + other.w); }

		// Subtraction operator
		Vector4 operator-(const Vector4& other) const { return Vector4(x - other.x, y - other.y, z - other.z, w - other.w); }

		// Scalar multiplication operator
		Vector4 operator*(float scalar) const { return Vector4(x * scalar, y * scalar, z * scalar, w * scalar); }

		// Scalar division operator
		Vector4 operator/(float scalar) const { return Vector4{ x / scalar, y / scalar, z / scalar , w / scalar }; }

		// Dot product operator
		float operator*(const Vector4& other) const { return x * other.x + y * other.y + z * other.z + w * other.w; }

		//	
		Vector4 operator=(float* other) const { return Vector4{ other[0], other[1], other[2] , other[3] }; }

		//	
		Vector4& operator*=(const Vector4& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			w += other.w;
			return *this;
		}

		// Length (magnitude) of the vector
		float length() const { return std::sqrt(x * x + y * y + z * z + w * w); }

		// Normalize the vector
		void normalize()
		{
			float len = length();
			if (len > 0) {
				x /= len;
				y /= len;
				z /= len;
				w /= len;
			}
		}
	};

	struct Matrix2x3
	{
		Vector2 TopRow, MiddleRow, BottomRow;

		// Matrix addition operator
		Matrix2x3 operator+(const Matrix2x3& other) const { return Matrix2x3{ TopRow + other.TopRow, MiddleRow + other.MiddleRow, BottomRow + other.BottomRow }; }

		// Matrix subtraction operator
		Matrix2x3 operator-(const Matrix2x3& other) const { return Matrix2x3{ TopRow - other.TopRow, MiddleRow - other.MiddleRow, BottomRow - other.BottomRow }; }

		// Matrix-scalar multiplication operator
		Matrix2x3 operator*(float scalar) const { return Matrix2x3{ TopRow * scalar, MiddleRow * scalar, BottomRow * scalar }; }

		bool isZeroMatrix(const Matrix2x3& matrix) {
			if (matrix.TopRow.x == 0 && matrix.TopRow.y == 0 &&
				matrix.MiddleRow.x == 0 && matrix.MiddleRow.y == 0 &&
				matrix.BottomRow.x == 0 && matrix.BottomRow.y == 0) {
				return true;
			}
			return false;
		}
	};

	struct Matrix3x3
	{
		Vector3 TopRow, MiddleRow, BottomRow;

		// Matrix addition operator
		Matrix3x3 operator+(const Matrix3x3& other) const { return Matrix3x3{ TopRow + other.TopRow, MiddleRow + other.MiddleRow, BottomRow + other.BottomRow }; }

		// Matrix subtraction operator
		Matrix3x3 operator-(const Matrix3x3& other) const { return Matrix3x3{ TopRow - other.TopRow, MiddleRow - other.MiddleRow, BottomRow - other.BottomRow }; }

		// Matrix-scalar multiplication operator
		Matrix3x3 operator*(float scalar) const { return Matrix3x3{ TopRow * scalar, MiddleRow * scalar, BottomRow * scalar }; }

		bool isZeroMatrix(const Matrix3x3& matrix) {
			if (matrix.TopRow.x == 0 && matrix.TopRow.y == 0 && matrix.TopRow.z == 0 &&
				matrix.MiddleRow.x == 0 && matrix.MiddleRow.y == 0 && matrix.MiddleRow.z == 0 &&
				matrix.BottomRow.x == 0 && matrix.BottomRow.y == 0 && matrix.BottomRow.z == 0) {
				return true;
			}
			return false;
		}
	};

	struct Matrix4x3
	{
		Vector4 TopRow, MiddleRow, BottomRow;

		// Matrix addition operator
		Matrix4x3 operator+(const Matrix4x3& other) const { return Matrix4x3{ TopRow + other.TopRow, MiddleRow + other.MiddleRow, BottomRow + other.BottomRow }; }

		// Matrix subtraction operator
		Matrix4x3 operator-(const Matrix4x3& other) const { return Matrix4x3{ TopRow - other.TopRow, MiddleRow - other.MiddleRow, BottomRow - other.BottomRow }; }

		// Matrix-scalar multiplication operator
		Matrix4x3 operator*(float scalar) const { return Matrix4x3{ TopRow * scalar, MiddleRow * scalar, BottomRow * scalar }; }

		bool isZeroMatrix(const Matrix4x3& matrix) {
			if (matrix.TopRow.x == 0 && matrix.TopRow.y == 0 && matrix.TopRow.z == 0 && matrix.TopRow.w == 0 &&
				matrix.MiddleRow.x == 0 && matrix.MiddleRow.y == 0 && matrix.MiddleRow.z == 0 && matrix.MiddleRow.w == 0 &&
				matrix.BottomRow.x == 0 && matrix.BottomRow.y == 0 && matrix.BottomRow.z == 0 && matrix.BottomRow.w == 0) {
				return true;
			}
			return false;
		}
	};

	struct ProcessInfo
	{
		DWORD			m_ModulePID{};
		HANDLE			m_ModuleHandle{};
		uintptr_t		m_ModuleBase{};

		HWND			m_GameWindow{};
		std::string		m_WindowTitle;
		std::string		m_ClassName;
		std::string		m_ModulePath;
		int				m_WindowWidth;
		int				m_WindowHeight;
		Vector2			m_WindowSize{};
	};

	enum EConsoleColors : int 
	{
		dark_blue = 1,
		dark_green,
		dark_teal,
		dark_red,
		dark_pink,
		dark_yellow,
		dark_white,
		dark_gray,
		blue,
		green,
		teal,
		red,
		pink,
		yellow,
		white,
		DEFAULT = white,
	};

#pragma endregion

	//----------------------------------------------------------------------------------------------------
	//									[SECTION] CORE CLASSES
	//-----------------------------------------------------------------------------------

#pragma region	//	CORE CLASSES

	class CGlobals
    {
    public: 
        static class Console*           g_console;		//	
        static class Engine*            g_engine;		//	
        static class Memory*            g_memory;		//	
    };

	class Engine
	{
	public:

	public:
		static Engine* GetDefaultInstance();

	private:
		static Engine* m_instance;                     //  static class instance
	};

    class Console
    {

    public:
        static Console*                 GetDefaultInstance();
        static void                     LogMsgEx(FILE* stream, HANDLE pHand, const char* msg, EConsoleColors color, va_list args);      //  Logs message to the console
        static void                     LogMsg(const char* msg, ...);                                                                   //  Easily logs message to the console
        static void                     cLogMsg(const char* msg, EConsoleColors color, ...);                                            //  Logs a color message to the console
        static void                     SetViewState(bool isVisible);																	//  Show or hide window with true or false
        static void                     DestroyConsole();                                                                               //	Should be called at the end of program as part of cleanup , decontructor with static methods would just constantly deallocate the console. 

    public:
        Console();
        Console(const char* title);

    private:
        static FILE*                    m_pInStream;		            //  Console Text Input Stream
        static FILE*                    m_pOutStream;		            //  Console Text Output Stream
        static FILE*                    m_pErrStream;		            //  Console Text Error Stream
        static HANDLE                   m_pHandle;	                    //  Handle to Console
        static HWND	                    m_pWndw;		                //  Window Handle to Console
        static HANDLE                   m_pPipe;		                //	Handle to Pipe
        static bool                     m_isConsoleAllocated;           //  flag for console creation
        static bool                     m_isVisible;                    //  flag for displaying console window
        static std::mutex               m_mutex;                        //  lock to ensure thread safety
        static Console*                 m_instance;                     //  static class instance
    };

    class Memory
    {
    public:
		template <typename T>
		static inline T                 ReadMemoryEx(__int64 addr) { return *(T*)addr; }

		template<typename T>
		static inline void              WriteMemoryEx(__int64 addr, T patch) { *(T*)addr = patch; }

		template<typename fnc>
		static fnc						GetVFunction(const void* czInstance, size_t vfIndex)
		{
			auto vfTbl = *static_cast<const void***>(const_cast<void*>(czInstance));
			return reinterpret_cast<fnc>(const_cast<void(*)>(vfTbl[vfIndex]));
		}

		template<typename retType, typename... Args>
		static retType					CallVFunction(const void* czInstance, size_t vfIndex, Args&&... args)
		{
			const auto FUNCTION = GetVFunction<retType(*)(const void*, Args...)>(czInstance, vfIndex);
			return FUNCTION(czInstance, std::forward<Args>(args)...);
		}

		template<typename OrigFunc>
		static void						hkVFunction(void* instance, int index, OrigFunc& originalFunc, void* newFunc)
		{
			DWORD old;
			auto vTable = *reinterpret_cast<__int64**>(instance);
			originalFunc = reinterpret_cast<OrigFunc>(vTable[index]);
			VirtualProtect(&vTable[index], sizeof(void*), PAGE_EXECUTE_READWRITE, &old);
			vTable[index] = reinterpret_cast<__int64>(newFunc);
			VirtualProtect(&vTable[index], sizeof(void*), old, &old);
		}

		template<typename OrigFunc>
		static void						hkRestoreVFunction(void* instance, int index, OrigFunc originalFunc)
		{
			DWORD old;
			auto vTable = *reinterpret_cast<__int64**>(instance);
			VirtualProtect(&vTable[index], sizeof(void*), PAGE_EXECUTE_READWRITE, &old);
			vTable[index] = reinterpret_cast<__int64>(originalFunc);
			VirtualProtect(&vTable[index], sizeof(void*), old, &old);
		}

    public:
        static Memory*                  GetDefaultInstance();
        static __int64                  GetModuleBase();                            //  returns the module base of PCSX2 Process
        static __int64                  GetAddr(unsigned int offset);               //  returns address offset from PCSX2 module base
        static bool                     ObtainProcessInfo(ProcessInfo& pInfo);      //  resolves information on a windows process ( PID, Handle, BaseAddr & Base Game Window Information )
        static __int64                  ResolvePtrChain(__int64 addr, std::vector<unsigned int> offsets = {});
        static bool                     BytePatch(__int64 Address, BYTE* bytes, unsigned int size);
        static bool                     NopBytes(__int64 Address, unsigned int size);
		static unsigned int				GetVtblOffset(void* czInstance, const char* dwModule = NULL);
		static int						GetVtblIndex(void* fncPtr, void* vTblAddr);
		static __int64					SignatureScan(const char* sig);
		static std::vector<int>			SignatureToBytes(const char* sig);

    public:
        Memory();
        ~Memory();
        
        friend class PS2Memory;
    
    private:
        static bool                     m_isInitialized;                                //  Set on first constructor call
        static __int64                  dwGameBase;                                     //  Process Module Base
        static __int64                  dwEEMem;                                        //  EEMem Pointer
        static __int64                  BasePS2MemorySpace;                             //  EEMem Base Address
        static ProcessInfo              Process;                                        //  Process Information Struct
        static Memory*                  m_instance;                                     //  static class instance
    };

    class PS2Memory
    {
    public:

        /*
            Summary: takes an input long address and reads its value
            Notes:
                -   'long' in this sense would be the physical address in memory such as '00007FF6C194DC00'

            Example Usage: Get float Position of Object
                -   PS2Memory::ReadLong<float>(address);
            
            Example Usage: Get Vector3 Position of Object
                -   PS2Memory::ReadLong<Vector3>(address)
        */
        template<typename T>
        static inline T                 ReadLong(__int64 addr) { return *(T*)(addr); }


        /*
            Summary: takes an input long address and writes a new value
            Notes:
                -   'short' in this sense would be the game offset such as'0x4899F0'

            Example Usage: Write new float Position of Object
                -   PS2Memory::WriteLong<float>(offset, patch);

            Example Usage: Write new Vector3 Position of Object
                -   PS2Memory::WriteLong<Vector3>(offset, patch)
        */
        template<typename T>
        static inline void              WriteLong(__int64 addr, T Patch) { *(T*)addr = Patch; }
        
        /*
            Summary: takes an input short offset , transforms it to an address in memory and reads its value
            Notes:
                -   'short' in this sense would be the game offset such as'0x4899F0'

            Example Usage: Get float Position of Object
                -   PS2Memory::ReadShort<float>(offset);

            Example Usage: Get Vector3 Position of Object
                -   PS2Memory::ReadShort<Vector3>(offset)
        */
        template <typename T>
        static inline T                 ReadShort(__int32 offset)  { return ReadLong<T>(GetAddr(offset)); }

        /*
            Summary: takes an input short offset , transforms it to an address in memory and writes a new value
            Notes:
                -   'short' in this sense would be the game offset such as'0x4899F0'

            Example Usage: write new float Position of Object
                -   PS2Memory::WriteShort<float>(offset, patch);

            Example Usage: Write new Vector3 Position of Object
                -   PS2Memory::WriteShort<Vector3>(offset, patch)
        */
        template <typename T>
        static inline void              WriteShort(__int32 offset, T patch) { WriteLong<T>(GetAddr(offset), patch); }

        //  takes an input long address , reads the value and casts it to a class pointer
        template <typename T>
        static inline T                GetPtrLong(long long address)
        {
            return reinterpret_cast<T>(GetModuleBase() + ReadLong<__int32>(address));
        }

        //  takes an input short offset , transforms it to an address in memory, reads its value and casts it to a class pointer 
        template <typename T>
        static inline T                GetPtrShort(__int32 offset)
        {
            return reinterpret_cast<T>(GetModuleBase() + ReadShort<__int32>(offset));
        }


    public:
        static __int64                  GetModuleBase();                    //  obtain PS2 EE memory base address. 00007FF6C0000000 
        static __int64                  GetAddr(__int32 offset);            //  transform offset to physical address. 0x44D648 -> 00007FF6C44D648      
        static __int64                  ResolvePtrChain(__int32 base_offset, std::vector<__int32> offsets);
    };

    class Tools
    {
    public:
        
        static bool                     GetKeyState(WORD v, SHORT delta);
        static bool                     GamepadGetButtonState(WORD button);


        class CPUTimer
        {
        public:
            //  Enumeration for specifying different timing units
            enum class ETimings
            {
                ETiming_MS,     //  millisecond
                ETiming_S,      //  second
                ETiming_M,      //  minute
                ETiming_HR,     //  hour
            };

        public:
            void                        Start();            //  Start the timer
            void                        Stop();             //  Stop the timer
            void                        Reset();            //  Reset the timer to start again
            double                      Stop(ETimings t);   // Stop the timer and 
            double                      GetElapsedTime(clock_t time, ETimings t = ETimings::ETiming_MS) const;  //  return the elapsed time in the specified unit //  NOTE: timer is not stopped

        public:
            CPUTimer();                                     // Constructor - starts the timer

        private:
            clock_t                     m_start;            // Start time of the timer
            clock_t                     m_end{ 0 };         // End time of the timer
        };

        class Math3D
        {
            static float GetDistanceTo3DObject(Vector3 POS, Vector3 POS2);
        };

    };

#pragma endregion

	//----------------------------------------------------------------------------------------------------
	//									[SECTION] PCSX2 METHODS
	//-----------------------------------------------------------------------------------

#pragma region	//	PCSX2 METHODS


#pragma endregion

	//----------------------------------------------------------------------------------------------------
	//									[SECTION] PCSX2 STRUCTS & ENUMS
	//-----------------------------------------------------------------------------------

#pragma region	//	PCSX2 STRUCTS & ENUMS


	/*
	* PCSX2 Source
		Console.h - Ln 22
	*/
	enum PCSX2ConsoleColors
	{
		Color_Current = -1,

		Color_Default = 0,

		Color_Black,
		Color_Green,
		Color_Red,
		Color_Blue,
		Color_Magenta,
		Color_Orange,
		Color_Gray,

		Color_Cyan,				// faint visibility, intended for logging PS2/IOP output
		Color_Yellow,			// faint visibility, intended for logging PS2/IOP output
		Color_White,			// faint visibility, intended for logging PS2/IOP output

		// Strong text *may* result in mis-aligned text in the console, 
		// depending on the font and the platform, so use these with caution.
		Color_StrongBlack,
		Color_StrongRed,		// intended for errors
		Color_StrongGreen,		// intended for infrequent state information
		Color_StrongBlue,		// intended for block headings
		Color_StrongMagenta,
		Color_StrongOrange,		// intended for warnings
		Color_StrongGray,

		Color_StrongCyan,
		Color_StrongYellow,
		Color_StrongWhite,

		ConsoleColors_Count
	};
	static const PCSX2ConsoleColors DefaultConsoleColor = Color_Default;

	//	
	enum DXGI : int
	{
		IDXGI_PRESENT = 8,
		IDXGI_DRAW_INDEXED = 12,
		IDXGI_RESIZE_BUFFERS = 13,
	};

	//	
	enum class RenderAPI
	{
		None,
		D3D11,
		Metal,
		D3D12,
		Vulkan,
		OpenGL
	};

	struct GPR_reg
	{
		//	char pad_0000[16];	//0x0000
		__int32 n[4];

	};	//Size: 0x0010

	struct E_GPRRegs
	{
		GPR_reg r0;	//0x0000
		GPR_reg at;	//0x0010
		GPR_reg v0;	//0x0020
		GPR_reg v1;	//0x0030
		GPR_reg a0;	//0x0040
		GPR_reg a1;	//0x0050
		GPR_reg a2;	//0x0060
		GPR_reg a3;	//0x0070
		GPR_reg t0;	//0x0080
		GPR_reg t1;	//0x0090
		GPR_reg t2;	//0x00A0
		GPR_reg t3;	//0x00B0
		GPR_reg t4;	//0x00C0
		GPR_reg t5;	//0x00D0
		GPR_reg t6;	//0x00E0
		GPR_reg t7;	//0x00F0
		GPR_reg s0;	//0x0100
		GPR_reg s1;	//0x0110
		GPR_reg s2;	//0x0120
		GPR_reg s3;	//0x0130
		GPR_reg s4;	//0x0140
		GPR_reg s5;	//0x0150
		GPR_reg s6;	//0x0160
		GPR_reg s7;	//0x0170
		GPR_reg t8;	//0x0180
		GPR_reg t9;	//0x0190
		GPR_reg k0;	//0x01A0
		GPR_reg k1;	//0x01B0
		GPR_reg gp;	//0x01C0
		GPR_reg sp;	//0x01D0
		GPR_reg s8;	//0x01E0
		GPR_reg ra;	//0x01F0
	};	//Size: 0x0200

	struct E_CP0regs
	{
		__int32 Index;		//0x0000
		__int32 Random;		//0x0004
		__int32 EntryLo0;	//0x0008
		__int32 EntryLo1;	//0x000C
		__int32 Context;	//0x0010
		__int32 PageMask;	//0x0014
		__int32 Wired;		//0x0018
		__int32 Reserved0;	//0x001C
		__int32 BadVAddr;	//0x0020
		__int32 Count;		//0x0024
		__int32 EntryHi;	//0x0028
		__int32 Compare;	//0x002C
		__int32 Status;		//0x0030	//	bitfield
		__int32 Cause;		//0x0034
		__int32 EPC;		//0x0038
		__int32 PRid;		//0x003C
		__int32 Config;		//0x0040
		__int32 LLAddr;		//0x0044
		__int32 WatchLO;	//0x0048
		__int32 WatchHI;	//0x004C
		__int32 XContext;	//0x0050
		__int32 Reserved1;	//0x0054
		__int32 Reserved2;	//0x0058
		__int32 Debug;		//0x005C
		__int32 DEPC;		//0x0060
		__int32 PerfCnt;	//0x0064
		__int32 ErrCtl;		//0x0068
		__int32 CachErr;	//0x006C
		__int32 TagLo;		//0x0070
		__int32 TagHi;		//0x0074
		__int32 ErrorEPC;	//0x0078
		__int32 DESAVE;		//0x007C
	};	//Size: 0x0080

	struct PERFregs
	{
		unsigned int r[4];	//0x0000
	};	//Size: 0x0010


	/*
		iR5900
		EE Register Information
		comments & structures thanks to pcsx2 team
	*/
	struct cpuRegisters
	{
		E_GPRRegs GPR;				//0x0000
		GPR_reg HI;					//0x0200
		GPR_reg LO;					//0x0210
		E_CP0regs CP0;				//0x0220
		__int32 sa;					//0x02A0
		__int32 IsDelaySlot;		//0x02A4
		__int32 pc;					//0x02A8
		__int32 code;				//0x02AC
		PERFregs PERF;				//0x02B0
		__int32 eCycle[32];			//0x02C0
		__int32 sCycle[32];			//0x0340
		__int32 cycle;				//0x03C0
		__int32 interrupt;			//0x03C4
		__int32 branch;				//0x03C8
		__int32 opmode;				//0x03CC
		__int32 tempcycles;			//0x03D0
		__int32 dmastall;			//0x03D4
		__int32 pcWriteback;		//0x03D8
		__int32 nextEventCycle;		//0x03DC
		__int32 lastEventCycle;		//0x03E0
		__int32 lastCOP0Cycle;		//0x03E4
		__int32 LastPERFCycle[2];	//0x03E8
	};	//Size: 0x03F0

	struct GPRRegs
	{
		__int32 r0;	//0x0000
		__int32 at;	//0x0004
		__int32 v0;	//0x0008
		__int32 v1;	//0x000C
		__int32 a0;	//0x0010
		__int32 a1;	//0x0014
		__int32 a2;	//0x0018
		__int32 a3;	//0x001C
		__int32 t0;	//0x0020
		__int32 t1;	//0x0024
		__int32 t2;	//0x0028
		__int32 t3;	//0x002C
		__int32 t4;	//0x0030
		__int32 t5;	//0x0034
		__int32 t6;	//0x0038
		__int32 t7;	//0x003C
		__int32 s0;	//0x0040
		__int32 s1;	//0x0044
		__int32 s2;	//0x0048
		__int32 s3;	//0x004C
		__int32 s4;	//0x0050
		__int32 s5;	//0x0054
		__int32 s6;	//0x0058
		__int32 s7;	//0x005C
		__int32 t8;	//0x0060
		__int32 t9;	//0x0064
		__int32 k0;	//0x0068
		__int32 k1;	//0x006C
		__int32 gp;	//0x0070
		__int32 sp;	//0x0074
		__int32 s8;	//0x0078
		__int32 ra;	//0x007C
		__int32 hi;	//0x0080
		__int32 lo;	//0x0084
	};	//Size: 0x0088

	struct CP0Regs
	{
		__int32 Index;			//0x0000
		__int32 Random;			//0x0004
		__int32 EntryLo0;		//0x0008
		__int32 EntryLo1;		//0x000C
		__int32 Context;		//0x0010
		__int32 PageMask;		//0x0014
		__int32 Wired;			//0x0018
		__int32 Reserved0;		//0x001C
		__int32 BadVAddr;		//0x0020
		__int32 Count;			//0x0024
		__int32 EntryHi;		//0x0028
		__int32 Compare;		//0x002C
		__int32 Status;			//0x0030
		__int32 Cause;			//0x0034
		__int32 EPC;			//0x0038
		__int32 PRid;			//0x003C
		__int32 Config;			//0x0040
		__int32 LLAddr;			//0x0044
		__int32 WatchLO;		//0x0048
		__int32 WatchHI;		//0x004C
		__int32 XContext;		//0x0050
		__int32 Reserved1;		//0x0054
		__int32 Reserved2;		//0x0058
		__int32 Reserved3;		//0x005C
		__int32 Reserved4;		//0x0060
		__int32 Reserved5;		//0x0064
		__int32 ECC;			//0x0068
		__int32 CacheErr;		//0x006C
		__int32 TagLo;			//0x0070
		__int32 TagHi;			//0x0074
		__int32 ErrorEPC;		//0x0078
		__int32 Reserved6;		//0x007C
	};	//Size: 0x0080

	struct CP2Data
	{
		__int16 v0[4];			//0x0000
		__int16 v1[4];			//0x0008
		__int16 v2[4];			//0x0010
		__int8 rgb[4];			//0x0018
		__int32 otz;			//0x001C
		__int32 ir0;			//0x0020
		__int32 ir1;			//0x0024
		__int32 ir2;			//0x0028
		__int32 ir3;			//0x002C
		__int16 sxy0[2];		//0x0030
		__int16 sxy1[2];		//0x0034
		__int16 sxy2[2];		//0x0038
		__int16 sxyp[2];		//0x003C
		__int16 sz0[2];			//0x0040
		__int16 sz1[2];			//0x0044
		__int16 sz2[2];			//0x0048
		__int16 sz3[2];			//0x004C
		__int8 rgb0[4];			//0x0050
		__int8 rgb1[4];			//0x0054
		__int8 rgb2[4];			//0x0058
		__int32 reserved;		//0x005C
		__int32 mac0;			//0x0060
		__int32 mac1;			//0x0064
		__int32 mac2;			//0x0068
		__int32 mac3;			//0x006C
		__int32 irgb;			//0x0070
		__int32 orgb;			//0x0074
		__int32 lzcs;			//0x0078
		__int32 lzcr;			//0x007C
	};	//Size: 0x0080

	struct CP2Ctrl
	{
		unsigned __int16 rMatrix[10];	//0x0000
		__int32 trX;	//0x0014
		__int32 trY;	//0x0018
		__int32 trZ;	//0x001C
		unsigned __int16 lMatrix[10];	//0x0020
		__int32 rbk;	//0x0034
		__int32 gbk;	//0x0038
		__int32 bbk;	//0x003C
		unsigned __int16 cMatrix[10];	//0x0040
		__int32 rfc;	//0x0054
		__int32 gfc;	//0x0058
		__int32 bfc;	//0x005C
		__int32 ofx;	//0x0060
		__int32 ofy;	//0x0064
		__int32 h;		//0x0068
		__int32 dqa;	//0x006C
		__int32 dqb;	//0x0070
		__int32 zsf3;	//0x0074
		__int32 zsf4;	//0x0078
		__int32 flag;	//0x007C
	};	//Size: 0x0080

	/*
		iR3000A
		IOP Register Information
		comments & structures thanks to pcsx2 team
	*/
	struct psxRegisters
	{
		GPRRegs GPR;				//	0x0000	//	General Purpose Rehisters
		CP0Regs CP0;				//	0x0088	//	Coprocessor0 Registers
		CP2Data CP2D;				//	0x0108	//	Cop2 data registers
		CP2Ctrl CP2C;				//	0x0188	//	Cop2 control registers
		__int32 pc;					//	0x0208	//	Program counter
		__int32 code;				//	0x020C	//	The instruction
		__int32 cycle;				//	0x0210	//	
		__int32 interrupt;			//	0x0214	//	
		__int32 pcWriteback;		//	0x0218	//	
		__int32 iopNextEventCycle;	//	0x021C	//	Controls when branch tests are performed.
		
		// This value is used when the IOP execution is broken to return control to the EE.
		// (which happens when the IOP throws EE-bound interrupts).  It holds the value of
		// iopCycleEE (which is set to zero to facilitate the code break), so that the unrun
		// cycles can be accounted for later.
		__int32 iopBreak;			//	0x0220	


		// Tracks current number of cycles IOP can run in EE cycles. When it dips below zero,
		// control is returned to the EE.
		__int32 iopCycleEE;			//	0x0224

		__int32 sCycle[32];			//	0x0228	//	start cycle for signaled ints
		__int32 eCycle[32];			//	0x022C	//	 cycle delta for signaled ints
	};	//Size: 0x021C

#pragma endregion

	//----------------------------------------------------------------------------------------------------
	//									[SECTION] PCSX2 CLASSES
	//-----------------------------------------------------------------------------------

#pragma region	//	PCSX2 CLASSES

	/*
		PCSX2 : 
	*/
	class PCSX2
    {	
    public:
        static unsigned int o_gs_device;                                                //  global pointer to GSDevice  -> PCSX2 v1.7.5617: 0x3FA2728	[ AOB: 48 8B 05 ? ? ? ? 83 78 ? ? 74 ? 8B 3D ]
        static class GSDevice* g_gs_device;                                             //  AOB: 48 8B 0D ? ? ? ? 48 85 C9 74 20
        
        //	function offsets
		static unsigned int o_GSUpdateDisplayWindow;                                    //  offset to function  ->  GSDevice::vfIndex [12]		[ AOB: 48 83 EC ? 48 8B 0D ? ? ? ? 48 8B 01 ]
        static unsigned int o_GSDevice_GetRenderAPI;                                    //  offset to function  ->  GSDevice::vfIndex [9]		[ AOB: B8 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC CC 41 57 41 56 41 55 ]
		static unsigned int o_recompileNextInstruction;                                 //  offset to function  ->  PCSX2 v1.7.5617: 0x291CA0
        static unsigned int o_psxRecompileInstruction;                                  //  offset to function  ->  PCSX2 v1.7.5617: 0x269D80
        static unsigned int o_recResetEE;                                               //  offset to function	->	PCSX2 v1.7.5617: 0x2942C0

        /*
            //  Function: recompileNextInstruction
            //  AOB: [ Nightly AOB: E8 ? ? ? ? C7 44 24 ? ? ? ? ? 49 ]  [ Source AOB: ~ ]
            //  .text:000000014029313B                 mov     cs:dword_142EA8F2C, eax
            //  dword_142EA8F2C = v157;;           // cpuRegs.code // 0x2AC

			
			//	sub_118A0("Branch %x in delay slot!", v88); v88 = cpuRegs.code, sub_118A0 = DevCon.Warning, executing function = recompileNextInstruction
			//	DevCon.Warning("Branch %x in delay slot!", cpuRegs.code);

        */
        static unsigned int o_cpuRegs;      //      offset  ->  PCSX2 v1.7.5617: 0x2EA8F2C	[ AOB: 8B 05 ? ? ? ? 89 C2 C1 EA ? 83 E2 ? 8D 4A ]
        static cpuRegisters* g_cpuRegs;     //      iR5900
        static __int32 g_cpupc;             //      offset  ->  PCSX2 v1.7.5617: 0      //  @TODO:have not determined a method for obtaining


        /*

            //  Function: psxRecompileNextInstruction
            //  AOB: [ Nightly AOB: E8 ? ? ? ? 8B 05 ? ? ? ? 8B 0D ? ? ? ? 85 ]  [ Source AOB: E8 ? ? ? ? 8B 15 ? ? ? ? 85 D2 75 ]
            //  .text:0000000140269D96                 mov     r15d, cs:dword_142EA809C
            //  v3 = dword_142EA809C;           // psxRegs.code // 0x20C

			//	sub_6820("psx: Unimplemented op %x", (unsigned int)dword_2EA809C); dword_2EA809C = psxRegs.code, sub
        */
        static unsigned int o_psxRegs;      //      offset  ->  PCSX2 v1.7.5617: 0x2EA809C	[	AOB: 8B 05 ? ? ? ? A9 ? ? ? ? 74 ? 89 C1 C1 E9 ]
        static psxRegisters* g_psxRegs;     //      iR3000A
        static __int32 g_psxpc;             //      offset  ->  PCSX2 v1.7.5617: 0      //  @TODO:have not determined a method for obtaining

		//	fn Prototypes
		typedef __int64(__fastcall* GSUpdateDisplayWindow_stub)();								//  [ AOB: 48 83 EC ? 48 8B 0D ? ? ? ? 48 8B 01 ]
		typedef __int8(__fastcall* GSDevice_GetRenderAPI_stub)(class GSDevice*);				//  GSDevice Virtual Method #6 Returns the graphics API used by this device.
		typedef void(__fastcall* recompileNextInstruction_stub)(bool, bool);					//  [ AOB: 41 57 41 56 41 55 41 54 56 57 55 53 48 81 EC ? ? ? ? 0F 29 74 24 ? 88 54 24 ] [ string: xref "Applying Dynamic Patch to address 0x%08X" ]
		typedef void(__fastcall* psxRecompileNextInstruction_stub)(bool, bool);					//  [ AOB: 41 57 41 56 41 55 41 54 56 57 55 53 48 81 EC ? ? ? ? 41 89 D5 44 8B 3D ]
		typedef void(__fastcall* recResetEE_stub)();											//  [ AOB: 80 3D ? ? ? ? ? 75 ? C6 05 ? ? ? ? ? C6 05 ? ? ? ? ? 80 3D ]
		typedef void(__fastcall* recResetIOP_stub)();											//  [ AOB: 41 57 41 56 41 55 41 54 56 57 55 53 48 83 EC ? 66 0F 7F 7C 24 ? 66 0F 7F 74 24 ? 49 BE ]
		static void ResetEE();																	//  helper function utilizing the offset and prototype fn	@TODO::relocate ? ?
		static void ResetIOP();																	//  helper function utilizing the offset and prototype fn	@TODO::relocate ? ?

		//	maintaining this might get annoying , future revision should just keep IWriterColor and set STRONG_ORANGE | STRONG_RED for warning & error
		typedef __int64(__fastcall* IConsoleWriter_WriteLn_stub)(const char*, ...);				//	[ AOB: 48 83 EC ? 48 89 C8 48 89 54 24 ? 4C 89 44 24 ? 4C 89 4C 24 ? 4C 8D 4C 24 ? 4C 89 4C 24 ? B9 ? ? ? ? 31 D2 49 89 C0 E8 ? ? ? ? 90 48 83 C4 ? C3 CC CC CC CC CC CC CC CC CC CC CC 48 83 EC ]
		static IConsoleWriter_WriteLn_stub WriteLn;

		typedef __int64(__fastcall* IConsoleWriter_cWriteLn_stub)(int, const char*, ...);		//	[ AOB: 48 83 EC ? 48 89 D0 89 CA 4C 89 44 24 ? 4C 89 4C 24 ? 4C 8D 4C 24 ? 4C 89 4C 24 ? B9 ? ? ? ? 49 89 C0 E8 ? ? ? ? 90 48 83 C4 ? C3 48 83 EC ]
		static IConsoleWriter_cWriteLn_stub WriteColorLn;

		typedef __int64(__fastcall* IConsoleWriter_Warning_stub)(const char*, ...);				//	[ AOB: 48 83 EC ? 48 89 C8 48 89 54 24 ? 4C 89 44 24 ? 4C 89 4C 24 ? 4C 8D 4C 24 ? 4C 89 4C 24 ? B9 ? ? ? ? BA ? ? ? ? 49 89 C0 E8 ? ? ? ? 90 48 83 C4 ? C3 CC CC CC CC CC CC CC CC 56 57 ]	
		static IConsoleWriter_Warning_stub WriteWarning;

		typedef __int64(__fastcall* IConsoleWriter_Error_stub)(const char*, ...);				//	[ AOB: 48 83 EC ? 48 89 C8 48 89 54 24 ? 4C 89 44 24 ? 4C 89 4C 24 ? 4C 8D 4C 24 ? 4C 89 4C 24 ? B9 ? ? ? ? BA ? ? ? ? 49 89 C0 E8 ? ? ? ? 90 48 83 C4 ? C3 CC CC CC CC CC CC CC CC 41 57 41 56 41 55 ]	//	Console.Error("EE: Unrecognized op %x", cpuRegs.code);
		static IConsoleWriter_Error_stub WriteError;

		typedef __int64(__fastcall* IConsoleWriterDev_WriteLn_stub)(const char*, ...);			//	[ AOB: 48 83 EC ? 48 89 C8 48 89 54 24 ? 4C 89 44 24 ? 4C 89 4C 24 ? 4C 8D 4C 24 ? 4C 89 4C 24 ? B9 ? ? ? ? 31 D2 49 89 C0 E8 ? ? ? ? 90 48 83 C4 ? C3 CC CC CC CC CC CC CC CC CC CC CC 41 57 ]			//	recResetIOP
		static IConsoleWriterDev_WriteLn_stub DevWriteLn;

		typedef __int64(__fastcall* IConsoleWriterDev_cWriteLn_stub)(int, const char*, ...);	//	[ AOB: 48 83 EC ? 48 89 D0 89 CA 4C 89 44 24 ? 4C 89 4C 24 ? 4C 8D 4C 24 ? 4C 89 4C 24 ? B9 ? ? ? ? 49 89 C0 E8 ? ? ? ? 90 48 83 C4 ? C3 56 ]															//	DevCon.WriteLn(Color_Gray, "[R3000 Debugger] Branch to 0x890 (SYSMEM). Clearing modules."); // iopRecRecompile
		static IConsoleWriterDev_cWriteLn_stub DevColorWriteLn;

		typedef __int64(__fastcall* IConsoleWriterDev_Warning_stub)(const char*, ...);			//	[ AOB: 48 83 EC ? 48 89 C8 48 89 54 24 ? 4C 89 44 24 ? 4C 89 4C 24 ? 4C 8D 4C 24 ? 4C 89 4C 24 ? B9 ? ? ? ? BA ? ? ? ? 49 89 C0 E8 ? ? ? ? 90 48 83 C4 ? C3 CC CC CC CC CC CC CC CC 56 48 81 EC ]	
		static IConsoleWriterDev_Warning_stub DevWriteWarning;

		typedef __int64(__fastcall* IConsoleWriterDev_Error_stub)(const char*, ...);			//	[ AOB: 48 83 EC ? 48 89 C8 48 89 54 24 ? 4C 89 44 24 ? 4C 89 4C 24 ? 4C 8D 4C 24 ? 4C 89 4C 24 ? B9 ? ? ? ? BA ? ? ? ? 49 89 C0 E8 ? ? ? ? 90 48 83 C4 ? C3 CC CC CC CC CC CC CC CC 41 57 41 56 56 ]	//	DevCon.Error("[IOP] Impossible block clearing failure");
		static IConsoleWriterDev_Error_stub DevWriteError;

		/*	GENERAL RENDERER HOOKING METHODS
		*
		*/
		typedef void (*RenderGUI)();												//	user render declaration
		typedef HRESULT(WINAPI* IDXGISwapChainPresent)(IDXGISwapChain*, UINT, UINT);
		typedef HRESULT(WINAPI* ResizeBuffers)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
		static HRESULT hkPresent(IDXGISwapChain*, UINT, UINT);
		static HRESULT hkResizeBuffers(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
		static LRESULT WndProc(const HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
		static void RegisterRenderFunction(RenderGUI p);		//	used to link a function to be executed in hkPresent
		static void ClearRenderFunction();						//	used to link a function to be executed in hkPresent
		static RenderGUI fn_Render;								//	called in hkPresent
		static IDXGISwapChainPresent oDXGISwapChainPresent;
		static ResizeBuffers oIDXGIResizeBuffers;
		static ID3D11RenderTargetView* D11RenderTarget;
		static ID3D11DeviceContext* D11DeviceCtx;
		static IDXGISwapChain* pSwapChain;
		static ID3D11Device* D3D11Device;
		static WNDPROC oWndProc;
		static HWND RenderWindow;
		static bool isImGuiInit;
		static bool isMenuShown;

		/*	STEAM GAME OVERLAY RENDERER
		* Module: GameOverlayRenderer64.dll
			NOTE: PCSX2 Must be launched with steam for the module to be populated
		*/
		typedef __int64(__fastcall* steamCreateHook_stub)(unsigned __int64, __int64, __int64*, int);				//	48 89 5C 24 ? 57 48 83 EC ? 33 C0 48 89 44 24
		typedef __int64(__fastcall* steamhkRelease11)(__int64);														//	48 89 5C 24 ? 57 48 83 EC ? 48 8B 05 ? ? ? ? 48 8B F9 FF D0 48 8B D7
		typedef __int64(__fastcall* steamhkPresent11)(__int64, unsigned int, unsigned int);							//	48 89 6C 24 ? 48 89 74 24 ? 41 56 48 83 EC ? 41 8B E8
		typedef __int64(__fastcall* steamhkSetFullscreenState11)(__int64, unsigned int, __int64);					//	48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 83 EC ? 44 8B F2
		typedef __int64(__fastcall* steamhkResizeBuffers11)(__int64, unsigned int, unsigned int, unsigned int);		//	48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC ? 44 8B FA


		/*	DISCORD OVERLAY RENDERER
		* Module: DiscordHook64.dll
			NOTE: Discord must both be running & have permissions to execute an overlay

		*/
		static unsigned int g_discordPresemt;	//	48 8B 05 ? ? ? ? 48 89 D9 44 89 FA 41 89 F0 FF 15 ? ? ? ? 89 C7 89 F8

        //  DEPRECATED: pcsx2 1.6
    private:    //  ~
        uintptr_t hk_OnLeftDClick   = NULL; //  Signature("48 8B 05 ? ? ? ? 80 B8 ? ? ? ? ? 74 0C").Scan().As<uint64_t>();
        uintptr_t hk_ResetEE        = NULL; //  Signature("80 3D ? ? ? ? ? 74 13 B8 ? ? ? ? 86").Scan().As<uint64_t>();	// 80 3D ? ? ? ? ? 75 30 C6 05 ? ? ? ? ? C6

		unsigned int o_gs_renderer;												//  global pointer to GRenderer  -> PCSX2 v1.7.5617: 0x0
		class GSRenderer* g_gs_renderer;											//	AOB: ? ? ?

		unsigned int o_g_emu_thread;                                             //  global pointer to GEmu  -> PCSX2 v1.7.5617: 0x0
		class EmuThread* g_emu_thread;											//	AOB: ? ? ?

	};

	/*
		GSRenderer : 
	*/
    class GSRenderer
    {

    };

	/*
		GSRendererHW : 
	*/
    class GSRendererHW
    {
    };

	/*
		GSDevice : Rendering API
	*/
    class GSDevice
    {
    private:
        //  - No member variables can be placed in this class due to spacing.
        //  -- instead functions will be used to obtain variables as this does not change the size of the class or inherited classes.

    public:
        static RenderAPI                GetRenderAPI();         //  Obtain the current render api. Should be used to get the current class instance of GDevice


	public:
   
	private:
        virtual void                    vf_CreateSurface();
        virtual void                    vf_Function1();
        virtual void                    vf_Function2();
        virtual void                    vf_DoFXAA();
        virtual void                    vf_Function4();
        virtual void                    vf_Function5();
        virtual void                    vf_Function6();
        virtual void                    vf_Create();
        virtual void                    vf_Destroy();
        virtual void                    vf_GetRenderAPI();
        virtual void                    vf_HasSurface();
        virtual void                    vf_Function11();
        virtual void                    vf_UpdateWindow();
        virtual void                    vf_ResizeWindow();
        virtual void                    vf_SupportsExclusiveFullscreen();
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
    };

    /*
        GSDevice11 : DirectX11 Rendering API
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
    };  //Size: 0x0138

    /*
        GSDevice12 : DirectX12 Rendering API
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
    */
    class GSDeviceOGL : public GSDevice
    {

    };

    /*
        GSDeviceMetal : Metal Rendering API
    */
    class GSDeviceMetal : public GSDevice
    {
        
    };

    /*
        GSDeviceVK : Vulkan Rendering API
    */
    class GSDeviceVK : public GSDevice
    {

    };

#pragma endregion

}