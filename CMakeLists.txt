cmake_minimum_required(VERSION 3.10)
project(d8gles CXX)

option(D8GLES_BUILD_TESTS "Build the headless render test (needs EGL)" ${PROJECT_IS_TOP_LEVEL})
option(D8GLES_DESKTOP_GL "Use desktop GL headers instead of GLES" OFF)

add_library(d8gles STATIC src/d8gles.cpp)
add_library(d8gles::d8gles ALIAS d8gles)
target_compile_features(d8gles PUBLIC cxx_std_11)
target_include_directories(d8gles PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
set_target_properties(d8gles PROPERTIES POSITION_INDEPENDENT_CODE ON)
if(D8GLES_DESKTOP_GL)
    target_compile_definitions(d8gles PUBLIC D8GLES_DESKTOP_GL)
endif()

# The host links its own GL ES library (Android: GLESv3); the test links it here.
if(D8GLES_BUILD_TESTS)
    enable_testing()
    find_library(D8GLES_EGL EGL)
    find_library(D8GLES_GLES GLESv2)
    if(D8GLES_EGL AND D8GLES_GLES)
        add_executable(d8gles_render_test tests/render_test.cpp)
        target_link_libraries(d8gles_render_test PRIVATE d8gles ${D8GLES_GLES} ${D8GLES_EGL})
        add_test(NAME render COMMAND d8gles_render_test)
        set_tests_properties(render PROPERTIES SKIP_RETURN_CODE 77 ENVIRONMENT "EGL_PLATFORM=surfaceless")
    endif()
endif()

install(TARGETS d8gles EXPORT d8glesTargets ARCHIVE DESTINATION lib)
install(DIRECTORY include/d8gles DESTINATION include)
install(EXPORT d8glesTargets NAMESPACE d8gles:: DESTINATION lib/cmake/d8gles)
