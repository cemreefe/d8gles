#pragma once
// Hooks the platform layer installs before creating a device. d8gles never creates
// a GL context or a window: the platform makes a GLES 3.0 context current on the
// render thread, then d8gles issues GL calls on it and asks the platform to present.

#ifdef __cplusplus
extern "C" {
#endif

enum { D8GLES_LOG_INFO = 0, D8GLES_LOG_ERROR = 1 };

typedef struct D8GLES_Platform
{
    /* Called from IDirect3DDevice8::Present (e.g. eglSwapBuffers). May be null. */
    void (*present)(void* user);
    /* Diagnostics. Null logs errors to stderr. */
    void (*log)(int level, const char* message, void* user);
    void* user;
} D8GLES_Platform;

void D8GLES_SetPlatform(const D8GLES_Platform* platform);

/* Debug knobs: solidMode 0 = normal rendering, 1..8 = flat-shading diagnostics;
   dumpThisFrame logs the draw state of the next frame. */
void D8GLES_SetDebug(int solidMode, int dumpThisFrame);
/* Draw calls issued since start-up. */
unsigned D8GLES_GetDrawCount(void);

#ifdef __cplusplus
}
#endif
