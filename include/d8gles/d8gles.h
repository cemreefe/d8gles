#pragma once
// d8gles: the Direct3D 8 / D3DX 8 API surface implemented on OpenGL ES 3.0.
// Context creation and presentation are the platform's job: see d8gles/platform.h.
#include "platform.h"

// OpenGL Headers for Mobile (GLES2 default for high compatibility)
#ifndef D8GLES_DESKTOP_GL
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
#else
    #include <GL/gl.h>
    #include <GL/glext.h>
#endif

#include <math.h>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

#ifndef HRESULT
typedef long HRESULT;
#endif
#ifndef S_OK
#define S_OK ((HRESULT)0L)
#endif
#ifndef S_FALSE
#define S_FALSE ((HRESULT)1L)
#endif
#ifndef FAILED
#define FAILED(hr) (((HRESULT)(hr)) < 0)
#endif
#ifndef SUCCEEDED
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#endif

#include <stdint.h>

#ifndef CONST
#define CONST const
#endif

typedef uint32_t UINT;
typedef uint32_t DWORD;
typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef int32_t LONG;
typedef int BOOL;
typedef uint32_t ULONG;
typedef const void* LPCVOID;
typedef void* LPVOID;
typedef const char* LPCSTR;
typedef char* LPSTR;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef _WIN32
typedef void* HWND;
typedef void* HDC;
typedef void* HGLRC;
#endif

// Hosts with their own Win32 type shim define D8GLES_HAVE_WIN32_TYPES.
#if !defined(_WIN32) && !defined(D8GLES_HAVE_WIN32_TYPES)
typedef struct _GUID { uint32_t Data1; uint16_t Data2, Data3; uint8_t Data4[8]; } GUID;
typedef union _LARGE_INTEGER { struct { DWORD LowPart; LONG HighPart; } u; int64_t QuadPart; } LARGE_INTEGER;
#endif
#ifndef E_FAIL
#define E_FAIL ((HRESULT)-1)
#endif

#ifndef _RECT_DEFINED
#define _RECT_DEFINED
#ifndef RECT
typedef struct tagRECT {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} RECT, *PRECT, *LPRECT;
#endif
#endif

#ifndef _POINT_DEFINED
#define _POINT_DEFINED
#ifndef POINT
typedef struct tagPOINT {
    LONG x;
    LONG y;
} POINT, *PPOINT, *LPPOINT;
#endif
#endif

#ifndef _SIZE_DEFINED
#define _SIZE_DEFINED
typedef struct tagSIZE {
    LONG cx;
    LONG cy;
} SIZE, *PSIZE, *LPSIZE;
#endif

#ifndef _PALETTEENTRY_DEFINED
#define _PALETTEENTRY_DEFINED
typedef struct tagPALETTEENTRY {
    BYTE peRed;
    BYTE peGreen;
    BYTE peBlue;
    BYTE peFlags;
} PALETTEENTRY;
#endif

// D3D Enum Types Stubbed
typedef int D3DFORMAT;
typedef int D3DPRIMITIVETYPE;
typedef int D3DDEVTYPE;
typedef int D3DTEXTUREOP;
typedef int D3DRENDERSTATETYPE;
typedef int D3DTEXTURESTAGESTATETYPE;
typedef int D3DBLEND;
typedef int D3DCMPFUNC;
typedef int D3DCULL;
typedef int D3DLIGHTSTATETYPE;

typedef int D3DFILLMODE;
typedef int D3DMULTISAMPLE_TYPE;
typedef int D3DTRANSFORMSTATETYPE;
typedef int D3DBACKBUFFER_TYPE;

#define D3DX_DEFAULT ((UINT)-1)
#define D3DX_DEFAULT_NONPOW2 ((UINT)-2)

#define D3DFMT_UNKNOWN 0
#define D3DFMT_R8G8B8 1
#define D3DFMT_A8R8G8B8 21
#define D3DFMT_X8R8G8B8 22
#define D3DFMT_A4R4G4B4 4
#define D3DFMT_A1R5G5B5 5
#define D3DFMT_X1R5G5B5 6
#define D3DFMT_A16B16G16R16 7
#define D3DFMT_DXT1 8
#define D3DFMT_DXT3 9
#define D3DFMT_DXT5 10
#define D3DFMT_D16 80
#define D3DFMT_D15S1 12
#define D3DFMT_D24X8 13
#define D3DFMT_D24S8 14
#define D3DFMT_D24X4S4 15
#define D3DFMT_D32 16
#define D3DFMT_INDEX16 17
#define D3DFMT_R5G6B5 18

#define D3DPOOL_MANAGED 1
#define D3DPOOL_SYSTEMMEM 2
#define D3DPOOL_DEFAULT 3
#define D3DPOOL_SCRATCH 4
typedef int D3DPOOL;

#define D3DX_FILTER_NONE 1
#define D3DX_FILTER_LINEAR 2

#define D3DTS_WORLD 1
#define D3DTS_VIEW 2
#define D3DTS_PROJECTION 3

#define D3DCLEAR_TARGET 1
#define D3DCLEAR_ZBUFFER 2

#define D3DFVF_XYZ 0x002
#define D3DFVF_XYZRHW 0x004
#define D3DFVF_NORMAL 0x010
#define D3DFVF_DIFFUSE 0x040
#define D3DFVF_SPECULAR 0x080
#define D3DFVF_TEX1 0x100
#define D3DFVF_TEX2 0x200
#define D3DFVF_TEX3 0x300
#define D3DFVF_TEX4 0x400
#define D3DFVF_TEXCOORDSIZE1(i) (3 << (i*2 + 16))
#define D3DFVF_TEXCOORDSIZE2(i) (0)
#define D3DFVF_TEXCOORDSIZE3(i) (1 << (i*2 + 16))
#define D3DFVF_TEXCOORDSIZE4(i) (2 << (i*2 + 16))
#define D3DFVF_TEXCOORDSIZEMASK(i) (3 << (i*2 + 16))
#define D3DFVF_TEXCOUNT_MASK 0xf00
#define D3DFVF_TEXCOUNT_SHIFT 8

#define D3DRS_ZENABLE 7
#define D3DRS_FILLMODE 8
#define D3DRS_SHADEMODE 9
#define D3DRS_ZWRITEENABLE 14
#define D3DRS_ALPHATESTENABLE 15
#define D3DRS_SRCBLEND 19
#define D3DRS_DESTBLEND 20
#define D3DRS_CULLMODE 22
#define D3DRS_ZFUNC 23
#define D3DRS_ALPHAREF 24
#define D3DRS_ALPHAFUNC 25
#define D3DRS_ALPHABLENDENABLE 27
#define D3DRS_LIGHTING 137
#define D3DRS_TEXTUREFACTOR 60
#define D3DTSS_COLORARG1 1
#define D3DTSS_COLORARG2 2
#define D3DTSS_COLOROP 3
#define D3DTSS_ALPHAARG1 4
#define D3DTSS_ALPHAARG2 5
#define D3DTSS_ALPHAOP 6
#define D3DTSS_MINFILTER 7
#define D3DTSS_MAGFILTER 8
#define D3DTSS_MIPFILTER 9
#define D3DTSS_ADDRESSU 10
#define D3DTSS_ADDRESSV 11

#define D3DPT_POINTLIST 1
#define D3DPT_LINELIST 2
#define D3DPT_LINESTRIP 3
#define D3DPT_TRIANGLELIST 4
#define D3DPT_TRIANGLESTRIP 5
#define D3DPT_TRIANGLEFAN 6

typedef RECT D3DRECT;
typedef struct _D3DLOCKED_RECT {
    int Pitch;
    void* pBits;
} D3DLOCKED_RECT;

#define D3DTA_TFACTOR 1
#define D3DTA_TEXTURE 2
#define D3DTA_CURRENT 3
#define D3DTA_DIFFUSE 4

#define D3DTOP_SELECTARG1 1
#define D3DTOP_DISABLE 2
#define D3DTOP_MODULATE 3
#define D3DTOP_ADD 4

#define D3DBLEND_INVDESTCOLOR 1
#define D3DBLEND_ONE 2
#define D3DBLEND_ZERO 3
#define D3DBLEND_SRCCOLOR 4
#define D3DBLEND_SRCALPHA 5
#define D3DBLEND_INVSRCALPHA 6

#define D3DCULL_CCW 1
#define D3DCULL_CW 2
#define D3DCULL_NONE 3

#define D3DFILL_SOLID 1
#define D3DFILL_WIREFRAME 2

#define D3DLIGHT_POINT 1
#define D3DSHADE_FLAT 1

#define D3DCMP_LESSEQUAL 1

#define D3DTEXF_NONE 0
#define D3DTEXF_POINT 1
#define D3DTEXF_LINEAR 2
#define D3DTEXF_ANISOTROPIC 3
#define D3DTEXF_FLATCUBIC 4
#define D3DTEXF_GAUSSIANCUBIC 5
#define D3DTADDRESS_WRAP 1
#define D3DTADDRESS_MIRROR 2
#define D3DTADDRESS_CLAMP 3
#define D3DTADDRESS_BORDER 4
#define D3DTADDRESS_MIRRORONCE 5
#define D3DTTFF_DISABLE 0
#define D3DTTFF_COUNT1 1
#define D3DTTFF_COUNT2 2
#define D3DTTFF_COUNT3 3
#define D3DTTFF_COUNT4 4
#define D3DTOP_DISABLE 1
#define D3DTOP_SELECTARG1 2
#define D3DTOP_SELECTARG2 3
#define D3DTOP_MODULATE 4
#define D3DTOP_MODULATE2X 5
#define D3DTOP_MODULATE4X 6
#define D3DTOP_ADD 7
#define D3DTOP_ADDSIGNED 8
#define D3DTOP_ADDSIGNED2X 9
#define D3DTOP_SUBTRACT 10
#define D3DTOP_ADDSMOOTH 11
#define D3DTOP_BLENDDIFFUSEALPHA 12
#define D3DTOP_BLENDTEXTUREALPHA 13
#define D3DTOP_BLENDFACTORALPHA 14
#define D3DTOP_BLENDTEXTUREALPHAPM 15
#define D3DTOP_BLENDCURRENTALPHA 16
#define D3DTOP_PREMODULATE 17
#define D3DTOP_MODULATEALPHA_ADDCOLOR 18
#define D3DTOP_MODULATECOLOR_ADDALPHA 19
#define D3DTOP_MODULATEINVALPHA_ADDCOLOR 20
#define D3DTOP_MODULATEINVCOLOR_ADDALPHA 21
#define D3DTOP_BUMPENVMAP 22
#define D3DTOP_BUMPENVMAPLUMINANCE 23
#define D3DTOP_DOTPRODUCT3 24
#define D3DTOP_MULTIPLYADD 25
#define D3DTOP_LERP 26
#define D3DTA_DIFFUSE 0
#define D3DTA_CURRENT 1
#define D3DTA_TEXTURE 2
#define D3DTA_TFACTOR 3
#define D3DTA_SPECULAR 4
#define D3DTA_TEMP 5
#define D3DTA_CONSTANT 6
#define D3DTA_COMPLEMENT 0x10
#define D3DTA_ALPHAREPLICATE 0x20
#define D3DFILL_POINT 1
#define D3DFILL_WIREFRAME 2
#define D3DFILL_SOLID 3
#define D3DSHADE_FLAT 1
#define D3DSHADE_GOURAUD 2
#define D3DSHADE_PHONG 3
#define D3DCMP_NEVER 1
#define D3DCMP_LESS 2
#define D3DCMP_EQUAL 3
#define D3DCMP_LESSEQUAL 4
#define D3DCMP_GREATER 5
#define D3DCMP_NOTEQUAL 6
#define D3DCMP_GREATEREQUAL 7
#define D3DCMP_ALWAYS 8
#define D3DBLEND_ZERO 1
#define D3DBLEND_ONE 2
#define D3DBLEND_SRCCOLOR 3
#define D3DBLEND_INVSRCCOLOR 4
#define D3DBLEND_SRCALPHA 5
#define D3DBLEND_INVSRCALPHA 6
#define D3DBLEND_DESTALPHA 7
#define D3DBLEND_INVDESTALPHA 8
#define D3DBLEND_DESTCOLOR 9
#define D3DBLEND_INVDESTCOLOR 10
#define D3DBLEND_SRCALPHASAT 11
#define D3DBLEND_BOTHSRCALPHA 12
#define D3DBLEND_BOTHINVSRCALPHA 13
#define D3DTSS_COLOROP 1
#define D3DTSS_COLORARG1 2
#define D3DTSS_COLORARG2 3
#define D3DTSS_ALPHAOP 4
#define D3DTSS_ALPHAARG1 5
#define D3DTSS_ALPHAARG2 6
#define D3DTSS_BUMPENVMAT00 7
#define D3DTSS_BUMPENVMAT01 8
#define D3DTSS_BUMPENVMAT10 9
#define D3DTSS_BUMPENVMAT11 10
#define D3DTSS_TEXCOORDINDEX 11
#define D3DTSS_ADDRESS 16
#define D3DTSS_MAGFILTER 17
#define D3DTSS_MINFILTER 18
#define D3DTSS_MIPFILTER 19
#define D3DTSS_MIPMAPLODBIAS 20
#define D3DTSS_MAXMIPLEVEL 21
#define D3DTSS_MAXANISOTROPY 22
#define D3DTSS_ADDRESSU 24
#define D3DTSS_ADDRESSV 25
#define D3DTSS_BORDERCOLOR 26
#define D3DTSS_TCI_PASSTHRU 0
#define D3DTSS_TCI_CAMERASPACENORMAL 0x10000
#define D3DTSS_TCI_CAMERASPACEPOSITION 0x20000
#define D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR 0x30000
#define D3DTS_WORLD 256
#define D3DTS_VIEW 2
#define D3DTS_PROJECTION 3
#define D3DTS_TEXTURE0 16
#define D3DTS_TEXTURE1 17
#define D3DTS_TEXTURE2 18
#define D3DTS_TEXTURE3 19
#define D3DTS_TEXTURE4 20
#define D3DTS_TEXTURE5 21
#define D3DTS_TEXTURE6 22
#define D3DTS_TEXTURE7 23
#define D3DCAPS2_CANRENDERWINDOWED 0x1
#define D3DCAPS2_FULLSCREENGAMMA 0x2


#define D3DLOCK_READONLY 1
#define D3DLOCK_NO_DIRTY_UPDATE 2
#define D3DLOCK_DISCARD 3
#define D3DLOCK_NOSYSLOCK 4
#define D3DFMT_R3G3B2 27
#define D3DFMT_A8R3G3B2 28
#define D3DFMT_X4R4G4B4 29
#define D3DFMT_A2B10G10R10 30
#define D3DLIGHT_POINT 1
#define D3DLIGHT_SPOT 2
#define D3DLIGHT_DIRECTIONAL 3
#define D3DFOG_NONE 0
#define D3DFOG_EXP 1
#define D3DFOG_EXP2 2
#define D3DFOG_LINEAR 3
#define D3DBACKBUFFER_TYPE_MONO 0
#define D3DSGR_NO_CALIBRATION 0
#define D3DSGR_CALIBRATE 1
typedef struct _D3DGAMMARAMP {
    WORD red[256];
    WORD green[256];
    WORD blue[256];
} D3DGAMMARAMP;
typedef struct _D3DSURFACE_DESC {
    int Format;
    int Type;
    DWORD Usage;
    int Pool;
    UINT Size;
    DWORD MultiSampleType;
    UINT Width, Height;
} D3DSURFACE_DESC;
#define D3DVSDE_POSITION 0
#define D3DVSDE_BLENDWEIGHT 1
#define D3DVSDE_BLENDINDICES 2
#define D3DVSDE_NORMAL 3
#define D3DVSDE_PSIZE 4
#define D3DVSDE_DIFFUSE 5
#define D3DVSDE_SPECULAR 6
#define D3DVSDE_TEXCOORD0 7
#define D3DVSDE_TEXCOORD1 8
#define D3DVSDE_TEXCOORD2 9
#define D3DVSDE_TEXCOORD3 10
#define D3DVSDE_TEXCOORD4 11
#define D3DVSDE_TEXCOORD5 12
#define D3DVSDE_TEXCOORD6 13
#define D3DVSDE_TEXCOORD7 14
#define D3DVSDE_POSITION2 15
#define D3DVSDE_NORMAL2 16
#define D3DUSAGE_WRITEONLY 1
#define D3DUSAGE_DYNAMIC 2
#define D3DUSAGE_DEPTHSTENCIL 3
#define D3DUSAGE_RENDERTARGET 4

#define D3D_OK 0
#define D3DERR_DEVICELOST 1
#define D3DERR_DEVICENOTRESET 2
#define D3DERR_INVALIDCALL 3
#define D3DERR_NOTAVAILABLE 4
#define D3DERR_OUTOFVIDEOMEMORY 5
#define D3DERR_WASSTILLDRAWING 6

#define D3DADAPTER_DEFAULT 0
#define D3DSWAPEFFECT_COPY 1
#define D3DSWAPEFFECT_DISCARD 2
#define D3DPRESENT_INTERVAL_IMMEDIATE 1
#define D3DPRESENTFLAG_LOCKABLE_BACKBUFFER 1
#define D3DENUM_NO_WHQL_LEVEL 1

#define D3DCREATE_PUREDEVICE 1
#define D3DCREATE_HARDWARE_VERTEXPROCESSING 2
#define D3DCREATE_MIXED_VERTEXPROCESSING 3
#define D3DCREATE_SOFTWARE_VERTEXPROCESSING 4

#define D3DRTYPE_SURFACE 1
#define D3DRTYPE_TEXTURE 2
#define D3DMULTISAMPLE_NONE 0

#define D3DDEVICETYPE_HAL 1
#define D3DDEVICETYPE_REF 2

#define D3DPMISCCAPS_CLIPTLVERTS 1
#define D3DVTXPCAPS_DIRECTIONALLIGHTS 1
#define D3DVTXPCAPS_POSITIONALLIGHTS 2
#define D3DVTXPCAPS_TEXGEN 3
#define D3DCAPS2_CANRENDERWINDOWED 1
#define D3DDEVCAPS_HWTRANSFORMANDLIGHT 1
#define D3DDEVCAPS_PUREDEVICE 2

#define D3DRS_FOGENABLE 28
#define D3DTSS_TEXTURETRANSFORMFLAGS 27
#define D3DTTFF_PROJECTED 256
#define D3DRS_DIFFUSEMATERIALSOURCE 145
#define D3DRS_SPECULARMATERIALSOURCE 146
#define D3DRS_AMBIENTMATERIALSOURCE 147
#define D3DRS_EMISSIVEMATERIALSOURCE 148
#define D3DRS_LINEPATTERN 10
#define D3DRS_LASTPIXEL 16
#define D3DRS_ZVISIBLE 30
#define D3DRS_FOGSTART 36
#define D3DRS_FOGEND 37
#define D3DRS_FOGDENSITY 38
#define D3DRS_EDGEANTIALIAS 40
#define D3DRS_ZBIAS 47
#define D3DRS_STENCILWRITEMASK 59
#define D3DRS_AMBIENT 139
#define D3DRS_LOCALVIEWER 142
#define D3DRS_NORMALIZENORMALS 143
#define D3DRS_VERTEXBLEND 151
#define D3DRS_CLIPPLANEENABLE 152
#define D3DRS_SOFTWAREVERTEXPROCESSING 153
#define D3DRS_MULTISAMPLEANTIALIAS 161
#define D3DRS_MULTISAMPLEMASK 162
#define D3DRS_INDEXEDVERTEXBLENDENABLE 167
#define D3DRS_COLORWRITEENABLE 168
#define D3DRS_BLENDOP 171
#define D3DRS_POSITIONDEGREE 172
#define D3DRS_NORMALDEGREE 173
#define D3DRS_SCISSORTESTENABLE 174
#define D3DRS_SLOPESCALEDEPTHBIAS 175
#define D3DRS_ANTIALIASEDLINEENABLE 176
#define D3DRS_MINTESSELLATIONLEVEL 177
#define D3DRS_MAXTESSELLATIONLEVEL 178
#define D3DRS_ADAPTIVETESS_X 179
#define D3DRS_ADAPTIVETESS_Y 180
#define D3DRS_ADAPTIVETESS_Z 181
#define D3DRS_ADAPTIVETESS_W 182
#define D3DRS_ENABLEADAPTIVETESSELLATION 183
#define D3DRS_TWOSIDEDSTENCILMODE 184
#define D3DRS_CCW_STENCILFAIL 185
#define D3DRS_CCW_STENCILZFAIL 186
#define D3DRS_CCW_STENCILPASS 187
#define D3DRS_CCW_STENCILFUNC 188
#define D3DRS_COLORWRITEENABLE1 189
#define D3DRS_COLORWRITEENABLE2 190
#define D3DRS_COLORWRITEENABLE3 191
#define D3DRS_BLENDFACTOR 192
#define D3DRS_SRGBWRITEENABLE 193
#define D3DRS_DEPTHBIAS 194
#define D3DRS_WRAP8 195
#define D3DRS_WRAP9 196

#define D3DPTFILTERCAPS_MINFANISOTROPIC 0x0400
#define D3DPTFILTERCAPS_MAGFANISOTROPIC 0x4000000
#define D3DMCS_MATERIAL 1
#define D3DMCS_COLOR1   2
#define D3DMCS_COLOR2   3
#define D3DMCS_DISABLE  0
#define D3DVBF_DISABLE  0
#define D3DBLENDOP_ADD      1
#define D3DBLENDOP_SUBTRACT 2
#define D3DBLENDOP_REVSUBTRACT 3
#define D3DBLENDOP_MIN      4
#define D3DBLENDOP_MAX      5
#define D3DRS_WRAP10 197
#define D3DRS_WRAP11 198
#define D3DRS_WRAP12 199
#define D3DRS_WRAP13 200
#define D3DRS_WRAP14 201
#define D3DRS_WRAP15 202
#define D3DRS_SEPARATEALPHABLENDENABLE 203
#define D3DRS_SRCBLENDALPHA 204
#define D3DRS_DESTBLENDALPHA 205
#define D3DRS_BLENDOPALPHA 206
#define D3DRS_FOGCOLOR 34
#define D3DRS_FOGTABLEMODE 35
#define D3DRS_FOGVERTEXMODE 140
#define D3DRS_RANGEFOGENABLE 48
#define D3DRS_DITHERENABLE 26
#define D3DRS_STENCILENABLE 52
#define D3DRS_CLIPPING 136
#define D3DRS_SPECULARENABLE 29
#define D3DRS_COLORVERTEX 141
#define D3DRS_WRAP0 128
#define D3DRS_WRAP1 129
#define D3DRS_WRAP2 130
#define D3DRS_WRAP3 131
#define D3DRS_WRAP4 132
#define D3DRS_WRAP5 133
#define D3DRS_WRAP6 134
#define D3DRS_WRAP7 135
#define D3DDEVTYPE_HAL 1
#define D3DDEVTYPE_REF 2
#define D3DPTADDRESSCAPS_BORDER 0x00000080
#define D3DDEVCAPS_HWTRANSFORMANDLIGHT 0x00000010
#define D3DDEVCAPS_PUREDEVICE 0x00000010
#define D3DCAPS2_CANRENDERWINDOWED 0x00080000
#define D3DPMISCCAPS_CLIPTLVERTS 0x00000100
#define D3DVTXPCAPS_DIRECTIONALLIGHTS 0x00000008
#define D3DVTXPCAPS_POSITIONALLIGHTS 0x00000010
#define D3DVTXPCAPS_TEXGEN 0x00000040

#define D3DVS_VERSION(major, minor) (0xFFFE0000 | ((major) << 8) | (minor))
#define D3D_SDK_VERSION 0
#define D3DX_PI 3.141592654f
#define D3DXToRadian( degree ) ((degree) * (D3DX_PI / 180.0f))
#define D3DXToDegree( radian ) ((radian) * (180.0f / D3DX_PI))

struct D3DVIEWPORT8 {
    DWORD X, Y, Width, Height;
    float MinZ, MaxZ;
};

struct D3DCAPS8 {
        DWORD TextureFilterCaps;
        DWORD MaxAnisotropy;
        DWORD MaxPrimitiveCount;
        DWORD MaxVertexIndex;
        DWORD MaxStreams;
        DWORD MaxStreamStride;
        DWORD MaxActiveLights;
        DWORD MaxUserClipPlanes;
        DWORD MaxVertexBlendMatrices;
        DWORD MaxPointSize;
        DWORD PixelShaderVersion;
        DWORD MaxSimultaneousTextures;
    DWORD TextureCaps;
    DWORD MaxTextureWidth, MaxTextureHeight;
    DWORD DevCaps;
    DWORD Caps2;
    DWORD VertexProcessingCaps;
    DWORD PrimitiveMiscCaps;
    DWORD TextureAddressCaps;
    DWORD VertexShaderVersion;
};

struct D3DPRESENT_PARAMETERS {
    UINT BackBufferWidth, BackBufferHeight;
    int Windowed;
    D3DFORMAT BackBufferFormat;
    UINT BackBufferCount;
    int MultiSampleType;
    int SwapEffect;
    HWND hDeviceWindow;
    UINT FullScreen_RefreshRateInHz;
    UINT FullScreen_PresentationInterval;
    DWORD Flags;
    BOOL EnableAutoDepthStencil;
    D3DFORMAT AutoDepthStencilFormat;
};

struct D3DADAPTER_IDENTIFIER8 {
    char Driver[512];
    char Description[512];
    LARGE_INTEGER DriverVersion;
    DWORD VendorId, DeviceId, SubSysId, Revision;
    GUID DeviceIdentifier;
    DWORD WHQLLevel;
};

struct D3DDISPLAYMODE {
    UINT Width, Height, RefreshRate;
    D3DFORMAT Format;
};

struct D3DXIMAGE_INFO {
    UINT Width;
    UINT Height;
    UINT Depth;
    UINT MipLevels;
    int Format;
    int ResourceType;
    int ImageFileFormat;
};

// Simple D3DX Math simulation for OpenGL
struct GLMATRIX {
    // MSVC compat: allow &(temporary) and uniform &x usage
    GLMATRIX* operator&() { return this; }
    const GLMATRIX* operator&() const { return this; }

    union {
        struct {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
        float m[4][4];
    };
    
    GLMATRIX() {
        memset(m, 0, sizeof(m));
        m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
    }

    GLMATRIX operator*(const GLMATRIX& rhs) const {
        GLMATRIX o;
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                o.m[r][c] = m[r][0]*rhs.m[0][c] + m[r][1]*rhs.m[1][c] + m[r][2]*rhs.m[2][c] + m[r][3]*rhs.m[3][c];
        return o;
    }
    GLMATRIX& operator*=(const GLMATRIX& rhs) { *this = *this * rhs; return *this; }
    GLMATRIX& operator+=(const GLMATRIX& rhs) {
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                m[r][c] += rhs.m[r][c];
        return *this;
    }
};

struct GLVECTOR3 {
    // MSVC compat: allow &(temporary) and uniform &x usage
    GLVECTOR3* operator&() { return this; }
    const GLVECTOR3* operator&() const { return this; }

    float x, y, z;
    
    GLVECTOR3() : x(0), y(0), z(0) {}
    GLVECTOR3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    
    GLVECTOR3 operator+(const GLVECTOR3& rhs) const { return GLVECTOR3(x+rhs.x, y+rhs.y, z+rhs.z); }
    GLVECTOR3 operator-(const GLVECTOR3& rhs) const { return GLVECTOR3(x-rhs.x, y-rhs.y, z-rhs.z); }
    GLVECTOR3 operator-() const { return GLVECTOR3(-x, -y, -z); }
    GLVECTOR3 operator*(float s) const { return GLVECTOR3(x*s, y*s, z*s); }
    GLVECTOR3 operator/(float s) const { return GLVECTOR3(x/s, y/s, z/s); }
    GLVECTOR3& operator+=(const GLVECTOR3& rhs) { x+=rhs.x; y+=rhs.y; z+=rhs.z; return *this; }
    GLVECTOR3& operator-=(const GLVECTOR3& rhs) { x-=rhs.x; y-=rhs.y; z-=rhs.z; return *this; }
    GLVECTOR3& operator*=(float s) { x*=s; y*=s; z*=s; return *this; }
    GLVECTOR3& operator/=(float s) { x/=s; y/=s; z/=s; return *this; }
    bool operator==(const GLVECTOR3& rhs) const { return x==rhs.x && y==rhs.y && z==rhs.z; }
    bool operator!=(const GLVECTOR3& rhs) const { return !(*this == rhs); }
};

inline GLVECTOR3 operator*(float s, const GLVECTOR3& v) {
    return GLVECTOR3(v.x * s, v.y * s, v.z * s);
}

inline GLVECTOR3* D3DXVec3Normalize(GLVECTOR3* pOut, const GLVECTOR3* pV) {
    float len = sqrtf(pV->x * pV->x + pV->y * pV->y + pV->z * pV->z);
    if (len > 0.0f) {
        pOut->x = pV->x / len;
        pOut->y = pV->y / len;
        pOut->z = pV->z / len;
    } else {
        pOut->x = pOut->y = pOut->z = 0.0f;
    }
    return pOut;
}

struct GLVECTOR2 {
    // MSVC compat: allow &(temporary) and uniform &x usage
    GLVECTOR2* operator&() { return this; }
    const GLVECTOR2* operator&() const { return this; }

    float x, y;
    GLVECTOR2() : x(0), y(0) {}
    GLVECTOR2(float _x, float _y) : x(_x), y(_y) {}
};

struct GLCOLOR {
    // MSVC compat: allow &(temporary) and uniform &x usage
    GLCOLOR* operator&() { return this; }
    const GLCOLOR* operator&() const { return this; }

    float r, g, b, a;
    GLCOLOR() : r(0), g(0), b(0), a(0) {}
    GLCOLOR(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
    GLCOLOR(unsigned int dw) {
        a = ((dw >> 24) & 0xff) / 255.0f;
        r = ((dw >> 16) & 0xff) / 255.0f;
        g = ((dw >> 8)  & 0xff) / 255.0f;
        b = ( dw        & 0xff) / 255.0f;
    }
    operator unsigned int() const {
        unsigned int A = (unsigned int)(a * 255.0f + 0.5f) & 0xff;
        unsigned int R = (unsigned int)(r * 255.0f + 0.5f) & 0xff;
        unsigned int G = (unsigned int)(g * 255.0f + 0.5f) & 0xff;
        unsigned int B = (unsigned int)(b * 255.0f + 0.5f) & 0xff;
        return (A << 24) | (R << 16) | (G << 8) | B;
    }
    bool operator==(const GLCOLOR& o) const { return r==o.r && g==o.g && b==o.b && a==o.a; }
    bool operator!=(const GLCOLOR& o) const { return !(*this == o); }
};

typedef GLCOLOR D3DCOLORVALUE;
typedef struct _D3DMATERIAL8 {
    GLCOLOR Diffuse, Ambient, Specular, Emissive;
    float Power;
} D3DMATERIAL8;


struct GLVECTOR4 {
    // MSVC compat: allow &(temporary) and uniform &x usage
    GLVECTOR4* operator&() { return this; }
    const GLVECTOR4* operator&() const { return this; }

    float x, y, z, w;
    GLVECTOR4() : x(0), y(0), z(0), w(0) {}
    GLVECTOR4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
};
struct GLQUATERNION {
    // MSVC compat: allow &(temporary) and uniform &x usage
    GLQUATERNION* operator&() { return this; }
    const GLQUATERNION* operator&() const { return this; }

    float x, y, z, w;
    GLQUATERNION() : x(0), y(0), z(0), w(1) {}
    GLQUATERNION(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
};
struct GLPLANE {
    // MSVC compat: allow &(temporary) and uniform &x usage
    GLPLANE* operator&() { return this; }
    const GLPLANE* operator&() const { return this; }

    float a, b, c, d;
    GLPLANE() : a(0), b(0), c(0), d(0) {}
    GLPLANE(float _a, float _b, float _c, float _d) : a(_a), b(_b), c(_c), d(_d) {}
};

inline float D3DXVec3Dot(const GLVECTOR3* pV1, const GLVECTOR3* pV2) {
    return pV1->x*pV2->x + pV1->y*pV2->y + pV1->z*pV2->z;
}
inline GLVECTOR3* D3DXVec3Cross(GLVECTOR3* pOut, const GLVECTOR3* pV1, const GLVECTOR3* pV2) {
    pOut->x = pV1->y * pV2->z - pV1->z * pV2->y;
    pOut->y = pV1->z * pV2->x - pV1->x * pV2->z;
    pOut->z = pV1->x * pV2->y - pV1->y * pV2->x;
    return pOut;
}
inline float D3DXVec3LengthSq(const GLVECTOR3* pV) {
    return pV->x*pV->x + pV->y*pV->y + pV->z*pV->z;
}
inline float D3DXVec3Length(const GLVECTOR3* pV) {
    return sqrtf(D3DXVec3LengthSq(pV));
}
inline float D3DXVec2Length(const GLVECTOR2* pV) {
    return sqrtf(pV->x*pV->x + pV->y*pV->y);
}
inline float D3DXVec2Dot(const GLVECTOR2* pV1, const GLVECTOR2* pV2) {
    return pV1->x*pV2->x + pV1->y*pV2->y;
}
inline GLVECTOR2* D3DXVec2Normalize(GLVECTOR2* pOut, const GLVECTOR2* pV) {
    float len = D3DXVec2Length(pV);
    if (len > 0) { pOut->x = pV->x/len; pOut->y = pV->y/len; }
    else { pOut->x = pOut->y = 0; }
    return pOut;
}
inline GLVECTOR4* D3DXVec3Transform(GLVECTOR4* pOut, const GLVECTOR3* pV, const GLMATRIX* pM) {
    float x = pV->x*pM->m[0][0] + pV->y*pM->m[1][0] + pV->z*pM->m[2][0] + pM->m[3][0];
    float y = pV->x*pM->m[0][1] + pV->y*pM->m[1][1] + pV->z*pM->m[2][1] + pM->m[3][1];
    float z = pV->x*pM->m[0][2] + pV->y*pM->m[1][2] + pV->z*pM->m[2][2] + pM->m[3][2];
    float w = pV->x*pM->m[0][3] + pV->y*pM->m[1][3] + pV->z*pM->m[2][3] + pM->m[3][3];
    pOut->x=x; pOut->y=y; pOut->z=z; pOut->w=w;
    return pOut;
}
inline GLVECTOR3* D3DXVec3TransformCoord(GLVECTOR3* pOut, const GLVECTOR3* pV, const GLMATRIX* pM) {
    float x = pV->x*pM->m[0][0] + pV->y*pM->m[1][0] + pV->z*pM->m[2][0] + pM->m[3][0];
    float y = pV->x*pM->m[0][1] + pV->y*pM->m[1][1] + pV->z*pM->m[2][1] + pM->m[3][1];
    float z = pV->x*pM->m[0][2] + pV->y*pM->m[1][2] + pV->z*pM->m[2][2] + pM->m[3][2];
    float w = pV->x*pM->m[0][3] + pV->y*pM->m[1][3] + pV->z*pM->m[2][3] + pM->m[3][3];
    float iw = w != 0.0f ? 1.0f / w : 0.0f;
    pOut->x = x * iw; pOut->y = y * iw; pOut->z = z * iw;
    return pOut;
}
inline GLMATRIX* D3DXMatrixIdentity(GLMATRIX* pOut) {
    memset(pOut->m, 0, sizeof(pOut->m));
    pOut->m[0][0] = pOut->m[1][1] = pOut->m[2][2] = pOut->m[3][3] = 1.0f;
    return pOut;
}
inline GLMATRIX* D3DXMatrixInverse(GLMATRIX* pOut, float* pDeterminant, const GLMATRIX* pM) {
    const float* a = &pM->m[0][0];
    float inv[16];
    inv[0] = a[5]*a[10]*a[15] - a[5]*a[11]*a[14] - a[9]*a[6]*a[15] + a[9]*a[7]*a[14] + a[13]*a[6]*a[11] - a[13]*a[7]*a[10];
    inv[4] = -a[4]*a[10]*a[15] + a[4]*a[11]*a[14] + a[8]*a[6]*a[15] - a[8]*a[7]*a[14] - a[12]*a[6]*a[11] + a[12]*a[7]*a[10];
    inv[8] = a[4]*a[9]*a[15] - a[4]*a[11]*a[13] - a[8]*a[5]*a[15] + a[8]*a[7]*a[13] + a[12]*a[5]*a[11] - a[12]*a[7]*a[9];
    inv[12] = -a[4]*a[9]*a[14] + a[4]*a[10]*a[13] + a[8]*a[5]*a[14] - a[8]*a[6]*a[13] - a[12]*a[5]*a[10] + a[12]*a[6]*a[9];
    inv[1] = -a[1]*a[10]*a[15] + a[1]*a[11]*a[14] + a[9]*a[2]*a[15] - a[9]*a[3]*a[14] - a[13]*a[2]*a[11] + a[13]*a[3]*a[10];
    inv[5] = a[0]*a[10]*a[15] - a[0]*a[11]*a[14] - a[8]*a[2]*a[15] + a[8]*a[3]*a[14] + a[12]*a[2]*a[11] - a[12]*a[3]*a[10];
    inv[9] = -a[0]*a[9]*a[15] + a[0]*a[11]*a[13] + a[8]*a[1]*a[15] - a[8]*a[3]*a[13] - a[12]*a[1]*a[11] + a[12]*a[3]*a[9];
    inv[13] = a[0]*a[9]*a[14] - a[0]*a[10]*a[13] - a[8]*a[1]*a[14] + a[8]*a[2]*a[13] + a[12]*a[1]*a[10] - a[12]*a[2]*a[9];
    inv[2] = a[1]*a[6]*a[15] - a[1]*a[7]*a[14] - a[5]*a[2]*a[15] + a[5]*a[3]*a[14] + a[13]*a[2]*a[7] - a[13]*a[3]*a[6];
    inv[6] = -a[0]*a[6]*a[15] + a[0]*a[7]*a[14] + a[4]*a[2]*a[15] - a[4]*a[3]*a[14] - a[12]*a[2]*a[7] + a[12]*a[3]*a[6];
    inv[10] = a[0]*a[5]*a[15] - a[0]*a[7]*a[13] - a[4]*a[1]*a[15] + a[4]*a[3]*a[13] + a[12]*a[1]*a[7] - a[12]*a[3]*a[5];
    inv[14] = -a[0]*a[5]*a[14] + a[0]*a[6]*a[13] + a[4]*a[1]*a[14] - a[4]*a[2]*a[13] - a[12]*a[1]*a[6] + a[12]*a[2]*a[5];
    inv[3] = -a[1]*a[6]*a[11] + a[1]*a[7]*a[10] + a[5]*a[2]*a[11] - a[5]*a[3]*a[10] - a[9]*a[2]*a[7] + a[9]*a[3]*a[6];
    inv[7] = a[0]*a[6]*a[11] - a[0]*a[7]*a[10] - a[4]*a[2]*a[11] + a[4]*a[3]*a[10] + a[8]*a[2]*a[7] - a[8]*a[3]*a[6];
    inv[11] = -a[0]*a[5]*a[11] + a[0]*a[7]*a[9] + a[4]*a[1]*a[11] - a[4]*a[3]*a[9] - a[8]*a[1]*a[7] + a[8]*a[3]*a[5];
    inv[15] = a[0]*a[5]*a[10] - a[0]*a[6]*a[9] - a[4]*a[1]*a[10] + a[4]*a[2]*a[9] + a[8]*a[1]*a[6] - a[8]*a[2]*a[5];
    float det = a[0]*inv[0] + a[1]*inv[4] + a[2]*inv[8] + a[3]*inv[12];
    if (pDeterminant) *pDeterminant = det;
    if (det == 0.0f) return NULL;
    float id = 1.0f / det;
    float* o = &pOut->m[0][0];
    for (int i = 0; i < 16; ++i) o[i] = inv[i] * id;
    return pOut;
}
inline GLMATRIX* D3DXMatrixMultiply(GLMATRIX* pOut, const GLMATRIX* pM1, const GLMATRIX* pM2) {
    GLMATRIX out;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            out.m[i][j] = pM1->m[i][0] * pM2->m[0][j] + pM1->m[i][1] * pM2->m[1][j] +
                          pM1->m[i][2] * pM2->m[2][j] + pM1->m[i][3] * pM2->m[3][j];
        }
    }
    *pOut = out;
    return pOut;
}
inline GLMATRIX* D3DXMatrixTranslation(GLMATRIX* pOut, float x, float y, float z) {
    D3DXMatrixIdentity(pOut);
    pOut->m[3][0] = x; pOut->m[3][1] = y; pOut->m[3][2] = z;
    return pOut;
}
inline GLMATRIX* D3DXMatrixScaling(GLMATRIX* pOut, float x, float y, float z) {
    D3DXMatrixIdentity(pOut);
    pOut->m[0][0] = x; pOut->m[1][1] = y; pOut->m[2][2] = z;
    return pOut;
}
inline GLMATRIX* D3DXMatrixRotationX(GLMATRIX* pOut, float angle) {
    float s = sinf(angle), c = cosf(angle);
    D3DXMatrixIdentity(pOut);
    pOut->m[1][1] = c; pOut->m[1][2] = s;
    pOut->m[2][1] = -s; pOut->m[2][2] = c;
    return pOut;
}
inline GLMATRIX* D3DXMatrixRotationY(GLMATRIX* pOut, float angle) {
    float s = sinf(angle), c = cosf(angle);
    D3DXMatrixIdentity(pOut);
    pOut->m[0][0] = c; pOut->m[0][2] = -s;
    pOut->m[2][0] = s; pOut->m[2][2] = c;
    return pOut;
}
inline GLMATRIX* D3DXMatrixRotationZ(GLMATRIX* pOut, float angle) {
    float s = sinf(angle), c = cosf(angle);
    D3DXMatrixIdentity(pOut);
    pOut->m[0][0] = c; pOut->m[0][1] = s;
    pOut->m[1][0] = -s; pOut->m[1][1] = c;
    return pOut;
}
inline GLMATRIX* D3DXMatrixRotationQuaternion(GLMATRIX* pOut, const GLQUATERNION* pQ) {
    float x2 = pQ->x + pQ->x, y2 = pQ->y + pQ->y, z2 = pQ->z + pQ->z;
    float xx = pQ->x * x2, xy = pQ->x * y2, xz = pQ->x * z2;
    float yy = pQ->y * y2, yz = pQ->y * z2, zz = pQ->z * z2;
    float wx = pQ->w * x2, wy = pQ->w * y2, wz = pQ->w * z2;
    D3DXMatrixIdentity(pOut);
    pOut->_11 = 1.0f - (yy + zz); pOut->_12 = xy + wz;        pOut->_13 = xz - wy;
    pOut->_21 = xy - wz;          pOut->_22 = 1.0f - (xx + zz); pOut->_23 = yz + wx;
    pOut->_31 = xz + wy;          pOut->_32 = yz - wx;        pOut->_33 = 1.0f - (xx + yy);
    return pOut;
}
inline GLQUATERNION* D3DXQuaternionMultiply(GLQUATERNION* pOut, const GLQUATERNION* pQ1, const GLQUATERNION* pQ2) {
    GLQUATERNION o;
    o.x = pQ1->w*pQ2->x + pQ1->x*pQ2->w + pQ1->y*pQ2->z - pQ1->z*pQ2->y;
    o.y = pQ1->w*pQ2->y - pQ1->x*pQ2->z + pQ1->y*pQ2->w + pQ1->z*pQ2->x;
    o.z = pQ1->w*pQ2->z + pQ1->x*pQ2->y - pQ1->y*pQ2->x + pQ1->z*pQ2->w;
    o.w = pQ1->w*pQ2->w - pQ1->x*pQ2->x - pQ1->y*pQ2->y - pQ1->z*pQ2->z;
    *pOut = o; return pOut;
}
inline GLQUATERNION* D3DXQuaternionConjugate(GLQUATERNION* pOut, const GLQUATERNION* pQ) {
    pOut->x = -pQ->x; pOut->y = -pQ->y; pOut->z = -pQ->z; pOut->w = pQ->w; return pOut;
}
inline GLQUATERNION* D3DXQuaternionRotationAxis(GLQUATERNION* pOut, const GLVECTOR3* pV, float angle) {
    float half = angle * 0.5f, s = sinf(half);
    pOut->x = pV->x * s; pOut->y = pV->y * s; pOut->z = pV->z * s; pOut->w = cosf(half);
    return pOut;
}
inline GLQUATERNION* D3DXQuaternionRotationYawPitchRoll(GLQUATERNION* pOut, float yaw, float pitch, float roll) {
    float cy = cosf(yaw*0.5f), sy = sinf(yaw*0.5f);
    float cp = cosf(pitch*0.5f), sp = sinf(pitch*0.5f);
    float cr = cosf(roll*0.5f), sr = sinf(roll*0.5f);
    pOut->x = cy*sp*cr + sy*cp*sr;
    pOut->y = sy*cp*cr - cy*sp*sr;
    pOut->z = cy*cp*sr - sy*sp*cr;
    pOut->w = cy*cp*cr + sy*sp*sr;
    return pOut;
}
inline GLVECTOR3* D3DXVec3Translation(GLVECTOR3* pOut, const GLVECTOR3* pV, const GLMATRIX* pM) { *pOut = *pV; return pOut; }
inline GLVECTOR3* D3DXVec3TransformNormal(GLVECTOR3* pOut, const GLVECTOR3* pV, const GLMATRIX* pM) {
    GLVECTOR3 o;
    o.x = pV->x*pM->_11 + pV->y*pM->_21 + pV->z*pM->_31;
    o.y = pV->x*pM->_12 + pV->y*pM->_22 + pV->z*pM->_32;
    o.z = pV->x*pM->_13 + pV->y*pM->_23 + pV->z*pM->_33;
    *pOut = o; return pOut;
}
inline GLVECTOR3* D3DXVec3Scale(GLVECTOR3* pOut, const GLVECTOR3* pV, float s) {
    pOut->x = pV->x*s; pOut->y = pV->y*s; pOut->z = pV->z*s; return pOut;
}
inline GLVECTOR3* D3DXVec3Add(GLVECTOR3* pOut, const GLVECTOR3* pV1, const GLVECTOR3* pV2) {
    pOut->x = pV1->x+pV2->x; pOut->y = pV1->y+pV2->y; pOut->z = pV1->z+pV2->z; return pOut;
}
inline GLVECTOR3* D3DXVec3Subtract(GLVECTOR3* pOut, const GLVECTOR3* pV1, const GLVECTOR3* pV2) {
    pOut->x = pV1->x-pV2->x; pOut->y = pV1->y-pV2->y; pOut->z = pV1->z-pV2->z; return pOut;
}
inline float D3DXPlaneDotCoord(const GLPLANE* pP, const GLVECTOR3* pV) { return pP->a*pV->x + pP->b*pV->y + pP->c*pV->z + pP->d; }
inline float D3DXMatrixfDeterminant(const GLMATRIX* pM) { GLMATRIX t; float d = 0.0f; D3DXMatrixInverse(&t, &d, pM); return d; }
inline GLMATRIX* D3DXMatrixLookAtRH(GLMATRIX* pOut, const GLVECTOR3* pEye, const GLVECTOR3* pAt, const GLVECTOR3* pUp) {
    GLVECTOR3 f, s, u;
    GLVECTOR3 tmp(pAt->x - pEye->x, pAt->y - pEye->y, pAt->z - pEye->z);
    D3DXVec3Normalize(&f, &tmp);
    D3DXVec3Cross(&s, &f, pUp);
    D3DXVec3Normalize(&s, &s);
    D3DXVec3Cross(&u, &s, &f);

    D3DXMatrixIdentity(pOut);
    pOut->m[0][0] = s.x; pOut->m[1][0] = s.y; pOut->m[2][0] = s.z;
    pOut->m[0][1] = u.x; pOut->m[1][1] = u.y; pOut->m[2][1] = u.z;
    pOut->m[0][2] = -f.x; pOut->m[1][2] = -f.y; pOut->m[2][2] = -f.z;
    pOut->m[3][0] = -D3DXVec3Dot(&s, pEye);
    pOut->m[3][1] = -D3DXVec3Dot(&u, pEye);
    pOut->m[3][2] = D3DXVec3Dot(&f, pEye);
    return pOut;
}
inline GLMATRIX* D3DXMatrixPerspectiveFovRH(GLMATRIX* pOut, float fovy, float Aspect, float zn, float zf) {
    float h = 1.0f / tanf(fovy / 2.0f);
    float w = h / Aspect;
    D3DXMatrixIdentity(pOut);
    pOut->m[0][0] = w;
    pOut->m[1][1] = h;
    pOut->m[2][2] = zf / (zn - zf);
    pOut->m[2][3] = -1.0f;
    pOut->m[3][2] = (zn * zf) / (zn - zf);
    pOut->m[3][3] = 0.0f;
    return pOut;
}
inline GLMATRIX* D3DXMatrixOrthoRH(GLMATRIX* pOut, float w, float h, float zn, float zf) {
    D3DXMatrixIdentity(pOut);
    pOut->m[0][0] = 2.0f / w;
    pOut->m[1][1] = 2.0f / h;
    pOut->m[2][2] = 1.0f / (zn - zf);
    pOut->m[3][2] = zn / (zn - zf);
    return pOut;
}
inline GLMATRIX* D3DXMatrixOrthoOffCenterRH(GLMATRIX* pOut, float l, float r, float b, float t, float zn, float zf) { 
    D3DXMatrixIdentity(pOut);
    if (r != l) pOut->m[0][0] = 2.0f / (r - l);
    if (t != b) pOut->m[1][1] = 2.0f / (t - b);
    if (zn != zf) {
        pOut->m[2][2] = 1.0f / (zn - zf);
        pOut->m[3][2] = zn / (zn - zf);
    }
    if (r != l) pOut->m[3][0] = (l + r) / (l - r);
    if (t != b) pOut->m[3][1] = (t + b) / (b - t);
    return pOut; 
}
inline HRESULT D3DXGetImageInfoFromFileInMemory(LPCVOID pSrcData, UINT SrcDataSize, D3DXIMAGE_INFO* pSrcInfo) {
    if (pSrcInfo) {
        pSrcInfo->Width = 256; pSrcInfo->Height = 256; pSrcInfo->Format = D3DFMT_A8R8G8B8;
    }
    return S_OK;
}
inline GLMATRIX* D3DXMatrixRotationAxis(GLMATRIX* pOut, const GLVECTOR3* pV, float angle) { GLVECTOR3 n; D3DXVec3Normalize(&n, pV); GLQUATERNION q; D3DXQuaternionRotationAxis(&q, &n, angle); return D3DXMatrixRotationQuaternion(pOut, &q); }
inline GLMATRIX* D3DXMatrixRotationYawPitchRoll(GLMATRIX* pOut, float y, float p, float r) {
    GLMATRIX my, mp, mr;
    D3DXMatrixRotationY(&my, y);
    D3DXMatrixRotationX(&mp, p);
    D3DXMatrixRotationZ(&mr, r);
    D3DXMatrixMultiply(pOut, &mr, &mp);
    D3DXMatrixMultiply(pOut, pOut, &my);
    return pOut;
}
inline GLVECTOR3* D3DXVec3Project(GLVECTOR3* pOut, const GLVECTOR3* pV, const D3DVIEWPORT8* pViewport, const GLMATRIX* pProjection, const GLMATRIX* pView, const GLMATRIX* pWorld) {
    GLMATRIX mat;
    D3DXMatrixIdentity(&mat);
    if (pWorld) D3DXMatrixMultiply(&mat, &mat, pWorld);
    if (pView) D3DXMatrixMultiply(&mat, &mat, pView);
    if (pProjection) D3DXMatrixMultiply(&mat, &mat, pProjection);
    GLVECTOR3 c;
    D3DXVec3TransformCoord(&c, pV, &mat);
    pOut->x = pViewport->X + (1.0f + c.x) * pViewport->Width * 0.5f;
    pOut->y = pViewport->Y + (1.0f - c.y) * pViewport->Height * 0.5f;
    pOut->z = pViewport->MinZ + c.z * (pViewport->MaxZ - pViewport->MinZ);
    return pOut;
}
inline GLVECTOR3* D3DXVec3Unproject(GLVECTOR3* pOut, const GLVECTOR3* pV, const D3DVIEWPORT8* pViewport, const GLMATRIX* pProjection, const GLMATRIX* pView, const GLMATRIX* pWorld) {
    GLMATRIX mat, inv;
    D3DXMatrixIdentity(&mat);
    if (pWorld) D3DXMatrixMultiply(&mat, &mat, pWorld);
    if (pView) D3DXMatrixMultiply(&mat, &mat, pView);
    if (pProjection) D3DXMatrixMultiply(&mat, &mat, pProjection);
    if (!D3DXMatrixInverse(&inv, NULL, &mat)) return NULL;
    float fDepth = pViewport->MaxZ - pViewport->MinZ;
    GLVECTOR3 c((pV->x - pViewport->X) * 2.0f / pViewport->Width - 1.0f,
                1.0f - (pV->y - pViewport->Y) * 2.0f / pViewport->Height,
                fDepth != 0.0f ? (pV->z - pViewport->MinZ) / fDepth : pV->z);
    D3DXVec3TransformCoord(pOut, &c, &inv);
    return pOut;
}
inline float D3DXVec2CCW(const GLVECTOR2* pV1, const GLVECTOR2* pV2) { return pV1->x*pV2->y - pV1->y*pV2->x; }
inline GLCOLOR* D3DXColorModulate(GLCOLOR* pOut, const GLCOLOR* pC1, const GLCOLOR* pC2) {
    pOut->r = pC1->r * pC2->r; pOut->g = pC1->g * pC2->g; pOut->b = pC1->b * pC2->b; pOut->a = pC1->a * pC2->a;
    return pOut;
}

    #define D3DMATRIX GLMATRIX
    #define D3DXMATRIX GLMATRIX
    #define D3DXVECTOR3 GLVECTOR3
    #define D3DVECTOR GLVECTOR3
    #define D3DXVECTOR2 GLVECTOR2
    #define D3DCOLOR GLCOLOR
    #define D3DXCOLOR GLCOLOR
    
    struct GL_LIGHT {
        int Type;
        GLCOLOR Diffuse;
        GLCOLOR Specular;
        GLCOLOR Ambient;
        GLVECTOR3 Position;
        GLVECTOR3 Direction;
        float Range;
        float Falloff;
        float Attenuation0;
        float Attenuation1;
        float Attenuation2;
        float Theta;
        float Phi;
    };
    #define D3DLIGHT8 GL_LIGHT
    
    struct GL_MATERIAL {
        GLCOLOR Diffuse;
        GLCOLOR Ambient;
        GLCOLOR Specular;
        GLCOLOR Emissive;
        float Power;
    };
    #define D3DMATERIAL8 GL_MATERIAL
    
    #define D3DXQUATERNION GLQUATERNION
    #define D3DXPLANE GLPLANE
    #define D3DXVECTOR4 GLVECTOR4

    struct IDirect3D8;
    struct IDirect3DDevice8;
    struct IDirect3DTexture8;
    struct IDirect3DSurface8;
    struct IDirect3DBaseTexture8;
    struct IDirect3DVertexBuffer8;
    struct IDirect3DIndexBuffer8;
    struct ID3DXBuffer;
    struct ID3DXMesh;

    typedef IDirect3D8* LPDIRECT3D8;
    typedef IDirect3DDevice8* LPDIRECT3DDEVICE8;
    typedef IDirect3DTexture8* LPDIRECT3DTEXTURE8;
    typedef IDirect3DBaseTexture8* LPDIRECT3DBASETEXTURE8;
    typedef IDirect3DBaseTexture8* LPDIRECT3DBaseTexture8;
    typedef IDirect3DSurface8* LPDIRECT3DSURFACE8;
    typedef IDirect3DVertexBuffer8* LPDIRECT3DVERTEXBUFFER8; // VBO
    typedef IDirect3DIndexBuffer8* LPDIRECT3DINDEXBUFFER8; // IBO/EBO
    typedef ID3DXMesh* LPD3DXMESH;
    typedef ID3DXBuffer* LPD3DXBUFFER;
    struct ID3DXMatrixStack;
    typedef ID3DXMatrixStack* LPD3DXMATRIXSTACK;

    #define D3DCOLOR_XRGB(r,g,b) GLCOLOR{ (r)/255.f, (g)/255.f, (b)/255.f, 1.0f }
    #define D3DCOLOR_ARGB(a,r,g,b) GLCOLOR{ (r)/255.f, (g)/255.f, (b)/255.f, (a)/255.f }

    #define D3DVSD_STREAM(s) (s)
    #define D3DVSD_REG(r, t) (r)
    #define D3DVSD_END() 0
    #define D3DVSDT_FLOAT3 0
    #define D3DVSDT_FLOAT2 0
    #define D3DVSDE_TEXCOORD1 0
    #define D3DVSDE_POSITION2 0
    #define D3DVSDE_NORMAL2 0


    struct IDirect3DBaseTexture8 {
        DWORD GetLevelCount() { return 1; }
        ULONG Release() { return 0; }
    };

    struct IDirect3DTexture8 : public IDirect3DBaseTexture8 {
        GLuint glId;
        int width, height;
        D3DFORMAT format;
        void* pLockedData;
        void* pScratch;
        ULONG refCount;
        IDirect3DTexture8() : glId(0), width(0), height(0), format(0), pLockedData(NULL), pScratch(NULL), refCount(1) {}
        ULONG AddRef() { return ++refCount; }
        HRESULT LockRect(UINT Level,D3DLOCKED_RECT* pLockedRect,const RECT* pRect,DWORD Flags);
        HRESULT UnlockRect(UINT Level);
        ULONG Release();
        HRESULT GetSurfaceLevel(UINT Level, LPDIRECT3DSURFACE8* ppSurface);
        DWORD GetLevelCount() { return 1; }
        HRESULT GetLevelDesc(UINT, D3DSURFACE_DESC* pDesc) { if (pDesc) memset(pDesc, 0, sizeof(*pDesc)); return S_OK; }
    };

    struct IDirect3DSurface8 {
        IDirect3DTexture8* pTexture;
        GLuint glRbo;
        UINT width, height;
        D3DFORMAT format;
        ULONG refCount;
        bool bBackBuffer;
        IDirect3DSurface8() : pTexture(NULL), glRbo(0), width(0), height(0), format(0), refCount(1), bBackBuffer(false) {}
        ULONG AddRef() { return bBackBuffer ? 1 : ++refCount; }
        ULONG Release();
        HRESULT LockRect(D3DLOCKED_RECT*, const RECT*, DWORD) { return S_OK; }
        HRESULT UnlockRect() { return S_OK; }
        HRESULT GetDesc(D3DSURFACE_DESC* pDesc) {
            if (pDesc) {
                memset(pDesc, 0, sizeof(*pDesc));
                pDesc->Format = format;
                pDesc->Width = width;
                pDesc->Height = height;
            }
            return S_OK;
        }
    };

    struct IDirect3DVertexBuffer8 {
        GLuint glVbo;
        UINT length;
        void* pData;
        IDirect3DVertexBuffer8(UINT len) : glVbo(0), length(len), pData(NULL) {
            glGenBuffers(1, &glVbo);
            pData = malloc(len);
        }
        ULONG AddRef() { return 1; }
        HRESULT Lock(UINT OffsetToLock, UINT SizeToLock, BYTE** ppbData, DWORD Flags) { 
            *ppbData = (BYTE*)pData + OffsetToLock;
            return S_OK; 
        }
        HRESULT Unlock() { 
            glBindBuffer(GL_ARRAY_BUFFER, glVbo);
            glBufferData(GL_ARRAY_BUFFER, length, pData, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            return S_OK; 
        }
        ULONG Release() { 
            if (glVbo) glDeleteBuffers(1, &glVbo);
            if (pData) free(pData);
            delete this;
            return 0; 
        }
    };
    struct IDirect3DIndexBuffer8 {
        GLuint glIbo;
        UINT length;
        void* pData;
        UINT indexSize;
        IDirect3DIndexBuffer8(UINT len) : glIbo(0), length(len), pData(NULL), indexSize(2) {
            glGenBuffers(1, &glIbo);
            pData = malloc(len);
        }
        ULONG AddRef() { return 1; }
        HRESULT Lock(UINT OffsetToLock, UINT SizeToLock, BYTE** ppbData, DWORD Flags) { 
            *ppbData = (BYTE*)pData + OffsetToLock;
            return S_OK; 
        }
        HRESULT Unlock() { 
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIbo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, length, pData, GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            return S_OK; 
        }
        ULONG Release() { 
            if (glIbo) glDeleteBuffers(1, &glIbo);
            if (pData) free(pData);
            delete this;
            return 0; 
        }
    };
    struct ID3DXMatrixStack {
        std::vector<GLMATRIX> m_stack;
        ID3DXMatrixStack() { m_stack.push_back(GLMATRIX()); }
        
        HRESULT Push() { m_stack.push_back(m_stack.back()); return S_OK; }
        HRESULT Pop() { if (m_stack.size() > 1) m_stack.pop_back(); return S_OK; }
        HRESULT LoadIdentity() { D3DXMatrixIdentity(&m_stack.back()); return S_OK; }
        HRESULT LoadMatrix(const GLMATRIX* pMat) { m_stack.back() = *pMat; return S_OK; }
        HRESULT MultMatrix(const GLMATRIX* pMat) { D3DXMatrixMultiply(&m_stack.back(), &m_stack.back(), pMat); return S_OK; }
        HRESULT MultMatrixLocal(const GLMATRIX* pMat) { D3DXMatrixMultiply(&m_stack.back(), pMat, &m_stack.back()); return S_OK; }
        HRESULT Scale(float x, float y, float z) { 
            GLMATRIX s; D3DXMatrixScaling(&s, x, y, z);
            MultMatrix(&s); return S_OK; 
        }
        HRESULT Translate(float x, float y, float z) {
            GLMATRIX t; D3DXMatrixTranslation(&t, x, y, z);
            MultMatrix(&t); return S_OK;
        }
        HRESULT RotateAxis(const GLVECTOR3* pV, float angle) {
            GLMATRIX m; D3DXMatrixRotationAxis(&m, pV, angle);
            MultMatrix(&m); return S_OK;
        }
        HRESULT RotateAxisLocal(const GLVECTOR3* pV, float angle) {
            GLMATRIX m; D3DXMatrixRotationAxis(&m, pV, angle);
            MultMatrixLocal(&m); return S_OK;
        }
        HRESULT RotateYawPitchRollLocal(float y, float p, float r) {
            GLMATRIX m; D3DXMatrixRotationYawPitchRoll(&m, y, p, r);
            MultMatrixLocal(&m); return S_OK;
        }
        GLMATRIX* GetTop() { return &m_stack.back(); }
        ULONG Release() { delete this; return 0; }
    };

    struct ID3DXBuffer {
        void* GetBufferPointer() { return 0; }
        DWORD GetBufferSize() { return 0; }
        ULONG Release() { delete this; return 0; }
    };
    struct ID3DXMesh {
        HRESULT GetIndexBuffer(LPDIRECT3DINDEXBUFFER8*) { return S_OK; }
        HRESULT GetVertexBuffer(LPDIRECT3DVERTEXBUFFER8*) { return S_OK; }
        DWORD GetFVF() { return 0; }
        DWORD GetNumVertices() { return 0; }
        DWORD GetNumFaces() { return 0; }
        ULONG Release() { return 0; }
    };

    struct IDirect3D8 {
        ULONG AddRef() { return 1; }
        UINT GetAdapterCount() { return 1; }
        HRESULT GetAdapterIdentifier(UINT, DWORD, D3DADAPTER_IDENTIFIER8* pIdentifier) {
            if (pIdentifier) {
                strcpy(pIdentifier->Driver, "OpenGL Generic");
                strcpy(pIdentifier->Description, "OpenGL Port");
            }
            return S_OK;
        }
        UINT GetAdapterModeCount(UINT) { return 5; }
        HRESULT EnumAdapterModes(UINT, UINT iMode, D3DDISPLAYMODE* pMode) {
            static const UINT s_aauModes[5][2] = { { 800, 600 }, { 1024, 768 }, { 1280, 720 }, { 1600, 900 }, { 1920, 1080 } };
            if (iMode >= 5)
                return E_FAIL;
            if (pMode) { pMode->Width = s_aauModes[iMode][0]; pMode->Height = s_aauModes[iMode][1]; pMode->RefreshRate = 60; pMode->Format = D3DFMT_X8R8G8B8; }
            return S_OK;
        }
        HRESULT GetAdapterDisplayMode(UINT, D3DDISPLAYMODE* pMode) {
            if (pMode) { pMode->Width = 1024; pMode->Height = 768; pMode->RefreshRate = 60; pMode->Format = D3DFMT_X8R8G8B8; }
            return S_OK;
        }
        HRESULT CheckDeviceType(UINT, D3DDEVTYPE, D3DFORMAT, D3DFORMAT, BOOL) { return S_OK; }
        HRESULT CheckDeviceFormat(UINT, int, int, DWORD, int, int) { return S_OK; }
        HRESULT CheckDepthStencilMatch(UINT, int, int, int, int) { return S_OK; }
        HRESULT GetDeviceCaps(UINT, int, D3DCAPS8* pCaps) {
            if (pCaps) {
                memset(pCaps, 0, sizeof(D3DCAPS8));
                pCaps->MaxTextureWidth = 8192;
                pCaps->MaxTextureHeight = 8192;
                pCaps->DevCaps = D3DDEVCAPS_HWTRANSFORMANDLIGHT;
                pCaps->Caps2 = D3DCAPS2_CANRENDERWINDOWED;
                pCaps->VertexProcessingCaps = D3DVTXPCAPS_DIRECTIONALLIGHTS | D3DVTXPCAPS_POSITIONALLIGHTS | D3DVTXPCAPS_TEXGEN;
                pCaps->PrimitiveMiscCaps = D3DPMISCCAPS_CLIPTLVERTS;
                pCaps->TextureAddressCaps = D3DPTADDRESSCAPS_BORDER;
                pCaps->MaxSimultaneousTextures = 2;
                pCaps->VertexShaderVersion = D3DVS_VERSION(1, 1);
            }
            return S_OK;
        }
        HRESULT CreateDevice(UINT Adapter, int DeviceType, HWND hWnd, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS*, IDirect3DDevice8** ppDevice);
        ULONG Release() { delete this; return 0; }
    };

    struct IDirect3DDevice8 {
        ULONG AddRef() { return 1; }
        ULONG Release() { return 0; }
        IDirect3DDevice8() {}
        HRESULT CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, LPDIRECT3DTEXTURE8* ppTexture);
        HRESULT SetLight(DWORD index, const D3DLIGHT8* pLight);
        HRESULT LightEnable(DWORD index, BOOL bEnable);
        DWORD m_dwFVF = 0;
        HRESULT GetVertexShader(DWORD* pShader) { *pShader = m_dwFVF; return S_OK; }
        HRESULT SetVertexShader(DWORD Shader) { m_dwFVF = Shader; return S_OK; }
        HRESULT CreateVertexShader(const DWORD* pDeclaration, const DWORD* pFunction, DWORD* pHandle, DWORD Usage) { *pHandle = 1; return S_OK; }
        HRESULT DeleteVertexShader(DWORD Handle) { return S_OK; }
        HRESULT CreatePixelShader(const DWORD* pFunction, DWORD* pHandle) { *pHandle = 2; return S_OK; }
        HRESULT DeletePixelShader(DWORD Handle) { return S_OK; }
        HRESULT SetPixelShader(DWORD Handle) { return S_OK; }
        HRESULT SetVertexShaderConstant(DWORD Register, const void* pData, DWORD Count) { return S_OK; }
        HRESULT SetPixelShaderConstant(DWORD Register, const void* pData, DWORD Count) { return S_OK; }
        HRESULT SetMaterial(const D3DMATERIAL8* mat) { if (mat) m_material = *mat; return S_OK; }
        HRESULT Clear(DWORD n, const void* pRects, DWORD flags, D3DCOLOR color, float z, DWORD stencil); 
#if 0
            if (flags & 2) { // D3DCLEAR_ZBUFFER
                #ifndef ANDROID
                    glClearDepth(z);
                #else
                    glClearDepthf(z);
                #endif
                mask |= GL_DEPTH_BUFFER_BIT;
            }
#endif
        HRESULT TestCooperativeLevel() { return S_OK; }
        HRESULT Reset(D3DPRESENT_PARAMETERS*);
        HRESULT Present(const RECT*,const RECT*,HWND,const void*);
        HRESULT CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE, LPDIRECT3DSURFACE8* ppSurface);
        HRESULT GetRenderTarget(LPDIRECT3DSURFACE8* ppSurface);
        HRESULT GetDepthStencilSurface(LPDIRECT3DSURFACE8* ppSurface);
        HRESULT GetViewport(D3DVIEWPORT8* v) { if (v) *v = m_viewport; return S_OK; }
        HRESULT SetRenderTarget(LPDIRECT3DSURFACE8 pRenderTarget, LPDIRECT3DSURFACE8 pDepthStencil);
        HRESULT SetViewport(const D3DVIEWPORT8* v);
        HRESULT BeginScene();
        HRESULT EndScene();
        HRESULT GetDeviceCaps(D3DCAPS8* pCaps) { IDirect3D8 d3d; return d3d.GetDeviceCaps(0, 0, pCaps); }
        void SetGammaRamp(DWORD,const D3DGAMMARAMP*) {}
        HRESULT GetBackBuffer(UINT, D3DBACKBUFFER_TYPE, LPDIRECT3DSURFACE8* ppSurface);
        UINT GetAvailableTextureMem() { return 128 * 1024 * 1024; }
        LPDIRECT3DVERTEXBUFFER8 m_pStreamSource = NULL;
        UINT m_StreamStride = 0;
        HRESULT SetStreamSource(UINT StreamNumber, LPDIRECT3DVERTEXBUFFER8 pStreamData, UINT Stride);
        HRESULT CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, LPDIRECT3DVERTEXBUFFER8* ppVertexBuffer) { 
            *ppVertexBuffer = new IDirect3DVertexBuffer8(Length);
            return S_OK; 
        }
        HRESULT CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, LPDIRECT3DINDEXBUFFER8* ppIndexBuffer) { 
            *ppIndexBuffer = new IDirect3DIndexBuffer8(Length);
            (*ppIndexBuffer)->indexSize = (Format == D3DFMT_INDEX16 || Format == 0) ? 2 : 4;
            return S_OK; 
        }
        LPDIRECT3DINDEXBUFFER8 m_pIndexBuffer = NULL;
        UINT m_BaseVertexIndex = 0;
        GLMATRIX m_matWorld, m_matView, m_matProj;
        HRESULT SetIndices(LPDIRECT3DINDEXBUFFER8 pIndexBuffer, UINT BaseVertexIndex);
        HRESULT DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT minIndex, UINT NumVertices, UINT startIndex, UINT primCount);
        HRESULT DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertexIndices, UINT PrimitiveCount, const void* pIndexData, D3DFORMAT IndexDataFormat, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride);
        HRESULT DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
        HRESULT DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride);
        HRESULT SetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE type, DWORD value);
        HRESULT SetRenderState(DWORD Type, DWORD Value);
        HRESULT SetTexture(DWORD stage, LPDIRECT3DBaseTexture8 tex);
        HRESULT SetTransform(DWORD type, const GLMATRIX* mat);
        HRESULT GetTransform(DWORD type, GLMATRIX* mat);

        enum { MAX_STAGES = 2 };
        IDirect3DTexture8* m_apTexture[MAX_STAGES] = {};
        DWORD m_adwColorOp[MAX_STAGES] = { D3DTOP_MODULATE, D3DTOP_DISABLE };
        DWORD m_adwColorArg1[MAX_STAGES] = { D3DTA_TEXTURE, D3DTA_TEXTURE };
        DWORD m_adwColorArg2[MAX_STAGES] = { D3DTA_CURRENT, D3DTA_CURRENT };
        DWORD m_adwAlphaOp[MAX_STAGES] = { D3DTOP_SELECTARG1, D3DTOP_DISABLE };
        DWORD m_adwAlphaArg1[MAX_STAGES] = { D3DTA_TEXTURE, D3DTA_TEXTURE };
        DWORD m_adwAlphaArg2[MAX_STAGES] = { D3DTA_CURRENT, D3DTA_CURRENT };
        DWORD m_adwAddressU[MAX_STAGES] = { D3DTADDRESS_WRAP, D3DTADDRESS_WRAP };
        DWORD m_adwAddressV[MAX_STAGES] = { D3DTADDRESS_WRAP, D3DTADDRESS_WRAP };
        DWORD m_adwMagFilter[MAX_STAGES] = { D3DTEXF_LINEAR, D3DTEXF_LINEAR };
        DWORD m_adwMinFilter[MAX_STAGES] = { D3DTEXF_LINEAR, D3DTEXF_LINEAR };
        DWORD m_adwTexCoordIndex[MAX_STAGES] = { 0, 1 };
        DWORD m_adwTextureTransformFlags[MAX_STAGES] = { D3DTTFF_DISABLE, D3DTTFF_DISABLE };
        GLMATRIX m_amatTexture[MAX_STAGES];
        enum { MAX_LIGHTS = 2 };
        D3DLIGHT8 m_aLight[MAX_LIGHTS] = {};
        BOOL m_abLightEnable[MAX_LIGHTS] = {};
        D3DMATERIAL8 m_material = {};
        BOOL m_bLighting = TRUE;
        BOOL m_bColorVertex = TRUE;
        DWORD m_dwDiffuseMaterialSource = D3DMCS_COLOR1;
        DWORD m_dwAmbientMaterialSource = D3DMCS_MATERIAL;
        DWORD m_dwEmissiveMaterialSource = D3DMCS_MATERIAL;
        DWORD m_dwAmbient = 0;
        BOOL m_bFogEnable = FALSE;
        DWORD m_dwFogColor = 0;
        DWORD m_dwFogTableMode = D3DFOG_NONE;
        DWORD m_dwFogVertexMode = D3DFOG_NONE;
        float m_fFogStart = 0.0f;
        float m_fFogEnd = 1.0f;
        float m_fFogDensity = 1.0f;
        DWORD m_dwTextureFactor = 0xffffffff;
        DWORD m_dwSrcBlend = D3DBLEND_ONE;
        DWORD m_dwDestBlend = D3DBLEND_ZERO;
        BOOL m_bAlphaTest = FALSE;
        DWORD m_dwAlphaRef = 0;
        DWORD m_dwAlphaFunc = D3DCMP_ALWAYS;
        float m_fViewportWidth = 1.0f;
        float m_fViewportHeight = 1.0f;
        D3DVIEWPORT8 m_viewport = { 0, 0, 1, 1, 0.0f, 1.0f };
        int m_iSurfaceHeight = 1;
        int m_iWindowHeight = 1;
        GLuint m_glFbo = 0;
        IDirect3DSurface8* m_pRenderTarget = NULL;
        IDirect3DSurface8* m_pDepthStencil = NULL;
        void ApplyDrawState(const BYTE* pVertexBase, UINT uStride);
    };


    IDirect3D8* Direct3DCreate8(UINT SDKVersion);

    inline D3DXPLANE* D3DXPlaneNormalize(D3DXPLANE* pOut, const D3DXPLANE* pP) {
        float len = sqrtf(pP->a*pP->a + pP->b*pP->b + pP->c*pP->c);
        if (len > 0.0f) { pOut->a=pP->a/len; pOut->b=pP->b/len; pOut->c=pP->c/len; pOut->d=pP->d/len; }
        else { pOut->a=pOut->b=pOut->c=pOut->d=0.0f; }
        return pOut;
    }
    inline GLVECTOR3* D3DXVec3Lerp(GLVECTOR3* pOut, const GLVECTOR3* pV1, const GLVECTOR3* pV2, float s) {
        pOut->x = pV1->x + s*(pV2->x-pV1->x);
        pOut->y = pV1->y + s*(pV2->y-pV1->y);
        pOut->z = pV1->z + s*(pV2->z-pV1->z);
        return pOut;
    }
    inline GLVECTOR4* D3DXVec4Transform(GLVECTOR4* pOut, const GLVECTOR4* pV, const GLMATRIX* pM) {
        pOut->x = pV->x*pM->m[0][0]+pV->y*pM->m[1][0]+pV->z*pM->m[2][0]+pV->w*pM->m[3][0];
        pOut->y = pV->x*pM->m[0][1]+pV->y*pM->m[1][1]+pV->z*pM->m[2][1]+pV->w*pM->m[3][1];
        pOut->z = pV->x*pM->m[0][2]+pV->y*pM->m[1][2]+pV->z*pM->m[2][2]+pV->w*pM->m[3][2];
        pOut->w = pV->x*pM->m[0][3]+pV->y*pM->m[1][3]+pV->z*pM->m[2][3]+pV->w*pM->m[3][3];
        return pOut;
    }
    inline UINT D3DXGetFVFVertexSize(DWORD fvf) {
        UINT size = 0;
        if (fvf & D3DFVF_XYZRHW) size += 16;
        else if (fvf & D3DFVF_XYZ) size += 12;
        if (fvf & D3DFVF_NORMAL) size += 12;
        if (fvf & D3DFVF_DIFFUSE) size += 4;
        if (fvf & D3DFVF_SPECULAR) size += 4;
        int texCount = (fvf & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
        for (int i = 0; i < texCount; ++i) {
            static const UINT s_auSize[4] = { 8, 12, 16, 4 };
            size += s_auSize[(fvf >> (i * 2 + 16)) & 3];
        }
        return size;
    }

    inline HRESULT D3DXCreateTexture(LPDIRECT3DDEVICE8 pDevice, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, LPDIRECT3DTEXTURE8* ppTexture) {
        if (!pDevice) return S_OK;
        return pDevice->CreateTexture(Width, Height, MipLevels, Usage, Format, Pool, ppTexture);
    }

    inline HRESULT D3DXCreateTextureFromFileInMemoryEx(LPDIRECT3DDEVICE8 pDevice, LPCVOID pSrcData, UINT SrcDataSize, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, LPDIRECT3DTEXTURE8* ppTexture) {
        if (pSrcInfo) {
            pSrcInfo->Width = (Width == D3DX_DEFAULT) ? 256 : Width;
            pSrcInfo->Height = (Height == D3DX_DEFAULT) ? 256 : Height;
            pSrcInfo->MipLevels = 1;
            pSrcInfo->Format = D3DFMT_A8R8G8B8;
        }
        if (pDevice) {
            return pDevice->CreateTexture(256, 256, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, ppTexture);
        }
        return S_OK;
    }

    inline HRESULT D3DXLoadSurfaceFromSurface(LPDIRECT3DSURFACE8 pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPDIRECT3DSURFACE8 pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey) {
        return S_OK;
    }

    // Stub functions
    inline HRESULT D3DXCreateMatrixStack(DWORD flags, LPD3DXMATRIXSTACK* ppStack) { if (ppStack) *ppStack = new ID3DXMatrixStack(); return S_OK; }
    inline HRESULT D3DXCreateSphere(LPDIRECT3DDEVICE8 pDevice, float radius, UINT slices, UINT stacks, LPD3DXMESH* ppMesh, LPD3DXBUFFER* ppAdjacency) { if (ppMesh) *ppMesh = new ID3DXMesh(); return S_OK; }
    inline HRESULT D3DXCreateCylinder(LPDIRECT3DDEVICE8 pDevice, float radius1, float radius2, float length, UINT slices, UINT stacks, LPD3DXMESH* ppMesh, LPD3DXBUFFER* ppAdjacency) { if (ppMesh) *ppMesh = new ID3DXMesh(); return S_OK; }
    inline HRESULT D3DXAssembleShaderFromFile(LPCSTR pSrcFile, DWORD flags, LPD3DXBUFFER* ppConstants, LPD3DXBUFFER* ppCompiledShader, LPD3DXBUFFER* ppCompilationErrors) { 
        if (ppCompiledShader) *ppCompiledShader = new ID3DXBuffer();
        if (ppCompilationErrors) *ppCompilationErrors = new ID3DXBuffer();
        return S_OK; 
    }

    inline HRESULT D3DXAssembleShader(LPCSTR pSrcData, UINT SrcDataLen, DWORD flags, LPD3DXBUFFER* ppConstants, LPD3DXBUFFER* ppCompiledShader, LPD3DXBUFFER* ppCompilationErrors) { 
        if (ppCompiledShader) *ppCompiledShader = new ID3DXBuffer();
        if (ppCompilationErrors) *ppCompilationErrors = new ID3DXBuffer();
        return S_OK; 
    }
