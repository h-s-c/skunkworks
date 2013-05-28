// Public Domain
#pragma once

#include <stdexcept>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#define GLCOREARB_PROTOTYPES
#include <GL/glcorearb.h>


template <typename F>
auto EGL_CheckError(F f) -> F
{
    if (!f) 
    {
        std::string error = "";
        EGLint egl_error = eglGetError();
        switch (egl_error) 
        {
            case EGL_SUCCESS:
                error = "No error";  break;
            case EGL_NOT_INITIALIZED: 
                error = "EGL not, or could not be, initialised";  break;
            case EGL_BAD_ACCESS: 
                error = "Access violation";  break;
            case EGL_BAD_ALLOC: 
                error = "Could not allocate resources";  break;
            case EGL_BAD_ATTRIBUTE: 
                error = "Invalid attribute";  break;
            case EGL_BAD_CONTEXT: 
                error = "Invalid context specified"; break;
            case EGL_BAD_CONFIG: 
                error = "Invald frame buffer configuration specified"; break;
            case EGL_BAD_CURRENT_SURFACE:
                error = "Current window, pbuffer or pixmap surface is no longer valid"; break;
            case EGL_BAD_DISPLAY: 
                error = "Invalid display specified"; break;
            case EGL_BAD_SURFACE: 
                error = "Invalid surface specified"; break;
            case EGL_BAD_MATCH: 
                error = "Bad argument match"; break;
            case EGL_BAD_PARAMETER: 
                error = "Invalid paramater"; break;
            case EGL_BAD_NATIVE_PIXMAP: 
                error = "Invalid NativePixmap"; break;
            case EGL_BAD_NATIVE_WINDOW: 
                error = "Invalid NativeWindow"; break;
            case EGL_CONTEXT_LOST: 
                error = "APM event caused context loss"; break;
            default: 
                error = "Unknown error " + std::to_string(egl_error); break;
        }
        std::runtime_error e(error);
        throw e;
    }
    return f;
}

template <typename F>
auto GL_CheckError(F f) -> F
{
    if (!f) 
    {
        /*std::runtime_error e(glGetError());
        throw e;*/
    }
    return f;
}
