# d8gles

The Direct3D 8 / D3DX 8 API used by early-2000s game clients, implemented on
OpenGL ES 3.0. Game code keeps calling `IDirect3DDevice8::SetRenderState`,
`SetTextureStageState`, `DrawIndexedPrimitive`, `LockRect` and friends; d8gles
turns them into GL calls and a generated fixed-function shader.

It was split out of the [Metin2 Android port](https://github.com/cemreefe/metin2-android),
where it renders terrain, water, characters, effects and the whole 2D UI.

## Layering

```
game / engine code          (D3D8 calls, unchanged)
  d8gles                    this library: API surface + GLES 3.0 backend, no OS code
platform adapter            yours: creates the window + GLES context, installs D8GLES_Platform
OS                          Android (EGL), Linux (EGL), Windows (ANGLE), ...
```

d8gles never creates a window or a context. The platform makes a GLES 3.0 context
current on the render thread, installs its hooks, and then creates the device:

```cpp
#include <d8gles/d8gles.h>

static void Present(void*) { eglSwapBuffers(display, surface); }
static void Log(int level, const char* msg, void*) { fprintf(stderr, "%s\n", msg); }

D8GLES_Platform platform = { Present, Log, nullptr };
D8GLES_SetPlatform(&platform);
IDirect3D8* d3d = Direct3DCreate8(D3D_SDK_VERSION);
IDirect3DDevice8* device;
d3d->CreateDevice(0, D3DDEVTYPE_HAL, hwnd, 0, &params, &device);
```

`IDirect3DDevice8::Present` calls `platform.present`. Everything else is plain GL
on the current context. `D8GLES_SetDebug` turns on flat-shading diagnostics and a
one-frame draw-state dump through `platform.log`.

The Android adapter in the Metin2 port, `clientsource/EterLib/GrpOpenGL.cpp`,
does exactly this: `eglSwapBuffers`, `__android_log_print`, and polls system
properties to drive the debug switches.

## What is implemented

- **Fixed-function pipeline in one shader:** pre-transformed (`XYZRHW`) and
  transformed vertices, world/view/projection, two texture stages with the common
  `D3DTOP_*`/`D3DTA_*` ops, texture coordinate generation (camera-space position
  and normal) and texture transforms, per-vertex lighting (directional and point
  lights, materials), linear fog, alpha test, and alpha blending.
- **FVF vertex layouts:** XYZ/XYZRHW, normal, diffuse, specular, up to two sets of
  texture coordinates.
- **Resources:** textures (A8R8G8B8, X8R8G8B8, R5G6B5, A4R4G4B4, A1R5G5B5,
  DXT1/3/5 where the GL supports them, A8/L8), `LockRect`/`UnlockRect`, vertex
  and index buffers (VBO/IBO), render-target textures through FBOs, and depth
  surfaces.
- **Render state:** z test/write, cull mode, blend factors, fill mode, viewport,
  scissor, and clear.
- **D3DX:** the matrix, vector, plane and quaternion helpers the clients use,
  with D3DX semantics (row vectors, left-handed projection helpers).

Not implemented: programmable vertex/pixel shaders (`CreateVertexShader` returns
a dummy handle), more than two texture stages, volume and cube textures, stencil
operations, `D3DXCreateSphere`-style mesh generation (returns empty meshes), and
multithreaded device use.

This list reflects what the Metin2 client needs. Other games will hit gaps; the
debug dump shows which states a draw uses.

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure    # headless EGL render test (Mesa works)
```

In a CMake project:

```cmake
add_subdirectory(third_party/d8gles)
target_link_libraries(game PRIVATE d8gles::d8gles GLESv3)   # Android; elsewhere GLESv2 + EGL
```

If your project already has its own Win32 type shim (`GUID`, `LARGE_INTEGER`,
...), define `D8GLES_HAVE_WIN32_TYPES` so d8gles doesn't declare them again.

The test (`tests/render_test.cpp`) creates a 64x64 EGL pbuffer, clears it blue,
draws an `XYZRHW | DIFFUSE` triangle through the D3D8 API, and checks the pixels.
It is skipped (exit 77) when no GLES 3 EGL context is available.

## Provenance

The code started as the OpenGL bridge in
[Bahori35/metin2-android](https://github.com/Bahori35/metin2-android) and was
mostly rewritten during the Android port (fixed-function shader, FBO render
targets, D3DX math, texture formats, the platform hooks). It contains no
Microsoft code; the API names and constants follow the public DirectX 8 SDK
documentation. Direct3D is a trademark of Microsoft; this project is not
affiliated with Microsoft.

The license has not been chosen yet: the upstream bridge was published without
one.
