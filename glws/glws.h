#ifndef _GLWS_H
#define _GLWS_H

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif


#define GLWS_SRGB					0x1
#define GLWS_ALPHA					0x2
#define GLWS_MULTISAMPLE			0x4

#define GLWS_CORE_PROFILE			0x00000001
#define GLWS_COMPATIBILITY_PROFILE	0x00000002

#define WGL_CONTEXT_DEBUG_BIT_ARB				0x00000001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB	0x00000002

#define GLWS_LBUTTON	0
#define GLWS_MBUTTON	1
#define GLWS_RBUTTON	2
#define GLWS_XBUTTON1	3
#define GLWS_XBUTTON2	4

typedef void (*WndRszFptr)(int, int);
typedef void (*MouseMvFptr)(int, int);
typedef void (*KeyFptr)(unsigned char, bool);
typedef void (*MouseBtnFptr)(unsigned char, bool);
typedef void (*MouseWheelFptr)(float);

bool glwsInit(void);
bool glwsCreateWindow(const char* const title, int width, int height);
void glwsClean(void); 				// Destructor for cleaning up our application  
void glwsSwapBuffers(void); 			// Render scene (display method from previous OpenGL tutorials)
void glwsPollEvents(void);
bool glwsIsWindowOpen(void);
void glwsToggleFullscreen(void);
void glwsSetDisplayMode(int);
void glwsSetProfile(int);
void glwsSetContextVersion(int major, int minor);

void glwsSetWindowSizeCB(WndRszFptr);
void glwsSetKeyCB(KeyFptr);
void glwsSetMouseMoveCB(MouseMvFptr);
void glwsSetMouseButtonCB(MouseBtnFptr);
void glwsSetMouseWheelCB(MouseWheelFptr);
void glwsToggleKeyRepeat(void);

bool glwsGetKey(unsigned char key);
bool glwsGetMouseButton(unsigned char button);
void glwsSetMousePos(int, int);
double glwsGetElapsedTime(void);


#ifdef __cplusplus
}
#endif

#endif