#define UNICODE
#define _WIN32_WINNT 0x0501 //Minimum Windows XP
#define WIN32_LEAN_AND_MEAN

#include "glws.h"
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>

#define GLWS_WTITLE_CHAR_MAX 256

#define WGL_DRAW_TO_WINDOW_ARB				0x2001
#define WGL_ACCELERATION_ARB				0x2003
#define WGL_SUPPORT_OPENGL_ARB				0x2010
#define WGL_DOUBLE_BUFFER_ARB				0x2011
#define WGL_PIXEL_TYPE_ARB					0x2013
#define WGL_COLOR_BITS_ARB					0x2014
#define WGL_ALPHA_BITS_ARB					0x201B
#define WGL_DEPTH_BITS_ARB					0x2022
#define WGL_STENCIL_BITS_ARB				0x2023
#define WGL_FULL_ACCELERATION_ARB			0x2027
#define WGL_TYPE_RGBA_ARB					0x202B
#define WGL_SAMPLE_BUFFERS_ARB				0x2041
#define WGL_SAMPLES_ARB						0x2042
#define WGL_CONTEXT_MAJOR_VERSION_ARB		0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB		0x2092
#define WGL_CONTEXT_FLAGS_ARB				0x2094
#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB	0x20A9
#define WGL_CONTEXT_PROFILE_MASK_ARB		0x9126

typedef BOOL (WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList);
typedef const char * (WINAPI * PFNWGLGETEXTENSIONSSTRINGARBPROC) (HDC hdc);

typedef struct{
	/*============================================================================================*/
	/*====================================* Window Properties *===================================*/
	/*============================================================================================*/
	int	width;
	int height;
	bool is_open;
	bool is_fullscreen;
	int displayMode;
	int profileMode;
	DWORD style;
	DWORD styleEx;
	int context_ver[2];
	bool keys[256];
	bool is_key_repeat;
	bool mouse_btn[8];
	int mouseX, mouseY;
	/*============================================================================================*/
	/*=====================================* Window Handles *=====================================*/
	/*============================================================================================*/
	HGLRC hrc;	//Rendering Context
	HDC hdc;	//Device Context
	HWND hwnd;	//Window Identifier
	/*============================================================================================*/
	/*====================================* Callback Functions *==================================*/
	/*============================================================================================*/
	WndRszFptr 		winResizeCB;
	KeyFptr 		keyCB;
	MouseMvFptr		mMoveCB;
	MouseBtnFptr	mButtonCB;
	MouseWheelFptr	mWheelCB;
	/*============================================================================================*/
	/*=====================================* Other Variables *====================================*/
	/*============================================================================================*/
	struct{
		__int64 t0;
		__int64 freq;
	}timer;
	/*============================================================================================*/
	/*============================================================================================*/
}_GlwsWindow;

_GlwsWindow glws_window;


static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static bool _glwsIsExtensionSupported(HDC hdc, const char* const extension);
static void _glwsDestroyWindow(void);

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////* C++ API Implementation *///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

bool glwsInit(void){
	glws_window.width			= 600;
	glws_window.height			= 600;
	glws_window.is_open			= false;
	glws_window.is_fullscreen	= false;
	glws_window.displayMode		= GLWS_ALPHA | GLWS_MULTISAMPLE;
	glws_window.profileMode		= GLWS_CORE_PROFILE;
	glws_window.style			= WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW;
	glws_window.styleEx			= WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	glws_window.context_ver[0]	= 3;
	glws_window.context_ver[1]	= 3;
	glws_window.is_key_repeat = false;
	
	for(int i = 0; i < 256; i++) glws_window.keys[i] = false;
	for(int i = 0; i < 8; i++) glws_window.mouse_btn[i] = false;
	
	glws_window.hrc		= NULL;
	glws_window.hdc		= NULL;
	glws_window.hwnd	= NULL;
	
	glws_window.winResizeCB = NULL;
	glws_window.keyCB		= NULL;
	glws_window.mMoveCB		= NULL;
	glws_window.mButtonCB	= NULL;
	glws_window.mWheelCB	= NULL;
	return true;
}

void glwsToggleFullscreen(void){
	static RECT prev_rect; //Save the window position when going fullscreen to restore it
	if(!glws_window.is_fullscreen){
		RECT wRect;
		GetWindowRect(glws_window.hwnd, &wRect);
		prev_rect = wRect;
		MONITORINFO monInfo;
		monInfo.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(MonitorFromWindow(glws_window.hwnd, MONITOR_DEFAULTTONEAREST), &monInfo);
		LONG_PTR lStyle = (glws_window.style & ~WS_OVERLAPPEDWINDOW) | WS_VISIBLE; //We also need to set the style to visible or else we get a black screen
		SetWindowLongPtr(glws_window.hwnd, GWL_STYLE, lStyle);
		int width = monInfo.rcMonitor.right - monInfo.rcMonitor.left;
		int height = monInfo.rcMonitor.bottom - monInfo.rcMonitor.top;
		LONG_PTR lExStyle = glws_window.styleEx & ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
		SetWindowLongPtr(glws_window.hwnd, GWL_EXSTYLE, lExStyle);
		SetWindowPos(glws_window.hwnd, HWND_TOP, monInfo.rcMonitor.left, monInfo.rcMonitor.top, width, height, SWP_FRAMECHANGED);
	}
	else{ //Exit fullscreen and restore previous window position and resolution
		int width = prev_rect.right - prev_rect.left;
		int height = prev_rect.bottom - prev_rect.top;
		LONG_PTR lStyle = glws_window.style | WS_VISIBLE; //We also need to set the style to visible or else we get a black screen
		SetWindowLongPtr(glws_window.hwnd, GWL_STYLE, lStyle);
		SetWindowLongPtr(glws_window.hwnd, GWL_EXSTYLE, glws_window.styleEx);
		SetWindowPos(glws_window.hwnd, HWND_TOP, prev_rect.left, prev_rect.top, width, height, SWP_FRAMECHANGED);
	}
	glws_window.is_fullscreen = !glws_window.is_fullscreen;
}

void glwsClean(void){
	_glwsDestroyWindow();
	UnregisterClass(TEXT("mWindowClass"), NULL);
}

void glwsSwapBuffers(void){
	SwapBuffers(glws_window.hdc);
	glwsPollEvents();
}

void glwsSetDisplayMode(int flags){
	if(glws_window.is_open) return;
	glws_window.displayMode = flags;
}

void glwsSetProfile(int profile){
	if(glws_window.is_open) return;
	glws_window.profileMode = profile;
}

void glwsSetContextVersion(int major, int minor){
	if(glws_window.is_open) return;
	glws_window.context_ver[0] = major;
	glws_window.context_ver[1] = minor;
}

bool glwsIsWindowOpen(void){
	return glws_window.is_open;
}

void glwsPollEvents(void){
	MSG msg = {};
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){ // Check for Messages
		if(msg.message == WM_QUIT) glws_window.is_open = false; // Quit message, set running to false
		else if(msg.message == WM_CREATE) MessageBox(NULL, L"The window is being created", L"Yo", MB_OK);
		else{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void glwsSetWindowSizeCB(WndRszFptr fptr){
	glws_window.winResizeCB = fptr;
}

void glwsSetKeyCB(KeyFptr fptr){
	glws_window.keyCB = fptr;
}

void glwsSetMouseMoveCB(MouseMvFptr fptr){
	glws_window.mMoveCB = fptr;
}

void glwsSetMouseButtonCB(MouseBtnFptr fptr){
	glws_window.mButtonCB = fptr;
}

void glwsSetMouseWheelCB(MouseWheelFptr fptr){
	glws_window.mWheelCB = fptr;
}

void glwsToggleKeyRepeat(void){
	glws_window.is_key_repeat = !glws_window.is_key_repeat;
}

bool glwsGetKey(unsigned char key){
	return glws_window.keys[key];
}

bool glwsGetMouseButton(unsigned char button){
	return glws_window.mouse_btn[button];
}

void glwsSetMousePos(int x, int y){
	glws_window.mouseX = x;
	glws_window.mouseY = y;
	POINT new_pos;
	new_pos.x = x;
	new_pos.y = y;
	ClientToScreen(glws_window.hwnd, &new_pos);
	SetCursorPos(new_pos.x, new_pos.y);
}

double glwsGetElapsedTime(void){
	__int64 t;
	QueryPerformanceCounter((LARGE_INTEGER*)&t);
	return (double)(t - glws_window.timer.t0) / glws_window.timer.freq;
}

bool glwsCreateWindow(const char* const title, int width, int height){
	WNDCLASS windowClass = {0};
	DWORD dwExStyle = glws_window.styleEx; //Extended Windows Styles
	DWORD dwStyle 	= glws_window.style; //Window Style
	HINSTANCE hInstance = GetModuleHandle(NULL);
	
	windowClass.style 			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc	 	= (WNDPROC) DefWindowProc;
	windowClass.hInstance		= hInstance;
	windowClass.hIcon 			= LoadIcon(NULL, IDI_WINLOGO);
	windowClass.hCursor			= LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName 	= TEXT("mWindowClass");
	
	if(!RegisterClass(&windowClass)) return false; // Failure!
	
	int wchar_len = MultiByteToWideChar(CP_UTF8, 0, title, -1, NULL, 0);
	if(wchar_len > GLWS_WTITLE_CHAR_MAX) wchar_len = GLWS_WTITLE_CHAR_MAX; //Allow up GLWS_WTITLE_CHAR_MAX characters
	WCHAR windowTitle[wchar_len]; //Variable length array on stack. C99 only.
	if(wchar_len > 0) MultiByteToWideChar(CP_UTF8, 0, title, (wchar_len + 1) * sizeof(WCHAR), windowTitle, wchar_len);
	windowTitle[wchar_len-1] = '\0'; //Make sure null terminated because of character limit
	
	glws_window.hwnd = CreateWindowEx(dwExStyle, windowClass.lpszClassName, windowTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hInstance, NULL);
	
	PIXELFORMATDESCRIPTOR pfd = {0}; //Create a new PIXELFORMATDESCRIPTOR
	
	pfd.nSize 			= sizeof(PIXELFORMATDESCRIPTOR); // Set the size of the PFD
	pfd.dwFlags 		= PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW; //Enabe double buffering and stuff
	pfd.iPixelType 		= PFD_TYPE_RGBA; // Set RGBA mode
	pfd.cColorBits 		= 32; // 32bit Color
	pfd.cDepthBits 		= 24; // 24 Bit Depth
	pfd.cStencilBits 	= 8; // 8 Bit Stencil
	pfd.iLayerType 		= PFD_MAIN_PLANE;
	
	glws_window.hdc = GetDC(glws_window.hwnd); // Get the device context
	int nPixelFormat = ChoosePixelFormat(glws_window.hdc, &pfd); //Get a pixel format based on pfd
	if(nPixelFormat == 0) return false; //Failure!
	
	bool bResult = SetPixelFormat(glws_window.hdc, nPixelFormat, &pfd); // Try to set the pixel format based on PFD
	if(!bResult) return false; // Failure!
	
	glws_window.hrc = wglCreateContext(glws_window.hdc); // Create an OpenGL 2.1 context for our device context
	wglMakeCurrent(glws_window.hdc, glws_window.hrc); // Make the OpenGL 2.1 context current and active
	
	//If OpenGL 3.x Context creation is supported and was asked for...
	if(_glwsIsExtensionSupported(glws_window.hdc, "WGL_ARB_create_context") && glws_window.context_ver[0] > 2){
		PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARBProc = (PFNWGLCHOOSEPIXELFORMATARBPROC) wglGetProcAddress("wglChoosePixelFormatARB");
		PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARBProc = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress( "wglCreateContextAttribsARB" );
		
		//Set the pixel format, equivalent to PIXELFORMATDESCRIPTOR
		int pixelAttribs[] = {
			WGL_DRAW_TO_WINDOW_ARB,				true,
			WGL_SUPPORT_OPENGL_ARB,				true,
			WGL_ACCELERATION_ARB,				WGL_FULL_ACCELERATION_ARB,
			WGL_DOUBLE_BUFFER_ARB,				true,
			WGL_SAMPLE_BUFFERS_ARB,				(bool)(glws_window.displayMode & GLWS_MULTISAMPLE),
			WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB,	(bool)(glws_window.displayMode & GLWS_SRGB),
			WGL_PIXEL_TYPE_ARB,					WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB,					32,
			WGL_DEPTH_BITS_ARB,					24,
			WGL_STENCIL_BITS_ARB,				8,
			WGL_ALPHA_BITS_ARB,					((glws_window.displayMode & GLWS_ALPHA)? 8 : 0),
			WGL_SAMPLES_ARB,					4,					//Multisampling 4x
			0
		};
		unsigned int nFormats;
		float fAttribs[] = {0.0f, 0.0f};
		
		bool bResult = wglChoosePixelFormatARBProc(glws_window.hdc, pixelAttribs, fAttribs, 1, &nPixelFormat, &nFormats);
		
		//If we succeeded in choosing a pixel format...
		if(bResult && nFormats > 0){
			//Create temperary window, device context and try to set the pixel format of the context
			HWND temp_hwnd = CreateWindowEx(dwExStyle, windowClass.lpszClassName, windowTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hInstance, NULL);
			HDC temp_hdc = GetDC(temp_hwnd); // Get the device context
			bResult = SetPixelFormat(temp_hdc, nPixelFormat, &pfd);
			if(bResult){
				int contextAttribs[] = {
					WGL_CONTEXT_MAJOR_VERSION_ARB, 	glws_window.context_ver[0],
					WGL_CONTEXT_MINOR_VERSION_ARB,	glws_window.context_ver[1],
					WGL_CONTEXT_PROFILE_MASK_ARB,	glws_window.profileMode,
					WGL_CONTEXT_FLAGS_ARB,			WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
					0
				};
				//Create a new context with the selected pixel format and then destroy the previous window and the device and rendering contexts
				HGLRC tempGLContext = wglCreateContextAttribsARBProc(temp_hdc, NULL, contextAttribs); // Create an OpenGL 3.x context based on the given attributes
				_glwsDestroyWindow();
				glws_window.hrc = tempGLContext;
				glws_window.hwnd = temp_hwnd;
				glws_window.hdc = temp_hdc;
				wglMakeCurrent(glws_window.hdc, glws_window.hrc); // Make our OpenGL 3.x context current
			}
			else{
				//If we did not succeed in setting the pixel format, release the device context and destroy the temporary window
				ReleaseDC(temp_hwnd, temp_hdc); // Release the device context from our window 
				DestroyWindow(temp_hwnd);
			}
		}
	}
	
	//Set the window process back to our process so that the program exits when the window is destroyed
	SetWindowLongPtr(glws_window.hwnd, GWL_WNDPROC, (LONG) WndProc);
	SendMessage(glws_window.hwnd, WM_CREATE, 0, 0); //Use this if you need a WM_CREATE. Not sure if this is correct
	
	ShowWindow(glws_window.hwnd, SW_SHOW);
	UpdateWindow(glws_window.hwnd);
	
	glws_window.is_open = true;
	
	return true;  
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){

	switch(message){
	case WM_SIZE:
		glws_window.width = LOWORD(lParam);
		glws_window.height = HIWORD(lParam);
		if(glws_window.winResizeCB) glws_window.winResizeCB(glws_window.width, glws_window.height);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		QueryPerformanceCounter((LARGE_INTEGER*)&glws_window.timer.t0);
		QueryPerformanceFrequency((LARGE_INTEGER*)&glws_window.timer.freq);
		break;
	
	case WM_MOUSEMOVE:
		glws_window.mouseX = GET_X_LPARAM(lParam);
		glws_window.mouseY = GET_Y_LPARAM(lParam);
		if(glws_window.mMoveCB) glws_window.mMoveCB(glws_window.mouseX, glws_window.mouseY);
		break;
	
	case WM_MOUSEWHEEL:
		if(glws_window.mWheelCB) glws_window.mWheelCB( (short)HIWORD(wParam) / (float)WHEEL_DELTA );
		break;
	
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONDOWN:
	{
		unsigned char mButton = 7; //7, just because we don't use it and we need to initialize it
		switch(message){
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			mButton = GLWS_LBUTTON;
			break;
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			mButton = GLWS_RBUTTON;
			break;
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
			mButton = GLWS_MBUTTON;
			break;
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
			if(HIWORD(wParam) == 1) mButton = GLWS_XBUTTON1;
			else mButton = GLWS_XBUTTON2;
			break;
		}
		glws_window.mouse_btn[mButton] = !glws_window.mouse_btn[mButton];
		if(glws_window.mButtonCB) glws_window.mButtonCB(mButton, glws_window.mouse_btn[mButton]);
		break;
	}
	
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		//Check if either key repeat is on or this is the first keystroke
		if(glws_window.is_key_repeat || !(lParam & (1 << 30))){
			glws_window.keys[wParam] = !glws_window.keys[wParam]; //Here we toggle the state so that key repeat works
			if(glws_window.keyCB) glws_window.keyCB(wParam, true);
		}
		if(wParam == VK_ESCAPE)	PostQuitMessage(0);
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if(wParam == VK_RETURN && glws_window.keys[VK_MENU]) glwsToggleFullscreen();
		glws_window.keys[wParam] = false;
		if(glws_window.keyCB) glws_window.keyCB(wParam, false);
		break;
	case WM_MENUCHAR:
		return MNC_CLOSE << 16; //Stops the stupid "Default Beep" sound. MSDN specifies MNC_CLOSE should be the high-order value
	}
	
	return DefWindowProc(hWnd, message, wParam, lParam);
}


static inline bool _glwsIsExtensionSupported(HDC hDC, const char const *extension){
	PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB =
	  (PFNWGLGETEXTENSIONSSTRINGARBPROC) wglGetProcAddress("wglGetExtensionsStringARB");
	if ( wglGetExtensionsStringARB == NULL ){
		return FALSE;
	}
	const char *pWglExtString = wglGetExtensionsStringARB( hDC );
    return ( pWglExtString != NULL ) && ( strstr(pWglExtString, extension) != NULL );
}

static void _glwsDestroyWindow(void){
	wglMakeCurrent(glws_window.hdc, NULL); // Remove the rendering context from our device context  
	if(glws_window.hrc){
		wglDeleteContext(glws_window.hrc); // Delete our rendering context
		glws_window.hrc = NULL;
	}
	ReleaseDC(glws_window.hwnd, glws_window.hdc); // Release the device context from our window 
	if(glws_window.hwnd){
		DestroyWindow(glws_window.hwnd);
		glws_window.hwnd = NULL;
	}
}
