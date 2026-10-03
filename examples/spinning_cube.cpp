// Spinning textured cube over a lit, fogged floor, drawn only through the D3D8 API.
// Renders headlessly into an EGL pbuffer (Mesa: EGL_PLATFORM=surfaceless).
//   spinning_cube                 render one frame and check it (CTest smoke test)
//   spinning_cube <dir> [frames]  also write <dir>/frame_NNN.ppm
#include <d8gles/d8gles.h>
#include <EGL/egl.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

static const int W = 640, H = 480;

static DWORD F2DW(float f) { DWORD d; memcpy(&d, &f, 4); return d; }

struct CubeVertex { float x, y, z; DWORD color; float u, v; };
static const DWORD CUBE_FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
struct FloorVertex { float x, y, z, nx, ny, nz, u, v; };
static const DWORD FLOOR_FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

// 16x16 pixel-art "8" on a framed tile, nearest-filtered for a chunky look.
static IDirect3DTexture8* MakeFaceTexture(IDirect3DDevice8* dev)
{
    static const char* glyph[10] = {
        " ###### ", "##    ##", "##    ##", "##    ##", " ###### ",
        "##    ##", "##    ##", "##    ##", "##    ##", " ###### " };
    IDirect3DTexture8* tex = 0;
    dev->CreateTexture(16, 16, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &tex);
    D3DLOCKED_RECT lr;
    tex->LockRect(0, &lr, 0, 0);
    for (int y = 0; y < 16; ++y) {
        DWORD* row = (DWORD*)((BYTE*)lr.pBits + y * lr.Pitch);
        for (int x = 0; x < 16; ++x) {
            bool frame = x == 0 || y == 0 || x == 15 || y == 15;
            int gx = x - 4, gy = y - 3;
            bool ink = gx >= 0 && gx < 8 && gy >= 0 && gy < 10 && glyph[gy][gx] == '#';
            row[x] = frame ? 0xff20202c : ink ? 0xffffffff : ((x + y) & 1 ? 0xff9a9aa8 : 0xff8a8a98);
        }
    }
    tex->UnlockRect(0);
    return tex;
}

static IDirect3DTexture8* MakeFloorTexture(IDirect3DDevice8* dev)
{
    IDirect3DTexture8* tex = 0;
    dev->CreateTexture(64, 64, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &tex);
    D3DLOCKED_RECT lr;
    tex->LockRect(0, &lr, 0, 0);
    for (int y = 0; y < 64; ++y) {
        DWORD* row = (DWORD*)((BYTE*)lr.pBits + y * lr.Pitch);
        for (int x = 0; x < 64; ++x)
            row[x] = (x < 2 || y < 2) ? 0xff38e0c8 : 0xff1a2330;
    }
    tex->UnlockRect(0);
    return tex;
}

static IDirect3DVertexBuffer8* MakeCube(IDirect3DDevice8* dev)
{
    static const float n[6][3] = { {0,0,-1}, {0,0,1}, {-1,0,0}, {1,0,0}, {0,1,0}, {0,-1,0} };
    static const DWORD faceColor[6] = { 0xffff5a6e, 0xff5ac8ff, 0xffffc84a, 0xff7cf08a, 0xffc890ff, 0xffff9a50 };
    IDirect3DVertexBuffer8* vb = 0;
    dev->CreateVertexBuffer(36 * sizeof(CubeVertex), 0, CUBE_FVF, D3DPOOL_MANAGED, &vb);
    CubeVertex* v;
    vb->Lock(0, 0, (BYTE**)&v, 0);
    for (int f = 0; f < 6; ++f) {
        // Two tangents per face give the four corners; (s,t) in {-1,1}.
        float ax[3] = { n[f][1] != 0 ? 1.f : 0.f, 0, 0 }, ay[3] = { 0, n[f][1] != 0 ? 0.f : 1.f, n[f][1] != 0 ? 1.f : 0.f };
        if (n[f][1] == 0) { ax[0] = n[f][2]; ax[2] = -n[f][0]; }
        static const float st[6][2] = { {-1,-1}, {-1,1}, {1,1}, {-1,-1}, {1,1}, {1,-1} };
        for (int i = 0; i < 6; ++i) {
            float s = st[i][0], t = st[i][1];
            CubeVertex& o = *v++;
            o.x = n[f][0] + s * ax[0] + t * ay[0];
            o.y = n[f][1] + s * ax[1] + t * ay[1];
            o.z = n[f][2] + s * ax[2] + t * ay[2];
            o.color = faceColor[f];
            o.u = (s + 1) * 0.5f; o.v = (1 - t) * 0.5f;
        }
    }
    vb->Unlock();
    return vb;
}

static IDirect3DVertexBuffer8* MakeFloor(IDirect3DDevice8* dev)
{
    const float e = 40.f, y = -1.6f, rep = 20.f;
    FloorVertex q[6] = { {-e,y,-e,0,1,0,0,0}, {-e,y,e,0,1,0,0,rep}, {e,y,e,0,1,0,rep,rep},
                         {-e,y,-e,0,1,0,0,0}, {e,y,e,0,1,0,rep,rep}, {e,y,-e,0,1,0,rep,0} };
    IDirect3DVertexBuffer8* vb = 0;
    dev->CreateVertexBuffer(sizeof(q), 0, FLOOR_FVF, D3DPOOL_MANAGED, &vb);
    BYTE* p;
    vb->Lock(0, 0, &p, 0);
    memcpy(p, q, sizeof(q));
    vb->Unlock();
    return vb;
}

static void WritePPM(const char* path, const std::vector<unsigned char>& rgba)
{
    FILE* f = fopen(path, "wb");
    if (!f) { perror(path); exit(1); }
    fprintf(f, "P6\n%d %d\n255\n", W, H);
    for (int y = H - 1; y >= 0; --y)
        for (int x = 0; x < W; ++x)
            fwrite(&rgba[(y * W + x) * 4], 1, 3, f);
    fclose(f);
}

int main(int argc, char** argv)
{
    const char* outDir = argc > 1 ? argv[1] : 0;
    int frames = argc > 2 ? atoi(argv[2]) : (outDir ? 60 : 1);

    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (dpy == EGL_NO_DISPLAY || !eglInitialize(dpy, 0, 0)) { printf("SKIP: no EGL display\n"); return 77; }
    const EGLint cfgAttr[] = { EGL_SURFACE_TYPE, EGL_PBUFFER_BIT, EGL_RENDERABLE_TYPE, 0x40 /* EGL_OPENGL_ES3_BIT */,
                               EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_DEPTH_SIZE, 24, EGL_NONE };
    EGLConfig cfg; EGLint n = 0;
    if (!eglChooseConfig(dpy, cfgAttr, &cfg, 1, &n) || n < 1) { printf("SKIP: no GLES3 pbuffer config\n"); return 77; }
    const EGLint pbAttr[] = { EGL_WIDTH, W, EGL_HEIGHT, H, EGL_NONE };
    EGLSurface surf = eglCreatePbufferSurface(dpy, cfg, pbAttr);
    const EGLint ctxAttr[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
    eglBindAPI(EGL_OPENGL_ES_API);
    EGLContext ctx = eglCreateContext(dpy, cfg, EGL_NO_CONTEXT, ctxAttr);
    if (surf == EGL_NO_SURFACE || ctx == EGL_NO_CONTEXT || !eglMakeCurrent(dpy, surf, surf, ctx)) { printf("SKIP: no GLES3 context\n"); return 77; }

    D8GLES_Platform platform = { 0, 0, 0 };
    D8GLES_SetPlatform(&platform);
    IDirect3D8* d3d = Direct3DCreate8(D3D_SDK_VERSION);
    IDirect3DDevice8* dev = 0;
    if (d3d->CreateDevice(0, D3DDEVTYPE_HAL, 0, 0, 0, &dev) != S_OK || !dev) { fprintf(stderr, "CreateDevice failed\n"); return 1; }

    IDirect3DTexture8* faceTex = MakeFaceTexture(dev);
    IDirect3DTexture8* floorTex = MakeFloorTexture(dev);
    IDirect3DVertexBuffer8* cube = MakeCube(dev);
    IDirect3DVertexBuffer8* floor = MakeFloor(dev);

    D3DVIEWPORT8 vp = { 0, 0, W, H, 0.f, 1.f };
    dev->SetViewport(&vp);
    D3DXMATRIX view, proj;
    D3DXVECTOR3 eye(0.f, 1.2f, 5.5f), at(0.f, -0.2f, 0.f), up(0.f, 1.f, 0.f);
    D3DXMatrixLookAtRH(&view, &eye, &at, &up);
    D3DXMatrixPerspectiveFovRH(&proj, 0.9f, (float)W / H, 0.1f, 100.f);
    dev->SetTransform(D3DTS_VIEW, &view);
    dev->SetTransform(D3DTS_PROJECTION, &proj);

    dev->SetRenderState(D3DRS_ZENABLE, TRUE);
    dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    dev->SetRenderState(D3DRS_FOGENABLE, TRUE);
    dev->SetRenderState(D3DRS_FOGCOLOR, 0xff0b0f1a);
    dev->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
    dev->SetRenderState(D3DRS_FOGSTART, F2DW(4.f));
    dev->SetRenderState(D3DRS_FOGEND, F2DW(26.f));
    dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

    D3DLIGHT8 light = {};
    light.Type = D3DLIGHT_DIRECTIONAL;
    light.Diffuse = D3DXCOLOR(1.f, 0.95f, 0.85f, 1.f);
    light.Direction = D3DXVECTOR3(-0.4f, -1.f, -0.6f);
    dev->SetLight(0, &light);
    dev->LightEnable(0, TRUE);
    dev->SetRenderState(D3DRS_AMBIENT, 0xff303848);
    D3DMATERIAL8 mat = {};
    mat.Diffuse = mat.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
    dev->SetMaterial(&mat);

    std::vector<unsigned char> px((size_t)W * H * 4);
    int fail = 0;
    for (int i = 0; i < frames; ++i) {
        float t = (float)i / frames * 6.2831853f;
        dev->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(11, 15, 26), 1.f, 0);
        dev->BeginScene();

        D3DXMATRIX world;
        D3DXMatrixTranslation(&world, 0.f, 0.f, 4.f * i / frames);   // scroll one floor tile per loop
        dev->SetTransform(D3DTS_WORLD, &world);
        dev->SetRenderState(D3DRS_LIGHTING, TRUE);
        dev->SetTexture(0, floorTex);
        dev->SetStreamSource(0, floor, sizeof(FloorVertex));
        dev->SetVertexShader(FLOOR_FVF);
        dev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

        D3DXMATRIX rx, ry, lift;
        D3DXMatrixRotationX(&rx, 0.45f + 0.25f * sinf(t));
        D3DXMatrixRotationY(&ry, t);
        D3DXMatrixTranslation(&lift, 0.f, 0.15f * sinf(2.f * t), 0.f);
        D3DXMatrixMultiply(&world, &ry, &rx);
        D3DXMatrixMultiply(&world, &world, &lift);
        dev->SetTransform(D3DTS_WORLD, &world);
        dev->SetRenderState(D3DRS_LIGHTING, FALSE);
        dev->SetTexture(0, faceTex);
        dev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
        dev->SetStreamSource(0, cube, sizeof(CubeVertex));
        dev->SetVertexShader(CUBE_FVF);
        dev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 12);
        dev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);

        dev->EndScene();
        glReadPixels(0, 0, W, H, GL_RGBA, GL_UNSIGNED_BYTE, px.data());
        dev->Present(0, 0, 0, 0);
        if (outDir) {
            char path[1024];
            snprintf(path, sizeof(path), "%s/frame_%03d.ppm", outDir, i);
            WritePPM(path, px);
        }
        if (i == 0) {
            // The cube covers the centre; the floor's fog ends near the horizon.
            const unsigned char* c = &px[((H / 2) * W + W / 2) * 4];
            const unsigned char* b = &px[((H / 6) * W + 8) * 4];
            printf("centre %u,%u,%u floor %u,%u,%u\n", c[0], c[1], c[2], b[0], b[1], b[2]);
            if (c[0] + c[1] + c[2] < 120) { fprintf(stderr, "cube not drawn\n"); ++fail; }
        }
    }

    cube->Release(); floor->Release(); faceTex->Release(); floorTex->Release();
    eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglTerminate(dpy);
    if (fail) return 1;
    printf("ok, %d frame(s)\n", frames);
    return 0;
}
