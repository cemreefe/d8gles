#include <d8gles/d8gles.h>

#include <cstdarg>
#include <cstdio>
#include <set>

static D8GLES_Platform g_platform = { 0, 0, 0 };
static unsigned g_uDbgDraws = 0;
static int g_iDbgSolid = 0;
static bool g_bDbgDump = false;

static void D8Log(int level, const char* fmt, ...)
{
    char msg[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);
    if (g_platform.log)
        g_platform.log(level, msg, g_platform.user);
    else if (level >= D8GLES_LOG_ERROR)
        fprintf(stderr, "d8gles: %s\n", msg);
}
#define LOGI(...) D8Log(D8GLES_LOG_INFO, __VA_ARGS__)
#define LOGE(...) D8Log(D8GLES_LOG_ERROR, __VA_ARGS__)

extern "C" {
void D8GLES_SetPlatform(const D8GLES_Platform* platform)
{
    if (platform)
        g_platform = *platform;
    else
        g_platform = D8GLES_Platform{ 0, 0, 0 };
}
void D8GLES_SetDebug(int solidMode, int dumpThisFrame)
{
    g_iDbgSolid = solidMode;
    g_bDbgDump = dumpThisFrame != 0;
}
unsigned D8GLES_GetDrawCount(void) { return g_uDbgDraws; }
}

namespace {
    // D3D8 fixed-function emulation: two texture stages, texgen/texture transforms, lighting, fog, alpha test, RHW vertices.
    // D3DCOLOR and A8R8G8B8 texels are stored little-endian as B,G,R,A, so both are swizzled with .bgra.
    const char* vShaderSource =
        "#version 300 es\n"
        "layout(location = 0) in vec4 aPos;\n"
        "layout(location = 1) in vec2 aTex0;\n"
        "layout(location = 2) in vec4 aColor;\n"
        "layout(location = 3) in vec2 aTex1;\n"
        "layout(location = 4) in vec3 aNormal;\n"
        "uniform mat4 uWV;\n"
        "uniform mat4 uProj;\n"
        "uniform bool uRHW;\n"
        "uniform vec2 uViewport;\n"
        "uniform float uFlipY;\n"
        "uniform mat4 uTexMat0;\n"
        "uniform mat4 uTexMat1;\n"
        "uniform ivec2 uTexGen;\n"
        "uniform ivec2 uTTFF;\n"
        "uniform bool uLighting;\n"
        "uniform bool uHasNormal;\n"
        "uniform bool uHasColor;\n"
        "uniform ivec3 uMatSrc;\n"
        "uniform vec4 uMatDiffuse;\n"
        "uniform vec4 uMatAmbient;\n"
        "uniform vec4 uMatEmissive;\n"
        "uniform vec4 uAmbient;\n"
        "uniform vec4 uLightDir[2];\n"
        "uniform vec4 uLightDiffuse[2];\n"
        "uniform vec4 uLightAmbient[2];\n"
        "out vec4 vTex0;\n"
        "out vec4 vTex1;\n"
        "out vec4 vColor;\n"
        "out float vFogZ;\n"
        "vec4 GenTex(int tci, int ttff, mat4 m, vec3 vpos, vec3 vn) {\n"
        "  int src = tci & 0xF0000;\n"
        "  vec4 t;\n"
        "  if (src == 0x20000) t = vec4(vpos, 1.0);\n"
        "  else if (src == 0x10000) t = vec4(vn, 1.0);\n"
        "  else if (src == 0x30000) t = vec4(reflect(normalize(vpos), vn), 1.0);\n"
        "  else t = vec4((tci & 0xFFFF) == 1 ? aTex1 : aTex0, 1.0, 0.0);\n"
        "  int cnt = ttff & 255;\n"
        "  float w = 1.0;\n"
        "  if (cnt != 0) {\n"
        "    t = m * t;\n"
        "    if ((ttff & 256) != 0) w = cnt == 2 ? t.y : (cnt == 3 ? t.z : t.w);\n"
        "  }\n"
        "  return vec4(t.xy, 0.0, w);\n"
        "}\n"
        "void main() {\n"
        "  vec4 vc = uHasColor ? aColor.bgra : vec4(1.0);\n"
        "  vec3 vpos = vec3(0.0);\n"
        "  vec3 vn = vec3(0.0, 0.0, -1.0);\n"
        "  if (uRHW) {\n"
        "    gl_Position = vec4(aPos.x / uViewport.x * 2.0 - 1.0, (1.0 - aPos.y / uViewport.y * 2.0) * uFlipY, aPos.z * 2.0 - 1.0, 1.0);\n"
        "    vFogZ = 0.0;\n"
        "    vColor = vc;\n"
        "  } else {\n"
        "    vec4 ep = uWV * vec4(aPos.xyz, 1.0);\n"
        "    vpos = ep.xyz;\n"
        "    if (uHasNormal) vn = normalize(mat3(uWV) * aNormal);\n"
        "    vec4 p = uProj * ep;\n"
        "    p.z = p.z * 2.0 - p.w;\n"
        "    p.y *= uFlipY;\n"
        "    gl_Position = p;\n"
        "    vFogZ = ep.z;\n"
        "    if (uLighting) {\n"
        "      vec4 md = (uMatSrc.x == 1 && uHasColor) ? vc : uMatDiffuse;\n"
        "      vec4 ma = (uMatSrc.y == 1 && uHasColor) ? vc : uMatAmbient;\n"
        "      vec4 me = (uMatSrc.z == 1 && uHasColor) ? vc : uMatEmissive;\n"
        "      vec3 c = me.rgb + uAmbient.rgb * ma.rgb;\n"
        "      for (int i = 0; i < 2; ++i) {\n"
        "        if (uLightDir[i].w == 0.0) continue;\n"
        "        float d = uHasNormal ? max(dot(vn, -uLightDir[i].xyz), 0.0) : 0.0;\n"
        "        c += uLightAmbient[i].rgb * ma.rgb + d * uLightDiffuse[i].rgb * md.rgb;\n"
        "      }\n"
        "      vColor = vec4(clamp(c, 0.0, 1.0), md.a);\n"
        "    } else {\n"
        "      vColor = vc;\n"
        "    }\n"
        "  }\n"
        "  vTex0 = GenTex(uTexGen.x, uTTFF.x, uTexMat0, vpos, vn);\n"
        "  vTex1 = GenTex(uTexGen.y, uTTFF.y, uTexMat1, vpos, vn);\n"
        "}\n";

    const char* fShaderSource =
        "#version 300 es\n"
        "precision highp float;\n"
        "in vec4 vTex0;\n"
        "in vec4 vTex1;\n"
        "in vec4 vColor;\n"
        "in float vFogZ;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D uTexture0;\n"
        "uniform sampler2D uTexture1;\n"
        "uniform ivec2 uUseTexture;\n"
        "uniform vec4 uTFactor;\n"
        "uniform ivec3 uColorStage0;\n"
        "uniform ivec3 uAlphaStage0;\n"
        "uniform ivec3 uColorStage1;\n"
        "uniform ivec3 uAlphaStage1;\n"
        "uniform int uAlphaFunc;\n"
        "uniform float uAlphaRef;\n"
        "uniform int uDbg;\n"
        "uniform int uFogMode;\n"
        "uniform vec4 uFogColor;\n"
        "uniform vec3 uFogParams;\n"
        "vec4 Arg(int a, vec4 cur, vec4 tex) {\n"
        "  int sel = a & 15;\n"
        "  vec4 v = cur;\n"
        "  if (sel == 0) v = vColor;\n"
        "  else if (sel == 2) v = tex;\n"
        "  else if (sel == 3) v = uTFactor;\n"
        "  else if (sel == 4) v = vec4(0.0);\n"
        "  if ((a & 16) != 0) v = vec4(1.0) - v;\n"
        "  if ((a & 32) != 0) v = vec4(v.a);\n"
        "  return v;\n"
        "}\n"
        "vec4 Op(int op, vec4 a1, vec4 a2, vec4 cur, vec4 tex) {\n"
        "  if (op == 2) return a1;\n"
        "  if (op == 3) return a2;\n"
        "  if (op == 5) return clamp(a1 * a2 * 2.0, 0.0, 1.0);\n"
        "  if (op == 6) return clamp(a1 * a2 * 4.0, 0.0, 1.0);\n"
        "  if (op == 7) return clamp(a1 + a2, 0.0, 1.0);\n"
        "  if (op == 8) return clamp(a1 + a2 - 0.5, 0.0, 1.0);\n"
        "  if (op == 9) return clamp((a1 + a2 - 0.5) * 2.0, 0.0, 1.0);\n"
        "  if (op == 10) return clamp(a1 - a2, 0.0, 1.0);\n"
        "  if (op == 11) return clamp(a1 + a2 - a1 * a2, 0.0, 1.0);\n"
        "  if (op == 12) return mix(a2, a1, vColor.a);\n"
        "  if (op == 13) return mix(a2, a1, tex.a);\n"
        "  if (op == 14) return mix(a2, a1, uTFactor.a);\n"
        "  if (op == 15) return clamp(a1 + a2 * (1.0 - tex.a), 0.0, 1.0);\n"
        "  if (op == 16) return mix(a2, a1, cur.a);\n"
        "  if (op == 18) return clamp(vec4(a1.rgb + a1.a * a2.rgb, a1.a), 0.0, 1.0);\n"
        "  if (op == 19) return clamp(vec4(a1.rgb * a2.rgb + a1.a, a1.a), 0.0, 1.0);\n"
        "  if (op == 20) return clamp(vec4((1.0 - a1.a) * a2.rgb + a1.rgb, a1.a), 0.0, 1.0);\n"
        "  if (op == 21) return clamp(vec4((1.0 - a1.rgb) * a2.rgb + a1.a, a1.a), 0.0, 1.0);\n"
        "  if (op == 24) return vec4(clamp(dot(a1.rgb - 0.5, a2.rgb - 0.5) * 4.0, 0.0, 1.0));\n"
        "  return a1 * a2;\n"
        "}\n"
        "vec4 Stage(ivec3 cs, ivec3 as, vec4 cur, vec4 tex) {\n"
        "  vec4 c = Op(cs.x, Arg(cs.y, cur, tex), Arg(cs.z, cur, tex), cur, tex);\n"
        "  vec4 a = as.x == 1 ? cur : Op(as.x, Arg(as.y, cur, tex), Arg(as.z, cur, tex), cur, tex);\n"
        "  return vec4(c.rgb, a.a);\n"
        "}\n"
        "bool AlphaPass(float a) {\n"
        "  if (uAlphaFunc == 1) return false;\n"
        "  if (uAlphaFunc == 2) return a < uAlphaRef;\n"
        "  if (uAlphaFunc == 3) return abs(a - uAlphaRef) < 0.002;\n"
        "  if (uAlphaFunc == 4) return a <= uAlphaRef;\n"
        "  if (uAlphaFunc == 5) return a > uAlphaRef;\n"
        "  if (uAlphaFunc == 6) return abs(a - uAlphaRef) >= 0.002;\n"
        "  if (uAlphaFunc == 7) return a >= uAlphaRef;\n"
        "  return true;\n"
        "}\n"
        "void main() {\n"
        "  vec4 cur = vColor;\n"
        "  if (uColorStage0.x != 1) {\n"
        "    vec4 t0 = uUseTexture.x != 0 ? texture(uTexture0, vTex0.xy / vTex0.w).bgra : vec4(1.0);\n"
        "    cur = Stage(uColorStage0, uAlphaStage0, cur, t0);\n"
        "    if (uColorStage1.x != 1) {\n"
        "      vec4 t1 = uUseTexture.y != 0 ? texture(uTexture1, vTex1.xy / vTex1.w).bgra : vec4(1.0);\n"
        "      cur = Stage(uColorStage1, uAlphaStage1, cur, t1);\n"
        "    }\n"
        "  }\n"
        "  if (!AlphaPass(cur.a)) discard;\n"
        "  if (uFogMode != 0) {\n"
        "    float z = abs(vFogZ);\n"
        "    float f = 1.0;\n"
        "    if (uFogMode == 3) f = (uFogParams.y - z) / max(uFogParams.y - uFogParams.x, 0.0001);\n"
        "    else if (uFogMode == 1) f = exp(-uFogParams.z * z);\n"
        "    else if (uFogMode == 2) f = exp(-(uFogParams.z * z) * (uFogParams.z * z));\n"
        "    cur.rgb = mix(uFogColor.rgb, cur.rgb, clamp(f, 0.0, 1.0));\n"
        "  }\n"
        "  FragColor = cur;\n"
        "  if (uDbg == 8) FragColor = vec4(vColor.rgb, 1.0);\n"
        "  if (uDbg == 9) FragColor = vec4(texture(uTexture0, vTex0.xy / vTex0.w).bgr, 1.0);\n"
        "  if (uDbg == 4) FragColor = vec4(clamp(vTex1.xy / vTex1.w, 0.0, 1.0), (vTex1.x < 0.0 || vTex1.x > 1.0 || vTex1.y < 0.0 || vTex1.y > 1.0) ? 1.0 : 0.0, 1.0);\n"
        "  if (uDbg == 5) FragColor = vec4(texture(uTexture1, vTex1.xy / vTex1.w).aaa, 1.0);\n"
        "  if (uDbg == 6) FragColor = vec4(textureLod(uTexture1, vec2(0.5), 0.0).aaa, 1.0);\n"
        "}\n";

    GLuint CompileShader(GLenum type, const char* source) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[1024];
            glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
            LOGE("Shader Compilation Failed: %s", infoLog);
        }
        return shader;
    }

    GLuint program = 0;
    struct SUniforms {
        GLint WV, Proj, RHW, Viewport, FlipY, TexMat0, TexMat1, TexGen, TTFF;
        GLint Lighting, HasNormal, HasColor, MatSrc, MatDiffuse, MatAmbient, MatEmissive, Ambient, LightDir, LightDiffuse, LightAmbient;
        GLint Texture0, Texture1, UseTexture, TFactor, ColorStage0, AlphaStage0, ColorStage1, AlphaStage1, AlphaFunc, AlphaRef, Dbg;
        GLint FogMode, FogColor, FogParams;
    } u;

    float DwordToFloat(DWORD v) {
        float f;
        memcpy(&f, &v, sizeof(f));
        return f;
    }

    void SetColorUniform(GLint loc, const GLCOLOR& c) {
        glUniform4f(loc, c.r, c.g, c.b, c.a);
    }

    GLenum ToGLBlend(DWORD v) {
        switch (v) {
            case D3DBLEND_ZERO: return GL_ZERO;
            case D3DBLEND_ONE: return GL_ONE;
            case D3DBLEND_SRCCOLOR: return GL_SRC_COLOR;
            case D3DBLEND_INVSRCCOLOR: return GL_ONE_MINUS_SRC_COLOR;
            case D3DBLEND_SRCALPHA: return GL_SRC_ALPHA;
            case D3DBLEND_INVSRCALPHA: return GL_ONE_MINUS_SRC_ALPHA;
            case D3DBLEND_DESTALPHA: return GL_DST_ALPHA;
            case D3DBLEND_INVDESTALPHA: return GL_ONE_MINUS_DST_ALPHA;
            case D3DBLEND_DESTCOLOR: return GL_DST_COLOR;
            case D3DBLEND_INVDESTCOLOR: return GL_ONE_MINUS_DST_COLOR;
            case D3DBLEND_SRCALPHASAT: return GL_SRC_ALPHA_SATURATE;
        }
        return GL_ONE;
    }

    GLenum ToGLCompare(DWORD v) {
        switch (v) {
            case D3DCMP_NEVER: return GL_NEVER;
            case D3DCMP_LESS: return GL_LESS;
            case D3DCMP_EQUAL: return GL_EQUAL;
            case D3DCMP_LESSEQUAL: return GL_LEQUAL;
            case D3DCMP_GREATER: return GL_GREATER;
            case D3DCMP_NOTEQUAL: return GL_NOTEQUAL;
            case D3DCMP_GREATEREQUAL: return GL_GEQUAL;
        }
        return GL_ALWAYS;
    }

    GLenum ToGLMode(D3DPRIMITIVETYPE type, UINT primCount, GLsizei* pCount) {
        switch (type) {
            case D3DPT_POINTLIST: *pCount = primCount; return GL_POINTS;
            case D3DPT_LINELIST: *pCount = primCount * 2; return GL_LINES;
            case D3DPT_LINESTRIP: *pCount = primCount + 1; return GL_LINE_STRIP;
            case D3DPT_TRIANGLESTRIP: *pCount = primCount + 2; return GL_TRIANGLE_STRIP;
            case D3DPT_TRIANGLEFAN: *pCount = primCount + 2; return GL_TRIANGLE_FAN;
        }
        *pCount = primCount * 3;
        return GL_TRIANGLES;
    }

    GLint ToGLWrap(DWORD v) {
        switch (v) {
            case D3DTADDRESS_MIRROR: return GL_MIRRORED_REPEAT;
            case D3DTADDRESS_CLAMP:
            case D3DTADDRESS_BORDER:
            case D3DTADDRESS_MIRRORONCE: return GL_CLAMP_TO_EDGE;
        }
        return GL_REPEAT;
    }

    DWORD FVFFromStride(UINT stride) {
        switch (stride) {
            case 12: return D3DFVF_XYZ;
            case 16: return D3DFVF_XYZ | D3DFVF_DIFFUSE;
            case 20: return D3DFVF_XYZ | D3DFVF_TEX1;
            case 24: return D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
            case 28: return D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
            case 32: return D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
            case 40: return D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2;
        }
        return D3DFVF_XYZ;
    }
}

IDirect3D8* Direct3DCreate8(UINT SDKVersion) {
    return new IDirect3D8();
}

HRESULT IDirect3D8::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice8** ppReturnedDeviceInterface) {
    IDirect3DDevice8* pDevice = new IDirect3DDevice8();
    *ppReturnedDeviceInterface = pDevice;

    GLuint vShader = CompileShader(GL_VERTEX_SHADER, vShaderSource);
    GLuint fShader = CompileShader(GL_FRAGMENT_SHADER, fShaderSource);
    program = glCreateProgram();
    glAttachShader(program, vShader);
    glAttachShader(program, fShader);
    glLinkProgram(program);
    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        char infoLog[1024];
        glGetProgramInfoLog(program, sizeof(infoLog), NULL, infoLog);
        LOGE("Shader Link Failed: %s", infoLog);
    }

    u.WV = glGetUniformLocation(program, "uWV");
    u.Proj = glGetUniformLocation(program, "uProj");
    u.RHW = glGetUniformLocation(program, "uRHW");
    u.Viewport = glGetUniformLocation(program, "uViewport");
    u.FlipY = glGetUniformLocation(program, "uFlipY");
    u.TexMat0 = glGetUniformLocation(program, "uTexMat0");
    u.TexMat1 = glGetUniformLocation(program, "uTexMat1");
    u.TexGen = glGetUniformLocation(program, "uTexGen");
    u.TTFF = glGetUniformLocation(program, "uTTFF");
    u.Lighting = glGetUniformLocation(program, "uLighting");
    u.HasNormal = glGetUniformLocation(program, "uHasNormal");
    u.HasColor = glGetUniformLocation(program, "uHasColor");
    u.MatSrc = glGetUniformLocation(program, "uMatSrc");
    u.MatDiffuse = glGetUniformLocation(program, "uMatDiffuse");
    u.MatAmbient = glGetUniformLocation(program, "uMatAmbient");
    u.MatEmissive = glGetUniformLocation(program, "uMatEmissive");
    u.Ambient = glGetUniformLocation(program, "uAmbient");
    u.LightDir = glGetUniformLocation(program, "uLightDir");
    u.LightDiffuse = glGetUniformLocation(program, "uLightDiffuse");
    u.LightAmbient = glGetUniformLocation(program, "uLightAmbient");
    u.Texture0 = glGetUniformLocation(program, "uTexture0");
    u.Texture1 = glGetUniformLocation(program, "uTexture1");
    u.UseTexture = glGetUniformLocation(program, "uUseTexture");
    u.TFactor = glGetUniformLocation(program, "uTFactor");
    u.ColorStage0 = glGetUniformLocation(program, "uColorStage0");
    u.AlphaStage0 = glGetUniformLocation(program, "uAlphaStage0");
    u.ColorStage1 = glGetUniformLocation(program, "uColorStage1");
    u.AlphaStage1 = glGetUniformLocation(program, "uAlphaStage1");
    u.AlphaFunc = glGetUniformLocation(program, "uAlphaFunc");
    u.AlphaRef = glGetUniformLocation(program, "uAlphaRef");
    u.Dbg = glGetUniformLocation(program, "uDbg");
    u.FogMode = glGetUniformLocation(program, "uFogMode");
    u.FogColor = glGetUniformLocation(program, "uFogColor");
    u.FogParams = glGetUniformLocation(program, "uFogParams");

    glUseProgram(program);
    glUniform1i(u.Texture0, 0);
    glUniform1i(u.Texture1, 1);

    GLint vp[4] = { 0, 0, 1, 1 };
    glGetIntegerv(GL_VIEWPORT, vp);
    pDevice->m_fViewportWidth = (float)(vp[2] > 0 ? vp[2] : 1);
    pDevice->m_fViewportHeight = (float)(vp[3] > 0 ? vp[3] : 1);
    pDevice->m_iSurfaceHeight = vp[3] > 0 ? vp[3] : 1;
    pDevice->m_iWindowHeight = pDevice->m_iSurfaceHeight;
    pDevice->m_viewport.X = vp[0];
    pDevice->m_viewport.Y = vp[1];
    pDevice->m_viewport.Width = vp[2] > 0 ? vp[2] : 1;
    pDevice->m_viewport.Height = vp[3] > 0 ? vp[3] : 1;
    glBlendFunc(GL_ONE, GL_ZERO);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    return S_OK;
}

HRESULT IDirect3DTexture8::LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags) {
    int bpp = (format == D3DFMT_A4R4G4B4 || format == D3DFMT_A1R5G5B5 || format == D3DFMT_X1R5G5B5 || format == D3DFMT_R5G6B5) ? 2 : 4;
    if (Level > 0) {
        if (!pScratch) pScratch = malloc((size_t)width * height * 4);
        pLockedRect->pBits = pScratch;
        pLockedRect->Pitch = (width >> Level ? width >> Level : 1) * bpp;
        return S_OK;
    }
    if (!pLockedData) pLockedData = malloc((size_t)width * height * 4);
    pLockedRect->pBits = pLockedData;
    pLockedRect->Pitch = width * bpp;
    return S_OK;
}

HRESULT IDirect3DTexture8::UnlockRect(UINT Level) {
    if (Level > 0 || !glId || !pLockedData)
        return S_OK;

    const void* pUpload = pLockedData;
    std::vector<DWORD> converted;
    if (format == D3DFMT_A4R4G4B4 || format == D3DFMT_A1R5G5B5 || format == D3DFMT_X1R5G5B5 || format == D3DFMT_R5G6B5) {
        converted.resize((size_t)width * height);
        const WORD* src = (const WORD*)pLockedData;
        for (size_t i = 0; i < converted.size(); ++i) {
            WORD w = src[i];
            DWORD a, r, g, b;
            if (format == D3DFMT_A4R4G4B4) {
                a = ((w >> 12) & 15) * 17; r = ((w >> 8) & 15) * 17; g = ((w >> 4) & 15) * 17; b = (w & 15) * 17;
            } else if (format == D3DFMT_R5G6B5) {
                a = 255; r = ((w >> 11) & 31) * 255 / 31; g = ((w >> 5) & 63) * 255 / 63; b = (w & 31) * 255 / 31;
            } else {
                a = (format == D3DFMT_X1R5G5B5 || (w & 0x8000)) ? 255 : 0;
                r = ((w >> 10) & 31) * 255 / 31; g = ((w >> 5) & 31) * 255 / 31; b = (w & 31) * 255 / 31;
            }
            converted[i] = (a << 24) | (r << 16) | (g << 8) | b;
        }
        pUpload = converted.data();
    }

    glBindTexture(GL_TEXTURE_2D, glId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pUpload);
    return S_OK;
}

ULONG IDirect3DTexture8::Release() {
    if (--refCount > 0)
        return refCount;
    if (glId) glDeleteTextures(1, &glId);
    if (pLockedData) free(pLockedData);
    if (pScratch) free(pScratch);
    delete this;
    return 0;
}

HRESULT IDirect3DDevice8::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, LPDIRECT3DTEXTURE8* ppTexture) {
    IDirect3DTexture8* tex = new IDirect3DTexture8();
    tex->width = Width;
    tex->height = Height;
    tex->format = Format;
    glGenTextures(1, &tex->glId);
    glBindTexture(GL_TEXTURE_2D, tex->glId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    *ppTexture = tex;
    return S_OK;
}

HRESULT IDirect3DTexture8::GetSurfaceLevel(UINT Level, LPDIRECT3DSURFACE8* ppSurface) {
    if (!ppSurface) return D3DERR_INVALIDCALL;
    IDirect3DSurface8* pSurface = new IDirect3DSurface8();
    pSurface->pTexture = this;
    pSurface->width = width >> Level ? width >> Level : 1;
    pSurface->height = height >> Level ? height >> Level : 1;
    pSurface->format = format;
    AddRef();
    *ppSurface = pSurface;
    return S_OK;
}

ULONG IDirect3DSurface8::Release() {
    if (bBackBuffer) return 1;
    if (--refCount > 0) return refCount;
    if (glRbo) glDeleteRenderbuffers(1, &glRbo);
    if (pTexture) pTexture->Release();
    delete this;
    return 0;
}

namespace {
    IDirect3DSurface8* BackBufferSurface(bool bDepth) {
        static IDirect3DSurface8 s_kColor, s_kDepth;
        IDirect3DSurface8& r = bDepth ? s_kDepth : s_kColor;
        r.bBackBuffer = true;
        return &r;
    }
}

HRESULT IDirect3DDevice8::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE, LPDIRECT3DSURFACE8* ppSurface) {
    if (!ppSurface) return D3DERR_INVALIDCALL;
    IDirect3DSurface8* pSurface = new IDirect3DSurface8();
    pSurface->width = Width;
    pSurface->height = Height;
    pSurface->format = Format;
    glGenRenderbuffers(1, &pSurface->glRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, pSurface->glRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, Width, Height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    *ppSurface = pSurface;
    return S_OK;
}

HRESULT IDirect3DDevice8::GetRenderTarget(LPDIRECT3DSURFACE8* ppSurface) {
    if (!ppSurface) return D3DERR_INVALIDCALL;
    *ppSurface = m_pRenderTarget ? m_pRenderTarget : BackBufferSurface(false);
    (*ppSurface)->AddRef();
    return S_OK;
}

HRESULT IDirect3DDevice8::GetDepthStencilSurface(LPDIRECT3DSURFACE8* ppSurface) {
    if (!ppSurface) return D3DERR_INVALIDCALL;
    *ppSurface = m_pDepthStencil ? m_pDepthStencil : BackBufferSurface(true);
    (*ppSurface)->AddRef();
    return S_OK;
}

HRESULT IDirect3DDevice8::GetBackBuffer(UINT, D3DBACKBUFFER_TYPE, LPDIRECT3DSURFACE8* ppSurface) {
    if (!ppSurface) return D3DERR_INVALIDCALL;
    IDirect3DSurface8* pSurface = BackBufferSurface(false);
    pSurface->width = (UINT)m_viewport.Width;
    pSurface->height = (UINT)m_iWindowHeight;
    pSurface->format = D3DFMT_X8R8G8B8;
    *ppSurface = pSurface;
    return S_OK;
}

HRESULT IDirect3DDevice8::SetRenderTarget(LPDIRECT3DSURFACE8 pRenderTarget, LPDIRECT3DSURFACE8 pDepthStencil) {
    if (pRenderTarget && pRenderTarget->bBackBuffer) pRenderTarget = NULL;
    if (pDepthStencil && pDepthStencil->bBackBuffer) pDepthStencil = NULL;
    if (pRenderTarget && !pRenderTarget->pTexture) return D3DERR_INVALIDCALL;

    if (pRenderTarget) pRenderTarget->AddRef();
    if (pDepthStencil) pDepthStencil->AddRef();
    if (m_pRenderTarget) m_pRenderTarget->Release();
    if (m_pDepthStencil) m_pDepthStencil->Release();
    m_pRenderTarget = pRenderTarget;
    m_pDepthStencil = pDepthStencil;

    if (!pRenderTarget) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        m_iSurfaceHeight = m_iWindowHeight;
        return S_OK;
    }

    if (!m_glFbo) glGenFramebuffers(1, &m_glFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_glFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pRenderTarget->pTexture->glId, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pDepthStencil ? pDepthStencil->glRbo : 0);
    m_iSurfaceHeight = (int)pRenderTarget->height;
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        LOGI("SetRenderTarget: incomplete framebuffer 0x%x", status);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        m_iSurfaceHeight = m_iWindowHeight;
        return D3DERR_INVALIDCALL;
    }
    return S_OK;
}

HRESULT IDirect3DDevice8::SetViewport(const D3DVIEWPORT8* v) {
    m_viewport = *v;
    // D3D viewports are top-left based; GL's are bottom-left based.
    if (m_pRenderTarget && m_pRenderTarget->pTexture)
        glViewport(v->X, v->Y, v->Width, v->Height);
    else
        glViewport(v->X, m_iSurfaceHeight - (int)v->Y - (int)v->Height, v->Width, v->Height);
    m_fViewportWidth = (float)(v->Width ? v->Width : 1);
    m_fViewportHeight = (float)(v->Height ? v->Height : 1);
    return S_OK;
}

HRESULT IDirect3DDevice8::Clear(DWORD Count, const void* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) {
    GLbitfield mask = 0;
    if (Flags & D3DCLEAR_TARGET) {
        glClearColor(Color.r, Color.g, Color.b, Color.a);
        mask |= GL_COLOR_BUFFER_BIT;
    }
    if (Flags & D3DCLEAR_ZBUFFER) {
        glClearDepthf(Z);
        glDepthMask(GL_TRUE);
        mask |= GL_DEPTH_BUFFER_BIT;
    }
    if (mask) glClear(mask);
    return S_OK;
}

HRESULT IDirect3DDevice8::SetTransform(DWORD type, const GLMATRIX* mat) {
    if (!mat) return S_OK;
    switch (type) {
        case D3DTS_WORLD: m_matWorld = *mat; break;
        case D3DTS_VIEW: m_matView = *mat; break;
        case D3DTS_PROJECTION: m_matProj = *mat; break;
        case D3DTS_TEXTURE0: m_amatTexture[0] = *mat; break;
        case D3DTS_TEXTURE1: m_amatTexture[1] = *mat; break;
    }
    return S_OK;
}

HRESULT IDirect3DDevice8::GetTransform(DWORD type, GLMATRIX* mat) {
    if (!mat) return S_OK;
    switch (type) {
        case D3DTS_WORLD: *mat = m_matWorld; break;
        case D3DTS_VIEW: *mat = m_matView; break;
        case D3DTS_PROJECTION: *mat = m_matProj; break;
        case D3DTS_TEXTURE0: *mat = m_amatTexture[0]; break;
        case D3DTS_TEXTURE1: *mat = m_amatTexture[1]; break;
        default: D3DXMatrixIdentity(mat); break;
    }
    return S_OK;
}

HRESULT IDirect3DDevice8::SetRenderState(DWORD Type, DWORD Value) {
    switch (Type) {
        case D3DRS_ZENABLE:
            if (Value) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
            break;
        case D3DRS_ZFUNC:
            glDepthFunc(ToGLCompare(Value));
            break;
        case D3DRS_ZWRITEENABLE:
            glDepthMask(Value ? GL_TRUE : GL_FALSE);
            break;
        case D3DRS_ALPHABLENDENABLE:
            if (Value) glEnable(GL_BLEND); else glDisable(GL_BLEND);
            break;
        case D3DRS_SRCBLEND:
            m_dwSrcBlend = Value;
            glBlendFunc(ToGLBlend(m_dwSrcBlend), ToGLBlend(m_dwDestBlend));
            break;
        case D3DRS_DESTBLEND:
            m_dwDestBlend = Value;
            glBlendFunc(ToGLBlend(m_dwSrcBlend), ToGLBlend(m_dwDestBlend));
            break;
        case D3DRS_ALPHATESTENABLE:
            m_bAlphaTest = Value ? TRUE : FALSE;
            break;
        case D3DRS_ALPHAREF:
            m_dwAlphaRef = Value;
            break;
        case D3DRS_ALPHAFUNC:
            m_dwAlphaFunc = Value;
            break;
        case D3DRS_TEXTUREFACTOR:
            m_dwTextureFactor = Value;
            break;
        case D3DRS_LIGHTING: m_bLighting = Value ? TRUE : FALSE; break;
        case D3DRS_COLORVERTEX: m_bColorVertex = Value ? TRUE : FALSE; break;
        case D3DRS_DIFFUSEMATERIALSOURCE: m_dwDiffuseMaterialSource = Value; break;
        case D3DRS_AMBIENTMATERIALSOURCE: m_dwAmbientMaterialSource = Value; break;
        case D3DRS_EMISSIVEMATERIALSOURCE: m_dwEmissiveMaterialSource = Value; break;
        case D3DRS_AMBIENT: m_dwAmbient = Value; break;
        case D3DRS_FOGENABLE: m_bFogEnable = Value ? TRUE : FALSE; break;
        case D3DRS_FOGCOLOR: m_dwFogColor = Value; break;
        case D3DRS_FOGTABLEMODE: m_dwFogTableMode = Value; break;
        case D3DRS_FOGVERTEXMODE: m_dwFogVertexMode = Value; break;
        case D3DRS_FOGSTART: m_fFogStart = DwordToFloat(Value); break;
        case D3DRS_FOGEND: m_fFogEnd = DwordToFloat(Value); break;
        case D3DRS_FOGDENSITY: m_fFogDensity = DwordToFloat(Value); break;
        case D3DRS_CULLMODE:
            // Metin2 relies on D3D's left-handed winding; keep culling off until 3D winding is validated.
            glDisable(GL_CULL_FACE);
            break;
    }
    return S_OK;
}

HRESULT IDirect3DDevice8::SetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE type, DWORD value) {
    if (stage >= MAX_STAGES) return S_OK;
    switch (type) {
        case D3DTSS_COLOROP: m_adwColorOp[stage] = value; break;
        case D3DTSS_COLORARG1: m_adwColorArg1[stage] = value; break;
        case D3DTSS_COLORARG2: m_adwColorArg2[stage] = value; break;
        case D3DTSS_ALPHAOP: m_adwAlphaOp[stage] = value; break;
        case D3DTSS_ALPHAARG1: m_adwAlphaArg1[stage] = value; break;
        case D3DTSS_ALPHAARG2: m_adwAlphaArg2[stage] = value; break;
        case D3DTSS_ADDRESSU: m_adwAddressU[stage] = value; break;
        case D3DTSS_ADDRESSV: m_adwAddressV[stage] = value; break;
        case D3DTSS_MAGFILTER: m_adwMagFilter[stage] = value; break;
        case D3DTSS_MINFILTER: m_adwMinFilter[stage] = value; break;
        case D3DTSS_TEXCOORDINDEX: m_adwTexCoordIndex[stage] = value; break;
        case D3DTSS_TEXTURETRANSFORMFLAGS: m_adwTextureTransformFlags[stage] = value; break;
    }
    return S_OK;
}

HRESULT IDirect3DDevice8::SetTexture(DWORD Stage, LPDIRECT3DBaseTexture8 pTexture) {
    if (Stage < MAX_STAGES)
        m_apTexture[Stage] = (IDirect3DTexture8*)pTexture;
    return S_OK;
}

HRESULT IDirect3DDevice8::SetLight(DWORD index, const D3DLIGHT8* pLight) {
    if (index < MAX_LIGHTS && pLight)
        m_aLight[index] = *pLight;
    return S_OK;
}

HRESULT IDirect3DDevice8::LightEnable(DWORD index, BOOL bEnable) {
    if (index < MAX_LIGHTS)
        m_abLightEnable[index] = bEnable;
    return S_OK;
}

void IDirect3DDevice8::ApplyDrawState(const BYTE* pVertexBase, UINT uStride) {
    ++g_uDbgDraws;
    glUseProgram(program);

    DWORD fvf = m_dwFVF;
    if (!(fvf & (D3DFVF_XYZ | D3DFVF_XYZRHW)) || D3DXGetFVFVertexSize(fvf) != uStride)
        fvf = FVFFromStride(uStride);

    UINT offset = 0;
    bool bRHW = (fvf & D3DFVF_XYZRHW) != 0;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, bRHW ? 4 : 3, GL_FLOAT, GL_FALSE, uStride, pVertexBase + offset);
    offset += bRHW ? 16 : 12;
    bool bNormal = (fvf & D3DFVF_NORMAL) != 0;
    if (bNormal) {
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, uStride, pVertexBase + offset);
        offset += 12;
    } else {
        glDisableVertexAttribArray(4);
        glVertexAttrib4f(4, 0.0f, 0.0f, -1.0f, 0.0f);
    }
    bool bColor = (fvf & D3DFVF_DIFFUSE) != 0;
    if (bColor) {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, uStride, pVertexBase + offset);
        offset += 4;
    } else {
        glDisableVertexAttribArray(2);
        glVertexAttrib4f(2, 1.0f, 1.0f, 1.0f, 1.0f);
    }
    if (fvf & D3DFVF_SPECULAR) offset += 4;
    int iTexCount = (fvf & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
    static const UINT s_auTexSize[4] = { 8, 12, 16, 4 };
    for (int i = 0; i < 2; ++i) {
        GLuint attr = i == 0 ? 1 : 3;
        if (i < iTexCount) {
            glEnableVertexAttribArray(attr);
            glVertexAttribPointer(attr, 2, GL_FLOAT, GL_FALSE, uStride, pVertexBase + offset);
            offset += s_auTexSize[(fvf >> (i * 2 + 16)) & 3];
        } else {
            glDisableVertexAttribArray(attr);
            glVertexAttrib4f(attr, 0.0f, 0.0f, 0.0f, 0.0f);
        }
    }

    GLMATRIX wv;
    D3DXMatrixMultiply(&wv, &m_matWorld, &m_matView);
    glUniformMatrix4fv(u.WV, 1, GL_FALSE, &wv.m[0][0]);
    glUniformMatrix4fv(u.Proj, 1, GL_FALSE, &m_matProj.m[0][0]);
    glUniform1i(u.RHW, bRHW ? 1 : 0);
    glUniform2f(u.Viewport, m_fViewportWidth, m_fViewportHeight);
    glUniform1f(u.FlipY, m_pRenderTarget && m_pRenderTarget->pTexture ? -1.0f : 1.0f);
    glUniformMatrix4fv(u.TexMat0, 1, GL_FALSE, &m_amatTexture[0].m[0][0]);
    glUniformMatrix4fv(u.TexMat1, 1, GL_FALSE, &m_amatTexture[1].m[0][0]);
    glUniform2i(u.TexGen, (GLint)m_adwTexCoordIndex[0], (GLint)m_adwTexCoordIndex[1]);
    glUniform2i(u.TTFF, (GLint)m_adwTextureTransformFlags[0], (GLint)m_adwTextureTransformFlags[1]);

    bool bLighting = m_bLighting && !bRHW;
    glUniform1i(u.Lighting, bLighting ? 1 : 0);
    glUniform1i(u.HasNormal, bNormal ? 1 : 0);
    glUniform1i(u.HasColor, bColor ? 1 : 0);
    if (bLighting) {
        glUniform3i(u.MatSrc,
            m_bColorVertex && m_dwDiffuseMaterialSource == D3DMCS_COLOR1 ? 1 : 0,
            m_bColorVertex && m_dwAmbientMaterialSource == D3DMCS_COLOR1 ? 1 : 0,
            m_bColorVertex && m_dwEmissiveMaterialSource == D3DMCS_COLOR1 ? 1 : 0);
        SetColorUniform(u.MatDiffuse, m_material.Diffuse);
        SetColorUniform(u.MatAmbient, m_material.Ambient);
        SetColorUniform(u.MatEmissive, m_material.Emissive);
        SetColorUniform(u.Ambient, GLCOLOR((unsigned int)m_dwAmbient));
        GLfloat afDir[MAX_LIGHTS * 4], afDiffuse[MAX_LIGHTS * 4], afAmbient[MAX_LIGHTS * 4];
        for (int i = 0; i < MAX_LIGHTS; ++i) {
            const D3DLIGHT8& l = m_aLight[i];
            const GLVECTOR3& d = l.Direction;
            float x = d.x * m_matView.m[0][0] + d.y * m_matView.m[1][0] + d.z * m_matView.m[2][0];
            float y = d.x * m_matView.m[0][1] + d.y * m_matView.m[1][1] + d.z * m_matView.m[2][1];
            float z = d.x * m_matView.m[0][2] + d.y * m_matView.m[1][2] + d.z * m_matView.m[2][2];
            float len = sqrtf(x * x + y * y + z * z);
            if (len > 0.0f) { x /= len; y /= len; z /= len; }
            afDir[i * 4 + 0] = x; afDir[i * 4 + 1] = y; afDir[i * 4 + 2] = z;
            afDir[i * 4 + 3] = (m_abLightEnable[i] && len > 0.0f) ? 1.0f : 0.0f;
            afDiffuse[i * 4 + 0] = l.Diffuse.r; afDiffuse[i * 4 + 1] = l.Diffuse.g; afDiffuse[i * 4 + 2] = l.Diffuse.b; afDiffuse[i * 4 + 3] = l.Diffuse.a;
            afAmbient[i * 4 + 0] = l.Ambient.r; afAmbient[i * 4 + 1] = l.Ambient.g; afAmbient[i * 4 + 2] = l.Ambient.b; afAmbient[i * 4 + 3] = l.Ambient.a;
        }
        glUniform4fv(u.LightDir, MAX_LIGHTS, afDir);
        glUniform4fv(u.LightDiffuse, MAX_LIGHTS, afDiffuse);
        glUniform4fv(u.LightAmbient, MAX_LIGHTS, afAmbient);
    }

    GLint aiUseTexture[MAX_STAGES] = { 0, 0 };
    for (int i = 0; i < MAX_STAGES; ++i) {
        IDirect3DTexture8* pTex = m_apTexture[i];
        glActiveTexture(GL_TEXTURE0 + i);
        if (pTex && pTex->glId) {
            glBindTexture(GL_TEXTURE_2D, pTex->glId);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ToGLWrap(m_adwAddressU[i]));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ToGLWrap(m_adwAddressV[i]));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_adwMagFilter[i] == D3DTEXF_POINT ? GL_NEAREST : GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_adwMinFilter[i] == D3DTEXF_POINT ? GL_NEAREST : GL_LINEAR);
            aiUseTexture[i] = 1;
        } else {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    glActiveTexture(GL_TEXTURE0);
    glUniform2i(u.UseTexture, aiUseTexture[0], aiUseTexture[1]);

    SetColorUniform(u.TFactor, GLCOLOR((unsigned int)m_dwTextureFactor));
    glUniform3i(u.ColorStage0, (GLint)m_adwColorOp[0], (GLint)m_adwColorArg1[0], (GLint)m_adwColorArg2[0]);
    glUniform3i(u.AlphaStage0, (GLint)m_adwAlphaOp[0], (GLint)m_adwAlphaArg1[0], (GLint)m_adwAlphaArg2[0]);
    glUniform3i(u.ColorStage1, (GLint)m_adwColorOp[1], (GLint)m_adwColorArg1[1], (GLint)m_adwColorArg2[1]);
    glUniform3i(u.AlphaStage1, (GLint)m_adwAlphaOp[1], (GLint)m_adwAlphaArg1[1], (GLint)m_adwAlphaArg2[1]);
    glUniform1i(u.AlphaFunc, m_bAlphaTest ? (GLint)m_dwAlphaFunc : D3DCMP_ALWAYS);
    glUniform1f(u.AlphaRef, (m_dwAlphaRef & 0xff) / 255.0f);
    glUniform1i(u.Dbg, (g_iDbgSolid >= 4 && !bRHW && fvf == (D3DFVF_XYZ | D3DFVF_NORMAL) && (g_iDbgSolid < 8 || m_bLighting)) ? g_iDbgSolid : 0);

    GLint iFogMode = 0;
    if (m_bFogEnable && !bRHW)
        iFogMode = (GLint)(m_dwFogTableMode != D3DFOG_NONE ? m_dwFogTableMode : m_dwFogVertexMode);
    glUniform1i(u.FogMode, iFogMode);
    if (iFogMode) {
        SetColorUniform(u.FogColor, GLCOLOR((unsigned int)m_dwFogColor));
        glUniform3f(u.FogParams, m_fFogStart, m_fFogEnd, m_fFogDensity);
    }

    if (g_iDbgSolid >= 2 && !bRHW && fvf == (D3DFVF_XYZ | D3DFVF_NORMAL)) {
        SetColorUniform(u.TFactor, GLCOLOR(0xffffffffu));
        glUniform3i(u.ColorStage0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_TEXTURE);
        glUniform3i(u.AlphaStage0, D3DTOP_SELECTARG1, D3DTA_TFACTOR, D3DTA_TFACTOR);
        glUniform3i(u.ColorStage1, g_iDbgSolid == 3 ? D3DTOP_SELECTARG1 : D3DTOP_DISABLE, D3DTA_TEXTURE | (g_iDbgSolid == 3 ? 0x20 : 0), D3DTA_TEXTURE);
        glUniform3i(u.AlphaStage1, D3DTOP_SELECTARG1, D3DTA_TFACTOR, D3DTA_TFACTOR);
        glUniform1i(u.AlphaFunc, D3DCMP_ALWAYS);
    }
    else if (g_iDbgSolid == 1 && !bRHW) {
        SetColorUniform(u.TFactor, GLCOLOR(fvf == (D3DFVF_XYZ | D3DFVF_NORMAL) ? 0xffff0000u : (fvf == (D3DFVF_XYZ | D3DFVF_TEX1) ? 0xff00ff00u : 0xff0000ffu)));
        glUniform3i(u.ColorStage0, D3DTOP_SELECTARG1, D3DTA_TFACTOR, D3DTA_TFACTOR);
        glUniform3i(u.AlphaStage0, D3DTOP_SELECTARG1, D3DTA_TFACTOR, D3DTA_TFACTOR);
        glUniform3i(u.ColorStage1, D3DTOP_DISABLE, 0, 0);
        glUniform1i(u.AlphaFunc, D3DCMP_ALWAYS);
    }
    if (g_bDbgDump) {
        static std::set<unsigned long long> s_kSeen;
        unsigned long long ullKey = ((unsigned long long)m_dwFVF << 40) ^ ((unsigned long long)fvf << 16) ^ uStride ^ ((unsigned long long)m_adwTexCoordIndex[0] << 8);
        if (s_kSeen.insert(ullKey).second && s_kSeen.size() < 60)
            LOGI("DBG combo setfvf=%x fvf=%x stride=%u tci0=%x tex0=%u lit=%d", m_dwFVF, fvf, uStride, m_adwTexCoordIndex[0], m_apTexture[0] ? m_apTexture[0]->glId : 0, bLighting);
        static int s_iTerrLogs = 0;
        if (fvf == (D3DFVF_XYZ | D3DFVF_NORMAL) && s_iTerrLogs < 12) {
            ++s_iTerrLogs;
            const GLMATRIX& t0 = m_amatTexture[0];
            LOGI("DBG terr tm0=[%.4g %.4g %.4g %.4g|%.4g %.4g %.4g %.4g|%.4g %.4g %.4g %.4g|%.4g %.4g %.4g %.4g] fog=%d %.1f-%.1f col=%08x wv41=%.1f,%.1f,%.1f",
                t0._11, t0._12, t0._13, t0._14, t0._21, t0._22, t0._23, t0._24, t0._31, t0._32, t0._33, t0._34, t0._41, t0._42, t0._43, t0._44,
                iFogMode, m_fFogStart, m_fFogEnd, m_dwFogColor, wv._41, wv._42, wv._43);
            const GLMATRIX& t1 = m_amatTexture[1];
            LOGI("DBG terr tm1=[%.4g %.4g %.4g %.4g|%.4g %.4g %.4g %.4g|%.4g %.4g %.4g %.4g|%.4g %.4g %.4g %.4g] addr1=%u,%u",
                t1._11, t1._12, t1._13, t1._14, t1._21, t1._22, t1._23, t1._24, t1._31, t1._32, t1._33, t1._34, t1._41, t1._42, t1._43, t1._44,
                m_adwAddressU[1], m_adwAddressV[1]);
        }
        if (fvf == (D3DFVF_XYZ | D3DFVF_NORMAL) && s_iTerrLogs < 12) LOGI("DBG draw fvf=%x stride=%u rhw=%d lit=%d tex=%u/%u c0=%d,%x,%x a0=%d,%x,%x c1=%d,%x,%x a1=%d,%x,%x tci=%x/%x ttff=%x/%x at=%d af=%d ref=%x blend=%d fog=%d tf=%08x",
            fvf, uStride, bRHW, bLighting, m_apTexture[0] ? m_apTexture[0]->glId : 0, m_apTexture[1] ? m_apTexture[1]->glId : 0,
            m_adwColorOp[0], m_adwColorArg1[0], m_adwColorArg2[0], m_adwAlphaOp[0], m_adwAlphaArg1[0], m_adwAlphaArg2[0],
            m_adwColorOp[1], m_adwColorArg1[1], m_adwColorArg2[1], m_adwAlphaOp[1], m_adwAlphaArg1[1], m_adwAlphaArg2[1],
            m_adwTexCoordIndex[0], m_adwTexCoordIndex[1], m_adwTextureTransformFlags[0], m_adwTextureTransformFlags[1],
            m_bAlphaTest, m_dwAlphaFunc, m_dwAlphaRef, glIsEnabled(GL_BLEND), iFogMode, m_dwTextureFactor);
        if (bLighting)
            LOGI("DBG   mat d=%.2f,%.2f,%.2f a=%.2f,%.2f,%.2f amb=%08x l0en=%d l0dir=%.2f,%.2f,%.2f l0d=%.2f,%.2f,%.2f",
                m_material.Diffuse.r, m_material.Diffuse.g, m_material.Diffuse.b, m_material.Ambient.r, m_material.Ambient.g, m_material.Ambient.b,
                m_dwAmbient, m_abLightEnable[0], m_aLight[0].Direction.x, m_aLight[0].Direction.y, m_aLight[0].Direction.z,
                m_aLight[0].Diffuse.r, m_aLight[0].Diffuse.g, m_aLight[0].Diffuse.b);
    }
}

HRESULT IDirect3DDevice8::DrawPrimitive(D3DPRIMITIVETYPE Type, UINT StartVertex, UINT PrimitiveCount) {
    if (!m_pStreamSource || !m_StreamStride) return S_OK;
    GLsizei count;
    GLenum mode = ToGLMode(Type, PrimitiveCount, &count);
    glBindBuffer(GL_ARRAY_BUFFER, m_pStreamSource->glVbo);
    ApplyDrawState((const BYTE*)0, m_StreamStride);
    glDrawArrays(mode, StartVertex, count);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return S_OK;
}

HRESULT IDirect3DDevice8::DrawIndexedPrimitive(D3DPRIMITIVETYPE Type, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount) {
    if (!m_pStreamSource || !m_StreamStride || !m_pIndexBuffer) return S_OK;
    if (g_iDbgSolid == 7 && m_bLighting && m_dwFVF == (D3DFVF_XYZ | D3DFVF_NORMAL)) return S_OK;
    GLsizei count;
    GLenum mode = ToGLMode(Type, PrimitiveCount, &count);
    glBindBuffer(GL_ARRAY_BUFFER, m_pStreamSource->glVbo);
    ApplyDrawState((const BYTE*)0 + (size_t)m_BaseVertexIndex * m_StreamStride, m_StreamStride);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pIndexBuffer->glIbo);
    UINT indexSize = m_pIndexBuffer->indexSize;
    glDrawElements(mode, count, indexSize == 4 ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT, (void*)(size_t)(StartIndex * indexSize));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return S_OK;
}

HRESULT IDirect3DDevice8::DrawPrimitiveUP(D3DPRIMITIVETYPE Type, UINT PrimitiveCount, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride) {
    if (!pVertexStreamZeroData || !VertexStreamZeroStride) return S_OK;
    GLsizei count;
    GLenum mode = ToGLMode(Type, PrimitiveCount, &count);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    ApplyDrawState((const BYTE*)pVertexStreamZeroData, VertexStreamZeroStride);
    glDrawArrays(mode, 0, count);
    // D3D8 semantics: DrawPrimitiveUP resets stream 0.
    m_pStreamSource = NULL;
    m_StreamStride = 0;
    return S_OK;
}

HRESULT IDirect3DDevice8::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE Type, UINT MinVertexIndex, UINT NumVertexIndices, UINT PrimitiveCount, const void* pIndexData, D3DFORMAT IndexDataFormat, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride) {
    if (!pVertexStreamZeroData || !VertexStreamZeroStride || !pIndexData) return S_OK;
    GLsizei count;
    GLenum mode = ToGLMode(Type, PrimitiveCount, &count);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    ApplyDrawState((const BYTE*)pVertexStreamZeroData, VertexStreamZeroStride);
    glDrawElements(mode, count, IndexDataFormat == D3DFMT_INDEX16 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, pIndexData);
    m_pStreamSource = NULL;
    m_StreamStride = 0;
    m_pIndexBuffer = NULL;
    return S_OK;
}

HRESULT IDirect3DDevice8::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters) { return S_OK; }
HRESULT IDirect3DDevice8::BeginScene() { return S_OK; }
HRESULT IDirect3DDevice8::EndScene() { return S_OK; }
HRESULT IDirect3DDevice8::Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const void* pDirtyRegion) {
    if (g_platform.present)
        g_platform.present(g_platform.user);
    return S_OK;
}
HRESULT IDirect3DDevice8::SetStreamSource(UINT StreamNumber, LPDIRECT3DVERTEXBUFFER8 pStreamData, UINT Stride) {
    if (StreamNumber == 0) {
        m_pStreamSource = pStreamData;
        m_StreamStride = Stride;
    }
    return S_OK;
}
HRESULT IDirect3DDevice8::SetIndices(LPDIRECT3DINDEXBUFFER8 pIndexBuffer, UINT BaseVertexIndex) {
    m_pIndexBuffer = pIndexBuffer;
    m_BaseVertexIndex = BaseVertexIndex;
    return S_OK;
}
