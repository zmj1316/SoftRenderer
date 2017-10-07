#include "MYUT.hpp"
#include "helpers.hpp"

//--------------------------------------------------------------------------------------
// Thread safety
//--------------------------------------------------------------------------------------
CRITICAL_SECTION g_cs;
bool g_bThreadSafe = true;


//--------------------------------------------------------------------------------------
// Automatically enters & leaves the CS upon object creation/deletion
//--------------------------------------------------------------------------------------
class DXUTLock
{
public:
	DXUTLock() { if (g_bThreadSafe) EnterCriticalSection(&g_cs); }
	~DXUTLock() { if (g_bThreadSafe) LeaveCriticalSection(&g_cs); }
};

//--------------------------------------------------------------------------------------
// Helper macros to build member functions that access member variables with thread safety
//--------------------------------------------------------------------------------------
#define SET_ACCESSOR( x, y )       inline void Set##y( x t )   { DXUTLock l; m_state.m_##y = t; };
#define GET_ACCESSOR( x, y )       inline x Get##y()           { DXUTLock l; return m_state.m_##y; };
#define GET_SET_ACCESSOR( x, y )   SET_ACCESSOR( x, y ) GET_ACCESSOR( x, y )

#define SETP_ACCESSOR( x, y )      inline void Set##y( x* t )  { DXUTLock l; m_state.m_##y = *t; };
#define GETP_ACCESSOR( x, y )      inline x* Get##y()          { DXUTLock l; return &m_state.m_##y; };
#define GETP_SETP_ACCESSOR( x, y ) SETP_ACCESSOR( x, y ) GETP_ACCESSOR( x, y )


class MYUTState
{
protected:
	struct STATE
	{
		HWND m_HWNDDeviceWindowed; // the main app focus window
		LPMYUTCALLBACKMSGPROC m_WindowMsgFunc;
		void* m_WindowMsgFuncUserContext;
		LPMYUTCALLBACKDRAW m_DrawFunc;
		bool m_InSizeMove;
		LPMYUTCALLBACKRESIZE m_ResizeFunc;
		bool m_Keys[256];
	};

	STATE m_state;

public:
	MYUTState() { Create(); }
	~MYUTState() { Destroy(); }

	void Create()
	{
		g_bThreadSafe = true;
		InitializeCriticalSectionAndSpinCount(&g_cs, 1000);
		ZeroMemory(&m_state, sizeof(STATE));
	}

	void Destroy()
	{
		DeleteCriticalSection(&g_cs);
	}

	GET_SET_ACCESSOR(HWND, HWNDDeviceWindowed);
	GET_SET_ACCESSOR(LPMYUTCALLBACKMSGPROC, WindowMsgFunc);
	GET_SET_ACCESSOR(void*, WindowMsgFuncUserContext);
	GET_SET_ACCESSOR(LPMYUTCALLBACKDRAW, DrawFunc);
	GET_SET_ACCESSOR(LPMYUTCALLBACKRESIZE, ResizeFunc);
	GET_SET_ACCESSOR(bool, InSizeMove);


	GET_ACCESSOR(bool*, Keys);
};

MYUTState* g_pMYUTState = nullptr;

HRESULT WINAPI MYUTCreateState()
{
	if (g_pMYUTState == nullptr)
	{
		g_pMYUTState = new MYUTState;
		if (nullptr == g_pMYUTState)
			return E_OUTOFMEMORY;
	}
	return S_OK;
}

void WINAPI MYUTDestroyState()
{
	SAFE_DELETE(g_pMYUTState);
}

class MYUTMemoryHelper
{
public:
	MYUTMemoryHelper() { MYUTCreateState(); }
	~MYUTMemoryHelper() { MYUTDestroyState(); }
};

MYUTState& GetMYUTState()
{
	// This class will auto create the memory when its first accessed and delete it after the program exits WinMain.
	// However the application can also call DXUTCreateState() & DXUTDestroyState() independantly if its wants 
	static MYUTMemoryHelper memory;
	MY_ASSERT(g_pMYUTState != NULL);
	return *g_pMYUTState;
}


HWND WINAPI MYUTGetHWND()
{
	return GetMYUTState().GetHWNDDeviceWindowed();
}


void MYUTRender3DEnvironment()
{
	{
		LPMYUTCALLBACKDRAW pDraw = GetMYUTState().GetDrawFunc();
		if (pDraw)
		{
			pDraw();
		}
	}
}

HRESULT _stdcall MYUTMainLoop(HACCEL hAccel)
{
	HWND hWnd = MYUTGetHWND();
	// Now we're ready to receive and process Windows messages.
	bool bGotMsg;
	MSG msg;
	msg.message = WM_NULL;
	PeekMessage(&msg, nullptr, 0U, 0U, PM_NOREMOVE);

	while (WM_QUIT != msg.message)
	{
		// Use PeekMessage() so we can use idle time to render the scene. 
		bGotMsg = (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE) != 0);

		if (bGotMsg)
		{
			// Translate and dispatch the message
			if (hAccel == nullptr || hWnd == nullptr ||
				0 == TranslateAccelerator(hWnd, hAccel, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			// Render a frame during idle time (no messages are waiting)
			MYUTRender3DEnvironment();
		}
	}

	// Cleanup the accelerator table
	if (hAccel != nullptr)
		DestroyAcceleratorTable(hAccel);
	return S_OK;
}

HWND WINAPI MYUTGetHWNDFocus() { return GetMYUTState().GetHWNDDeviceWindowed(); }

void __stdcall MYUTSetCallBackDraw(LPMYUTCALLBACKDRAW pCallback) { GetMYUTState().SetDrawFunc(pCallback); }

void __stdcall MYUTSetCallBackResize(LPMYUTCALLBACKRESIZE pCallback) { GetMYUTState().SetResizeFunc(pCallback); }

bool* __stdcall MYUTGetKeys() { return GetMYUTState().GetKeys(); }

void CheckForWindowSizeChange()
{
	RECT rcCurrentClient;
	GetClientRect(MYUTGetHWND(), &rcCurrentClient);
	LPMYUTCALLBACKRESIZE pFunc = GetMYUTState().GetResizeFunc();
	if (pFunc)
	{
		int width = rcCurrentClient.right - rcCurrentClient.left;
		int height = rcCurrentClient.bottom - rcCurrentClient.top;
		pFunc(width, height);
	}
}

LRESULT CALLBACK MyStaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Pass all messages to the app's MsgProc callback, and don't 
	// process further messages if the apps says not to.
	LPMYUTCALLBACKMSGPROC pCallbackMsgProc = GetMYUTState().GetWindowMsgFunc();
	if (pCallbackMsgProc)
	{
		bool bNoFurtherProcessing = false;
		LRESULT nResult = pCallbackMsgProc(hWnd, uMsg, wParam, lParam, &bNoFurtherProcessing,
		                                   GetMYUTState().GetWindowMsgFuncUserContext());
		if (bNoFurtherProcessing)
			return nResult;
	}

	// Consolidate the keyboard messages and pass them to the app's keyboard callback
	if (uMsg == WM_KEYDOWN ||
		uMsg == WM_SYSKEYDOWN ||
		uMsg == WM_KEYUP ||
		uMsg == WM_SYSKEYUP)
	{
		bool bKeyDown = (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN);
		DWORD dwMask = (1 << 29);
//		bool bAltDown = ((lParam & dwMask) != 0);
		auto keys = GetMYUTState().GetKeys();
		keys[(BYTE)(wParam & 0xFF)] = bKeyDown;
	}

	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(WM_QUIT);
		break;
	case WM_ENTERSIZEMOVE:
		// Halt frame movement while the app is sizing or moving
		GetMYUTState().SetInSizeMove(true);
		break;
	case WM_EXITSIZEMOVE:
		GetMYUTState().SetInSizeMove(false);
		CheckForWindowSizeChange();
		break;
	case WM_PAINT:
		break;
	case WM_SIZE:
		if (SIZE_MINIMIZED == wParam)
		{
			//pause()
		}
		else
		{
			RECT rcCurrentClient;
			GetClientRect(MYUTGetHWND(), &rcCurrentClient);
			if (rcCurrentClient.top == 0 && rcCurrentClient.bottom == 0)
			{
				// Rapidly clicking the task bar to minimize and restore a window
				// can cause a WM_SIZE message with SIZE_RESTORED when 
				// the window has actually become minimized due to rapid change
				// so just ignore this message
			}
			else if (SIZE_MAXIMIZED == wParam)
			{
				//if (GetDXUTState().GetMinimized())
				//	DXUTPause(false, false); // Unpause since we're no longer minimized
				//GetDXUTState().SetMinimized(false);
				//GetDXUTState().SetMaximized(true);
				//DXUTCheckForWindowSizeChange();
				//DXUTCheckForWindowChangingMonitors();
			}
			else if (SIZE_RESTORED == wParam)
			{
				////DXUTCheckForDXGIFullScreenSwitch();
				//if (GetDXUTState().GetMaximized())
				//{
				//	GetDXUTState().SetMaximized(false);
				//	DXUTCheckForWindowSizeChange();
				//	DXUTCheckForWindowChangingMonitors();
				//}
				//else if (GetDXUTState().GetMinimized())
				//{
				//	DXUTPause(false, false); // Unpause since we're no longer minimized
				//	GetDXUTState().SetMinimized(false);
				//	DXUTCheckForWindowSizeChange();
				//	DXUTCheckForWindowChangingMonitors();
				//}
				//else 
				if (GetMYUTState().GetInSizeMove())
				{
					// If we're neither maximized nor minimized, the window size 
					// is changing by the user dragging the window edges.  In this 
					// case, we don't reset the device yet -- we wait until the 
					// user stops dragging, and a WM_EXITSIZEMOVE message comes.
				}
				else
				{
					// This WM_SIZE come from resizing the window via an API like SetWindowPos() so 
					// resize and reset the device now.
					//DXUTCheckForWindowSizeChange();
					//DXUTCheckForWindowChangingMonitors();
				}
			}
		}

		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


HRESULT WINAPI MYUTCreateWindow(const WCHAR* strWindowTitle, HINSTANCE hInstance,
                                HICON hIcon, HMENU hMenu, int x, int y)
{
	if (hInstance == nullptr)
		hInstance = (HINSTANCE)GetModuleHandle(nullptr);
	if (MYUTGetHWNDFocus() == nullptr)
	{
		WCHAR szExePath[MAX_PATH];
		GetModuleFileName(nullptr, szExePath, MAX_PATH);
		if (hIcon == nullptr) // If the icon is NULL, then use the first one found in the exe
			hIcon = ExtractIcon(hInstance, szExePath, 0);

		// Register the windows class
		WNDCLASSEX WndCls;

		WndCls.cbSize = sizeof(WndCls);
		WndCls.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
		WndCls.lpfnWndProc = MyStaticWndProc;
		WndCls.cbClsExtra = 0;
		WndCls.cbWndExtra = 0;
		WndCls.hInstance = hInstance;
		WndCls.hIcon = hIcon;
		WndCls.hCursor = LoadCursor(nullptr, IDC_ARROW);
		WndCls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		WndCls.lpszMenuName = nullptr;
		WndCls.lpszClassName = TEXT("MAIN");
		WndCls.hIconSm = hIcon;

		MY_ASSERT(RegisterClassEx(&WndCls) != 0);


		// Find the window's initial size, but it might be changed later
		int nDefaultWidth = 640;
		int nDefaultHeight = 480;

		RECT rc;
		SetRect(&rc, 0, 0, nDefaultWidth, nDefaultHeight);
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, (hMenu != nullptr) ? true : false);

		// Create the render window
		HWND hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
		                           TEXT("MAIN"), strWindowTitle,
		                           WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		                           CW_USEDEFAULT, CW_USEDEFAULT, nDefaultWidth, nDefaultHeight,
		                           nullptr, nullptr, hInstance, nullptr);
		MY_ASSERT(hWnd != NULL);
		UpdateWindow(hWnd);
		GetMYUTState().SetHWNDDeviceWindowed(hWnd);
	}

	return S_OK;
}
