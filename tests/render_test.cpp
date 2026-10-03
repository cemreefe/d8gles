// Headless check: clears and draws a pre-transformed (XYZRHW | DIFFUSE) triangle
// through the D3D8 API on an EGL pbuffer, then reads pixels back.
#include <d8gles/d8gles.h>
#include <EGL/egl.h>
#include <cstdio>
#include <cstdlib>

static int g_fail = 0;
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%s:%d: CHECK(%s) failed\n", __FILE__, __LINE__, #c); ++g_fail; } } while (0)

static int g_presents = 0;
static void Present(void*) { ++g_presents; }

int main()
{
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (dpy == EGL_NO_DISPLAY || !eglInitialize(dpy, 0, 0)) { printf("SKIP: no EGL display\n"); return 77; }
    const EGLint cfgAttr[] = { EGL_SURFACE_TYPE, EGL_PBUFFER_BIT, EGL_RENDERABLE_TYPE, 0x40 /* EGL_OPENGL_ES3_BIT */,
                               EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_DEPTH_SIZE, 16, EGL_NONE };
    EGLConfig cfg; EGLint n = 0;
    if (!eglChooseConfig(dpy, cfgAttr, &cfg, 1, &n) || n < 1) { printf("SKIP: no GLES3 pbuffer config\n"); return 77; }
    const EGLint pbAttr[] = { EGL_WIDTH, 64, EGL_HEIGHT, 64, EGL_NONE };
    EGLSurface surf = eglCreatePbufferSurface(dpy, cfg, pbAttr);
    const EGLint ctxAttr[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
    eglBindAPI(EGL_OPENGL_ES_API);
    EGLContext ctx = eglCreateContext(dpy, cfg, EGL_NO_CONTEXT, ctxAttr);
    if (surf == EGL_NO_SURFACE || ctx == EGL_NO_CONTEXT || !eglMakeCurrent(dpy, surf, surf, ctx)) { printf("SKIP: no GLES3 context\n"); return 77; }

    D8GLES_Platform platform = { Present, 0, 0 };
    D8GLES_SetPlatform(&platform);

    IDirect3D8* d3d = Direct3DCreate8(220);
    IDirect3DDevice8* dev = 0;
    CHECK(d3d->CreateDevice(0, D3DDEVTYPE_HAL, 0, 0, 0, &dev) == S_OK && dev);
    D3DVIEWPORT8 vp = { 0, 0, 64, 64, 0.f, 1.f };
    dev->SetViewport(&vp);
    dev->SetRenderState(D3DRS_ZENABLE, FALSE);
    dev->SetRenderState(D3DRS_LIGHTING, FALSE);
    dev->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.f, 0);

    struct V { float x, y, z, rhw; DWORD color; };
    // D3DCOLOR in vertex data is 0xAARRGGBB: opaque red.
    V tri[3] = { { 0, 0, 0.5f, 1, 0xffff0000 }, { 64, 0, 0.5f, 1, 0xffff0000 }, { 0, 64, 0.5f, 1, 0xffff0000 } };
    dev->SetTexture(0, 0);
    dev->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, tri, sizeof(V));

    unsigned char inside[4] = {}, outside[4] = {};
    glReadPixels(8, 64 - 1 - 8, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, inside);   // top-left in D3D space
    glReadPixels(56, 64 - 1 - 56, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, outside); // bottom-right
    printf("inside %u,%u,%u outside %u,%u,%u\n", inside[0], inside[1], inside[2], outside[0], outside[1], outside[2]);
    CHECK(outside[2] > 200 && outside[0] < 50);
    CHECK((inside[0] > 200 && inside[2] < 50) || (inside[0] < 50 && inside[2] > 200));
    bool redSomewhere = (inside[0] > 200 && inside[2] < 50);
    if (!redSomewhere) {
        unsigned char flipped[4] = {};
        glReadPixels(8, 8, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, flipped);
        CHECK(flipped[0] > 200 && flipped[2] < 50);
    }
    CHECK(D8GLES_GetDrawCount() == 1);
    dev->Present(0, 0, 0, 0);
    CHECK(g_presents == 1);

    eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglTerminate(dpy);
    if (g_fail) { printf("%d failures\n", g_fail); return 1; }
    printf("ok\n");
    return 0;
}
