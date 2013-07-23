/*
** Copyright (c) 2012 Pierre-Marc Jobin
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
** THE SOFTWARE.
*/

#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#define GLX_ARB_render_texture 1    /* GLX_ARB_render_texture symbols are not exported by default! */
#include <GL/glx.h>
#include "glxext.h"

#define countof(a)          (sizeof (a) / sizeof (a[0]))

#define UNUSED(x)           (void)(x)

#define _STRINGIZE(x)       #x
#define STRINGIZE(x)        _STRINGIZE(x)

#define EGL_MAJOR           1
#define EGL_MINOR           4

#define GLX_EXT(e, m)
#define GLX_PROC(r, f, ...) typedef r (*f##_proc)(__VA_ARGS__);
#include "glx.def"
#undef GLX_EXT
#undef GLX_PROC

#define GLX_EXT(e, m)   static EGLBoolean e##_flag;
#define GLX_PROC(r, f, ...) static f##_proc f;
#include "glx.def"
#undef GLX_EXT
#undef GLX_PROC

typedef enum
{
    EGL_PBUFFER,
    EGL_PIXMAP,
    EGL_WINDOW
} EGLSurfaceType;

static struct
{
    EGLenum renderable;
    EGLNativeDisplayType default_display;
    XContext surface_type;
}
egl =
{
    .renderable = 0,
    .default_display = NULL,
    .surface_type = 0
};

static __thread struct
{
    EGLint error;
    EGLenum api;
}
egl_thread =
{
    .error = EGL_SUCCESS,
    .api = EGL_OPENGL_ES_API
};

extern EGLBoolean eglLinkGL(void);
extern EGLBoolean eglLinkGLES(void);
extern EGLBoolean eglLinkGLES2(void);
extern EGLBoolean eglLinkGLES3(void);

static EGLenum eglLink(EGLNativeDisplayType dpy)
{
    const char *exts, *missing_warning = "[WARNING] Missing GLX extension: %s.\n", *missing_error = "[ERROR] Missing GLX extension: %s.\n", *entry_error = "[ERROR] Missing GLX entry point: %s.\n";
    EGLBoolean failed = EGL_FALSE, mandatory;
    EGLenum renderable = 0;

    exts = glXQueryExtensionsString(dpy, DefaultScreen(dpy));

#define GLX_EXT(e, m)\
    e##_flag = strstr(exts, #e) ? EGL_TRUE : EGL_FALSE;\
    if (!e##_flag)\
    {\
        fprintf(stderr, m ? missing_error : missing_warning, #e);\
        failed |= m;\
    }
#define GLX_PROC(r, f, ...)
#include "glx.def"
#undef GLX_EXT
#undef GLX_PROC
    if (failed)
        return 0;

#define GLX_EXT(e, m)       mandatory = m;
#define GLX_PROC(r, f, ...)\
    f = (f##_proc)glXGetProcAddress((GLubyte *)#f);\
    if (!f && mandatory)\
    {\
        fprintf(stderr, entry_error, #f);\
        failed = EGL_TRUE;\
    }
#include "glx.def"
#undef GLX_EXT
#undef GLX_PROC
    if (failed)
        return 0;

    if (eglLinkGL())
        renderable |= EGL_OPENGL_BIT;
    if (eglLinkGLES())
        renderable |= EGL_OPENGL_ES_BIT;
    if (eglLinkGLES2())
        renderable |= EGL_OPENGL_ES2_BIT;
    if (eglLinkGLES3())
        renderable |= EGL_OPENGL_ES3_BIT_KHR;

    return renderable;
}

static EGLSurfaceType eglGetSurfaceType(EGLDisplay dpy, EGLSurface surface)
{
    XPointer type;
    XFindContext((Display *)dpy, (GLXDrawable)surface, egl.surface_type, &type);

    return (EGLSurfaceType)type;
}

#define eglSetError(e, r)\
({\
    { egl_thread.error = (EGLint)(e); }\
    r;\
})

EGLint EGLAPIENTRY eglGetError(void)
{
    EGLint error = egl_thread.error;
    egl_thread.error = EGL_SUCCESS;

    return error;
}

EGLDisplay EGLAPIENTRY eglGetDisplay(EGLNativeDisplayType display_id)
{
    if (display_id == EGL_DEFAULT_DISPLAY)
    {
        if (!egl.default_display)
            egl.default_display = XOpenDisplay(NULL);
        display_id = egl.default_display;
    }

    return (EGLDisplay)display_id ? (EGLDisplay)display_id : EGL_NO_DISPLAY;
}

EGLBoolean EGLAPIENTRY eglInitialize(EGLDisplay dpy, EGLint *major, EGLint *minor)
{
    if (dpy == EGL_NO_DISPLAY)
        return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);

    if (!egl.renderable)
    {
        egl.renderable = eglLink((EGLNativeDisplayType)dpy);
        if (!egl.renderable)
            return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);
        egl.surface_type = XUniqueContext();
    }

    if (major)
        *major = EGL_MAJOR;
    if (minor)
        *minor = EGL_MINOR;

    return EGL_TRUE;
}

EGLBoolean EGLAPIENTRY eglTerminate(EGLDisplay dpy)
{
    if (dpy == EGL_NO_DISPLAY)
        return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);

    if (dpy == egl.default_display)
    {
        XCloseDisplay(egl.default_display);
        egl.default_display = NULL;
    }

    return EGL_TRUE;
}

const char * EGLAPIENTRY eglQueryString(EGLDisplay dpy, EGLint name)
{
    if (dpy == EGL_NO_DISPLAY)
        return eglSetError(EGL_BAD_DISPLAY, NULL);
    if (!egl.renderable)
        return eglSetError(EGL_NOT_INITIALIZED, NULL);

    switch (name)
    {
    case EGL_VENDOR:
        return GLX_EXTENSION_NAME;

    case EGL_VERSION:
        return STRINGIZE(EGL_MAJOR) "." STRINGIZE(EGL_MINOR);

    case EGL_EXTENSIONS:
        return "EGL_KHR_config_attribs EGL_KHR_create_context_robustness EGL_KHR_create_context EGL_KHR_surfaceless_context";

    case EGL_CLIENT_APIS:
        return "OpenGL OpenGL_ES";

    default:
        return eglSetError(EGL_BAD_PARAMETER, NULL);
    }
}

EGLBoolean EGLAPIENTRY eglGetConfigs(EGLDisplay dpy, EGLConfig *configs, EGLint config_size, EGLint *num_config)
{
    const int glx_attribs[] =
    {
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        None
    };

    GLXFBConfig *glx_configs, *pconf;
    int n;

    if (dpy == EGL_NO_DISPLAY)
        return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
    if (!egl.renderable)
        return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);
    if (!num_config)
        return eglSetError(EGL_BAD_PARAMETER, EGL_FALSE);

    glx_configs = glXChooseFBConfig((Display *)dpy, DefaultScreen((Display *)dpy), glx_attribs, &n);
    if (!glx_configs)
        return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);

    if (configs)
    {
        pconf = glx_configs;
        n = config_size < n ? config_size : n;
        *num_config = n;
        while (n--)
            *configs++ = (EGLConfig)*pconf++;
    }
    else
        *num_config = n;

    XFree(glx_configs);

    return EGL_TRUE;
}

EGLBoolean EGLAPIENTRY eglChooseConfig(EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config)
{
    GLXFBConfig *glx_configs, *pconf;
    int glx_attribs[128], *pattr = glx_attribs, n;

    if (dpy == EGL_NO_DISPLAY)
        return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
    if (!egl.renderable)
        return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);
    if (!num_config)
        return eglSetError(EGL_BAD_PARAMETER, EGL_FALSE);

    while (attrib_list && *attrib_list != EGL_NONE && pattr < glx_attribs + countof (glx_attribs))
    {
        switch (*attrib_list++)
        {
        case EGL_BUFFER_SIZE:
            *pattr++ = GLX_BUFFER_SIZE;
            *pattr++ = *attrib_list++;
            break;

        case EGL_ALPHA_SIZE:
            *pattr++ = GLX_ALPHA_SIZE;
            *pattr++ = *attrib_list++;
            break;

        case EGL_BLUE_SIZE:
            *pattr++ = GLX_BLUE_SIZE;
            *pattr++ = *attrib_list++;
            break;

        case EGL_GREEN_SIZE:
            *pattr++ = GLX_GREEN_SIZE;
            *pattr++ = *attrib_list++;
            break;

        case EGL_RED_SIZE:
            *pattr++ = GLX_RED_SIZE;
            *pattr++ = *attrib_list++;
            break;

        case EGL_DEPTH_SIZE:
            *pattr++ = GLX_DEPTH_SIZE;
            *pattr++ = *attrib_list++;
            break;

        case EGL_STENCIL_SIZE:
            *pattr++ = GLX_STENCIL_SIZE;
            *pattr++ = *attrib_list++;
            break;

        case EGL_CONFIG_CAVEAT:
            *pattr++ = GLX_CONFIG_CAVEAT;
            switch (*attrib_list++)
            {
            case EGL_SLOW_CONFIG:
                *pattr++ = GLX_SLOW_CONFIG;
                break;

            case EGL_NON_CONFORMANT_CONFIG:
                *pattr++ = GLX_NON_CONFORMANT_CONFIG;
                break;

            default:
                *pattr++ = GLX_NONE;
                break;
            }
            break;

        case EGL_CONFIG_ID:
            *pattr++ = GLX_FBCONFIG_ID;
            *pattr++ = *attrib_list++;
            break;

        case EGL_LEVEL:
            *pattr++ = GLX_LEVEL;
            *pattr++ = *attrib_list++;
            break;

        case EGL_NATIVE_RENDERABLE:
            *pattr++ = GLX_X_RENDERABLE;
            *pattr++ = *attrib_list++;
            break;

        case EGL_NATIVE_VISUAL_TYPE:
            *pattr++ = GLX_X_VISUAL_TYPE;
            *pattr++ = *attrib_list++;
            break;

        case EGL_SAMPLES:
            *pattr++ = GLX_SAMPLES;
            *pattr++ = *attrib_list++;
            break;

        case EGL_SAMPLE_BUFFERS:
            *pattr++ = GLX_SAMPLE_BUFFERS;
            *pattr++ = *attrib_list++;
            break;

        case EGL_SURFACE_TYPE:
            *pattr++ = GLX_DRAWABLE_TYPE;
            *pattr++ =
                (*attrib_list & EGL_PBUFFER_BIT ? GLX_PBUFFER_BIT : 0) |
                (*attrib_list & EGL_PIXMAP_BIT ? GLX_PIXMAP_BIT : 0) |
                (*attrib_list & EGL_WINDOW_BIT ? GLX_WINDOW_BIT : 0);
            if (*attrib_list & (EGL_PBUFFER_BIT | EGL_WINDOW_BIT))
            {
                *pattr++ = GLX_DOUBLEBUFFER;
                *pattr++ = True;
            }
            attrib_list++;
            break;

        case EGL_TRANSPARENT_TYPE:
            *pattr++ = GLX_TRANSPARENT_TYPE;
            switch (*attrib_list++)
            {
            case EGL_TRANSPARENT_RGB:
                *pattr++ = GLX_TRANSPARENT_RGB;
                break;

            default:
                *pattr++ = GLX_NONE;
                break;
            }
            break;

        case EGL_TRANSPARENT_BLUE_VALUE:
            *pattr++ = GLX_TRANSPARENT_BLUE_VALUE;
            *pattr++ = *attrib_list++;
            break;

        case EGL_TRANSPARENT_GREEN_VALUE:
            *pattr++ = GLX_TRANSPARENT_GREEN_VALUE;
            *pattr++ = *attrib_list++;
            break;

        case EGL_TRANSPARENT_RED_VALUE:
            *pattr++ = GLX_TRANSPARENT_RED_VALUE;
            *pattr++ = *attrib_list++;
            break;

        case EGL_BIND_TO_TEXTURE_RGB:
            *pattr++ = GLX_BIND_TO_TEXTURE_RGB_EXT;
            *pattr++ = *attrib_list++;
            break;

        case EGL_BIND_TO_TEXTURE_RGBA:
            *pattr++ = GLX_BIND_TO_TEXTURE_RGBA_EXT;
            *pattr++ = *attrib_list++;
            break;

        case EGL_MIN_SWAP_INTERVAL:
            attrib_list++;
            break;

        case EGL_MAX_SWAP_INTERVAL:
            attrib_list++;
            break;

        case EGL_LUMINANCE_SIZE:
            if (*attrib_list++ != 0)
            {
                *num_config = 0;
                return EGL_TRUE;
            }
            break;

        case EGL_ALPHA_MASK_SIZE:
            if (*attrib_list++ != 0)
            {
                *num_config = 0;
                return EGL_TRUE;
            }
            break;

        case EGL_COLOR_BUFFER_TYPE:
            if (*attrib_list++ != EGL_RGB_BUFFER)
            {
                *num_config = 0;
                return EGL_TRUE;
            }
            break;

        case EGL_RENDERABLE_TYPE:
            attrib_list++;
            break;

        case EGL_MATCH_NATIVE_PIXMAP:
            attrib_list++;
            break;

        case EGL_CONFORMANT:
            attrib_list++;
            break;

        case EGL_MAX_PBUFFER_WIDTH:
        case EGL_MAX_PBUFFER_PIXELS:
        case EGL_MAX_PBUFFER_HEIGHT:
        case EGL_NATIVE_VISUAL_ID:
            attrib_list++;
            break;

        default:
            return eglSetError(EGL_BAD_ATTRIBUTE, EGL_FALSE);
        }
    }
    *pattr++ = GLX_RENDER_TYPE;
    *pattr++ = GLX_RGBA_BIT;
    *pattr = None;

    glx_configs = glXChooseFBConfig((Display *)dpy, DefaultScreen((Display *)dpy), glx_attribs, &n);
    if (!glx_configs)
        return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);

    if (configs)
    {
        pconf = glx_configs;
        n = config_size < n ? config_size : n;
        *num_config = n;
        while (n--)
            *configs++ = (EGLConfig)*pconf++;
    }
    else
        *num_config = n;

    XFree(glx_configs);

    return EGL_TRUE;
}

EGLBoolean EGLAPIENTRY eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value)
{
    int glx_attrib;

    if (dpy == EGL_NO_DISPLAY)
        return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
    if (!egl.renderable)
        return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);

    switch (attribute)
    {
    case EGL_BUFFER_SIZE:
        glx_attrib = GLX_BUFFER_SIZE;
        break;

    case EGL_ALPHA_SIZE:
        glx_attrib = GLX_ALPHA_SIZE;
        break;

    case EGL_BLUE_SIZE:
        glx_attrib = GLX_BLUE_SIZE;
        break;

    case EGL_GREEN_SIZE:
        glx_attrib = GLX_GREEN_SIZE;
        break;

    case EGL_RED_SIZE:
        glx_attrib = GLX_RED_SIZE;
        break;

    case EGL_DEPTH_SIZE:
        glx_attrib = GLX_DEPTH_SIZE;
        break;

    case EGL_STENCIL_SIZE:
        glx_attrib = GLX_STENCIL_SIZE;
        break;

    case EGL_CONFIG_CAVEAT:
        glx_attrib = GLX_CONFIG_CAVEAT;
        break;

    case EGL_CONFIG_ID:
        glx_attrib = GLX_FBCONFIG_ID;
        break;

    case EGL_LEVEL:
        glx_attrib = GLX_LEVEL;
        break;

    case EGL_MAX_PBUFFER_HEIGHT:
        glx_attrib = GLX_MAX_PBUFFER_HEIGHT;
        break;

    case EGL_MAX_PBUFFER_PIXELS:
        glx_attrib = GLX_MAX_PBUFFER_PIXELS;
        break;

    case EGL_MAX_PBUFFER_WIDTH:
        glx_attrib = GLX_MAX_PBUFFER_WIDTH;
        break;

    case EGL_NATIVE_RENDERABLE:
        glx_attrib = GLX_X_RENDERABLE;
        break;

    case EGL_NATIVE_VISUAL_ID:
        glx_attrib = GLX_VISUAL_ID;
        break;

    case EGL_NATIVE_VISUAL_TYPE:
        glx_attrib = GLX_X_VISUAL_TYPE;
        break;

    case EGL_SAMPLES:
        glx_attrib = GLX_SAMPLES;
        break;

    case EGL_SAMPLE_BUFFERS:
        glx_attrib = GLX_SAMPLE_BUFFERS;
        break;

    case EGL_SURFACE_TYPE:
        glx_attrib = GLX_DRAWABLE_TYPE;
        break;

    case EGL_TRANSPARENT_TYPE:
        glx_attrib = GLX_TRANSPARENT_TYPE;
        break;

    case EGL_TRANSPARENT_BLUE_VALUE:
        glx_attrib = GLX_TRANSPARENT_BLUE_VALUE;
        break;

    case EGL_TRANSPARENT_GREEN_VALUE:
        glx_attrib = GLX_TRANSPARENT_GREEN_VALUE;
        break;

    case EGL_TRANSPARENT_RED_VALUE:
        glx_attrib = GLX_TRANSPARENT_RED_VALUE;
        break;

    case EGL_BIND_TO_TEXTURE_RGB:
        glx_attrib = GLX_BIND_TO_TEXTURE_RGB_EXT;
        break;

    case EGL_BIND_TO_TEXTURE_RGBA:
        glx_attrib = GLX_BIND_TO_TEXTURE_RGBA_EXT;
        break;

    case EGL_MIN_SWAP_INTERVAL:
        *value = 0;
        return EGL_TRUE;

    case EGL_MAX_SWAP_INTERVAL:
        *value = GLX_SGI_swap_control_flag ? 1 : 0;
        return EGL_TRUE;

    case EGL_LUMINANCE_SIZE:
        *value = 0;
        return EGL_TRUE;

    case EGL_ALPHA_MASK_SIZE:
        *value = 0;
        return EGL_TRUE;

    case EGL_COLOR_BUFFER_TYPE:
        *value = EGL_RGB_BUFFER;
        return EGL_TRUE;

    case EGL_RENDERABLE_TYPE:
        *value = egl.renderable;
        return EGL_TRUE;

    case EGL_CONFORMANT:
        *value = egl.renderable;
        return EGL_TRUE;

    default:
        return eglSetError(EGL_BAD_ATTRIBUTE, EGL_FALSE);
    }

    if (glXGetFBConfigAttrib((Display *)dpy, (GLXFBConfig)config, glx_attrib, value) != Success)
        return eglSetError(EGL_BAD_CONFIG, EGL_FALSE);

    switch (glx_attrib)
    {
    case GLX_CONFIG_CAVEAT:
        switch (*value)
        {
        case GLX_SLOW_CONFIG:
            *value = EGL_SLOW_CONFIG;
            break;

        case GLX_NON_CONFORMANT_CONFIG:
            *value = EGL_NON_CONFORMANT_CONFIG;
            break;

        default:
            *value = EGL_NONE;
            break;
        }
        break;

    case GLX_DRAWABLE_TYPE:
        *value =
            (*value & GLX_PBUFFER_BIT ? EGL_PBUFFER_BIT : 0) |
            (*value & GLX_PIXMAP_BIT ? EGL_PIXMAP_BIT : 0) |
            (*value & GLX_WINDOW_BIT ? EGL_WINDOW_BIT : 0);
        break;

    case GLX_TRANSPARENT_TYPE:
        switch (*value)
        {
        case GLX_TRANSPARENT_RGB:
            *value = EGL_TRANSPARENT_RGB;
            break;

        default:
            *value = EGL_NONE;
            break;
        }
        break;
    }

    return EGL_TRUE;
}

EGLSurface EGLAPIENTRY eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list)
{
    GLXWindow glx_win;

    if (dpy == EGL_NO_DISPLAY)
        return eglSetError(EGL_BAD_DISPLAY, EGL_NO_SURFACE);
    if (!egl.renderable)
        return eglSetError(EGL_NOT_INITIALIZED, EGL_NO_SURFACE);

    while (attrib_list && *attrib_list != EGL_NONE)
    {
        switch (*attrib_list++)
        {
        case EGL_RENDER_BUFFER:
        case EGL_VG_COLORSPACE:
        case EGL_VG_ALPHA_FORMAT:
            attrib_list++;
            break;

        default:
            return eglSetError(EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE);
        }
    }

    glx_win = glXCreateWindow((Display *)dpy, (GLXFBConfig)config, win, NULL);
    if (glx_win)
    {
        XSaveContext((Display *)dpy, glx_win, egl.surface_type, (XPointer)EGL_WINDOW);
        return (EGLSurface)glx_win;
    }
    else
        return eglSetError(EGL_BAD_MATCH, EGL_NO_SURFACE);
}

EGLSurface EGLAPIENTRY eglCreatePbufferSurface(EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list)
{
    GLXPbuffer glx_pbuffer;
    int glx_attribs[32], *pattr = glx_attribs;

    if (dpy == EGL_NO_DISPLAY)
        return eglSetError(EGL_BAD_DISPLAY, EGL_NO_SURFACE);
    if (!egl.renderable)
        return eglSetError(EGL_NOT_INITIALIZED, EGL_NO_SURFACE);

    while (attrib_list && *attrib_list != EGL_NONE && pattr < glx_attribs + countof (glx_attribs))
    {
        switch (*attrib_list++)
        {
        case EGL_HEIGHT:
            *pattr++ = GLX_PBUFFER_HEIGHT;
            *pattr++ = *attrib_list++;
            break;

        case EGL_WIDTH:
            *pattr++ = GLX_PBUFFER_WIDTH;
            *pattr++ = *attrib_list++;
            break;

        case EGL_LARGEST_PBUFFER:
            *pattr++ = GLX_LARGEST_PBUFFER;
            *pattr++ = *attrib_list++;
            break;

        case EGL_TEXTURE_FORMAT:
            attrib_list++;
            break;

        case EGL_TEXTURE_TARGET:
            attrib_list++;
            break;

        case EGL_MIPMAP_TEXTURE:
            attrib_list++;
            break;

        case EGL_VG_COLORSPACE:
        case EGL_VG_ALPHA_FORMAT:
            attrib_list++;
            break;

        default:
            return eglSetError(EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE);
        }
    }
    *pattr = None;

    glx_pbuffer = glXCreatePbuffer((Display *)dpy, (GLXFBConfig)config, glx_attribs);
    if (glx_pbuffer)
    {
        XSaveContext((Display *)dpy, glx_pbuffer, egl.surface_type, (XPointer)EGL_PBUFFER);
        return (EGLSurface)glx_pbuffer;
    }
    else
        return eglSetError(EGL_BAD_MATCH, EGL_NO_SURFACE);
}

EGLSurface EGLAPIENTRY eglCreatePixmapSurface(EGLDisplay dpy, EGLConfig config, EGLNativePixmapType pixmap, const EGLint *attrib_list)
{
    GLXPixmap glx_pixmap;

    if (dpy == EGL_NO_DISPLAY)
        return eglSetError(EGL_BAD_DISPLAY, EGL_NO_SURFACE);
    if (!egl.renderable)
        return eglSetError(EGL_NOT_INITIALIZED, EGL_NO_SURFACE);

    while (attrib_list && *attrib_list != EGL_NONE)
    {
        switch (*attrib_list++)
        {
        case EGL_VG_COLORSPACE:
        case EGL_VG_ALPHA_FORMAT:
            attrib_list++;
            break;

        default:
            return eglSetError(EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE);
        }
    }

    glx_pixmap = glXCreatePixmap((Display *)dpy, (GLXFBConfig)config, pixmap, NULL);
    if (glx_pixmap)
    {
        XSaveContext((Display *)dpy, glx_pixmap, egl.surface_type, (XPointer)EGL_PIXMAP);
        return (EGLSurface)glx_pixmap;
    }
    else
        return eglSetError(EGL_BAD_MATCH, EGL_NO_SURFACE);
}

EGLBoolean EGLAPIENTRY eglDestroySurface(EGLDisplay dpy, EGLSurface surface)
{
    EGLSurfaceType type;

    if (dpy == EGL_NO_DISPLAY)
        return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
    if (!egl.renderable)
        return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);
    if (surface == EGL_NO_SURFACE)
        return eglSetError(EGL_BAD_SURFACE, EGL_FALSE);

    type = eglGetSurfaceType(dpy, surface);
    XDeleteContext((Display *)dpy, (XID)surface, egl.surface_type);
    switch (type)
    {
    case EGL_PBUFFER:
        glXDestroyPbuffer((Display *)dpy, (GLXDrawable)surface);
        break;

    case EGL_PIXMAP:
        glXDestroyPixmap((Display *)dpy, (GLXDrawable)surface);
        break;

    case EGL_WINDOW:
        glXDestroyWindow((Display *)dpy, (GLXDrawable)surface);
        break;

    default:
        return eglSetError(EGL_BAD_SURFACE, EGL_FALSE);
    }

    return EGL_TRUE;
}

EGLBoolean EGLAPIENTRY eglQuerySurface(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint *value)
{
    int glx_attrib;

    if (dpy == EGL_NO_DISPLAY)
        return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
    if (!egl.renderable)
        return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);
    if (surface == EGL_NO_SURFACE)
        return eglSetError(EGL_BAD_SURFACE, EGL_FALSE);

    switch (attribute)
    {
    case EGL_CONFIG_ID:
        glx_attrib = GLX_FBCONFIG_ID;
        break;

    case EGL_WIDTH:
        glx_attrib = GLX_WIDTH;
        break;

    case EGL_HEIGHT:
        glx_attrib = GLX_HEIGHT;
        break;

    case EGL_LARGEST_PBUFFER:
        glx_attrib = GLX_LARGEST_PBUFFER;
        break;

    case EGL_HORIZONTAL_RESOLUTION:
    case EGL_VERTICAL_RESOLUTION:
    case EGL_PIXEL_ASPECT_RATIO:
        *value = EGL_UNKNOWN;
        return EGL_TRUE;

    case EGL_MULTISAMPLE_RESOLVE:
        *value = EGL_MULTISAMPLE_RESOLVE_DEFAULT;
        return EGL_TRUE;

    case EGL_RENDER_BUFFER:
        *value = eglGetSurfaceType(dpy, surface) == EGL_PIXMAP ? EGL_SINGLE_BUFFER : EGL_BACK_BUFFER;
        return EGL_TRUE;

    case EGL_SWAP_BEHAVIOR:
        *value = EGL_BUFFER_DESTROYED;
        return EGL_TRUE;

    case EGL_TEXTURE_FORMAT:
    case EGL_TEXTURE_TARGET:
    case EGL_MIPMAP_TEXTURE:
    case EGL_MIPMAP_LEVEL:
        return EGL_TRUE;

    default:
        return eglSetError(EGL_BAD_ATTRIBUTE, EGL_FALSE);
    }

    glXQueryDrawable((Display *)dpy, (GLXDrawable)surface, glx_attrib, (unsigned int *)value);

    return EGL_TRUE;
}

EGLBoolean EGLAPIENTRY eglBindAPI(EGLenum api)
{
    if (api == EGL_OPENGL_API || api == EGL_OPENGL_ES_API)
    {
        egl_thread.api = api;
        return EGL_TRUE;
    }
    else
        return eglSetError(EGL_BAD_PARAMETER, EGL_FALSE);
}

EGLenum EGLAPIENTRY eglQueryAPI(void)
{
    return egl_thread.api;
}

EGLBoolean EGLAPIENTRY eglWaitClient(void)
{
    glXWaitGL();

    return EGL_TRUE;
}

EGLBoolean EGLAPIENTRY eglReleaseThread(void)
{
    eglMakeCurrent(eglGetCurrentDisplay(), EGL_NO_CONTEXT, EGL_NO_SURFACE, EGL_NO_SURFACE);
    eglBindAPI(EGL_OPENGL_ES_API);
    eglSetError(EGL_SUCCESS, 0);

    return EGL_TRUE;
}

EGLSurface EGLAPIENTRY eglCreatePbufferFromClientBuffer(EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint *attrib_list)
{
    UNUSED(buftype);
    UNUSED(buffer);
    UNUSED(config);
    UNUSED(attrib_list);

    if (dpy == EGL_NO_DISPLAY)
        return eglSetError(EGL_BAD_DISPLAY, EGL_NO_SURFACE);
    if (!egl.renderable)
        return eglSetError(EGL_NOT_INITIALIZED, EGL_NO_SURFACE);

    return eglSetError(EGL_BAD_PARAMETER, EGL_NO_SURFACE);
}

EGLBoolean EGLAPIENTRY eglSurfaceAttrib(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value)
{
    if (dpy == EGL_NO_DISPLAY)
        return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
    if (!egl.renderable)
        return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);
    if (surface == EGL_NO_SURFACE)
        return eglSetError(EGL_BAD_SURFACE, EGL_FALSE);

    switch (attribute)
    {
    case EGL_MIPMAP_LEVEL:
        return EGL_TRUE;

    case EGL_MULTISAMPLE_RESOLVE:
        return value == EGL_MULTISAMPLE_RESOLVE_DEFAULT ? EGL_TRUE : eglSetError(EGL_BAD_MATCH, EGL_FALSE);

    case EGL_SWAP_BEHAVIOR:
        return value == EGL_BUFFER_DESTROYED ? EGL_TRUE : eglSetError(EGL_BAD_MATCH, EGL_FALSE);

    default:
        return eglSetError(EGL_BAD_ATTRIBUTE, EGL_FALSE);
    }
}

EGLBoolean EGLAPIENTRY eglBindTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
    int glx_buffer;

    if (dpy == EGL_NO_DISPLAY)
        return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
    if (!egl.renderable)
        return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);
    if (surface == EGL_NO_SURFACE)
        return eglSetError(EGL_BAD_SURFACE, EGL_FALSE);
    /*if (!GLX_ARB_render_texture_flag)
        return eglSetError(EGL_BAD_MATCH, EGL_FALSE);*/

    switch (buffer)
    {
    case EGL_BACK_BUFFER:
        glx_buffer = GLX_BACK_EXT;
        break;

    default:
        return eglSetError(EGL_BAD_PARAMETER, EGL_FALSE);
    }

    glXBindTexImageARB((Display *)dpy, (GLXDrawable)surface, glx_buffer);

    return EGL_TRUE;
}

EGLBoolean EGLAPIENTRY eglReleaseTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
    int glx_buffer;

    if (dpy == EGL_NO_DISPLAY)
        return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
    if (!egl.renderable)
        return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);
    if (surface == EGL_NO_SURFACE)
        return eglSetError(EGL_BAD_SURFACE, EGL_FALSE);
    /*if (!GLX_ARB_render_texture_flag)
        return eglSetError(EGL_BAD_MATCH, EGL_FALSE);*/

    switch (buffer)
    {
    case EGL_BACK_BUFFER:
        glx_buffer = GLX_BACK_EXT;
        break;

    default:
        return eglSetError(EGL_BAD_PARAMETER, EGL_FALSE);
    }

    glXReleaseTexImageARB((Display *)dpy, (GLXDrawable)surface, glx_buffer);

    return EGL_TRUE;
}

EGLBoolean EGLAPIENTRY eglSwapInterval(EGLDisplay dpy, EGLint interval)
{
    if (dpy == EGL_NO_DISPLAY)
        return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
    if (!egl.renderable)
        return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);

    return GLX_SGI_swap_control_flag && glXSwapIntervalSGI(interval) == 0 ? EGL_TRUE : eglSetError(EGL_BAD_PARAMETER, EGL_FALSE);
}

EGLContext EGLAPIENTRY eglCreateContext(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list)
{
    int glx_attribs[32], *pattr = glx_attribs, major = 1, minor = 0, flags = 0, profile = 0, robustness = 0;
    GLXContext glx_ctx;

    if (dpy == EGL_NO_DISPLAY)
        return eglSetError(EGL_BAD_DISPLAY, EGL_NO_CONTEXT);
    if (!egl.renderable)
        return eglSetError(EGL_NOT_INITIALIZED, EGL_NO_CONTEXT);

    if (egl_thread.api == EGL_OPENGL_ES_API)
    {
        while (attrib_list && *attrib_list != EGL_NONE && pattr < glx_attribs + countof (glx_attribs))
        {
            switch (*attrib_list++)
            {
            case EGL_CONTEXT_MAJOR_VERSION_KHR:
                major = *attrib_list++;
                break;

            case EGL_CONTEXT_MINOR_VERSION_KHR:
                minor = *attrib_list++;
                break;

            default:
                return eglSetError(EGL_BAD_ATTRIBUTE, EGL_NO_CONTEXT);
            }
        }
        if ((major == 1 && !(egl.renderable & EGL_OPENGL_ES_BIT)) || (major == 2 && !(egl.renderable & EGL_OPENGL_ES2_BIT)) || (major == 3 && !(egl.renderable & EGL_OPENGL_ES3_BIT_KHR)) || major < 1 || major > 3)
            return eglSetError(EGL_BAD_CONFIG, EGL_NO_CONTEXT);

        if (GLX_EXT_create_context_es2_profile_flag)
            profile |= GLX_CONTEXT_ES2_PROFILE_BIT_EXT;
    }
    else if (egl_thread.api == EGL_OPENGL_API)
    {
        while (attrib_list && *attrib_list != EGL_NONE && pattr < glx_attribs + countof (glx_attribs))
        {
            switch (*attrib_list++)
            {
            case EGL_CONTEXT_MAJOR_VERSION_KHR:
                major = *attrib_list++;
                break;

            case EGL_CONTEXT_MINOR_VERSION_KHR:
                minor = *attrib_list++;
                break;

            case EGL_CONTEXT_FLAGS_KHR:
                if (*attrib_list & EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR)
                    flags |= GLX_CONTEXT_DEBUG_BIT_ARB;
                if (*attrib_list & EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR)
                    flags |= GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
                if (*attrib_list & EGL_CONTEXT_OPENGL_ROBUST_ACCESS_BIT_KHR && GLX_ARB_create_context_robustness_flag)
                    flags |= GLX_CONTEXT_ROBUST_ACCESS_BIT_ARB;
                attrib_list++;
                break;

            case EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR:
                if (*attrib_list & EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR)
                    profile |= GLX_CONTEXT_CORE_PROFILE_BIT_ARB;
                if (*attrib_list & EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT_KHR)
                    profile |= GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
                attrib_list++;
                break;

            case EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY_KHR:
                switch (*attrib_list++)
                {
                case EGL_NO_RESET_NOTIFICATION_KHR:
                    robustness = GLX_NO_RESET_NOTIFICATION_ARB;
                    break;

                case EGL_LOSE_CONTEXT_ON_RESET_KHR:
                    robustness = GLX_LOSE_CONTEXT_ON_RESET_ARB;
                    break;

                default:
                    return eglSetError(EGL_BAD_ATTRIBUTE, EGL_NO_CONTEXT);
                }
                break;

            default:
                return eglSetError(EGL_BAD_ATTRIBUTE, EGL_NO_CONTEXT);
            }
        }
    }
    else if (egl_thread.api == EGL_OPENVG_API)
    {
        while (attrib_list && *attrib_list != EGL_NONE && pattr < glx_attribs + countof (glx_attribs))
        {
            switch (*attrib_list++)
            {
            default:
                return eglSetError(EGL_BAD_ATTRIBUTE, EGL_NO_CONTEXT);
            }
        }
    }

    if (GLX_ARB_create_context_flag)
    {
        *pattr++ = GLX_RENDER_TYPE;
        *pattr++ = GLX_RGBA_TYPE;
        if (major != 1 || minor != 0)
        {
            *pattr++ = GLX_CONTEXT_MAJOR_VERSION_ARB;
            *pattr++ = major;
            *pattr++ = GLX_CONTEXT_MINOR_VERSION_ARB;
            *pattr++ = minor;
        }
        if (flags)
        {
            *pattr++ = GLX_CONTEXT_FLAGS_ARB;
            *pattr++ = flags;
        }
        if (profile && GLX_ARB_create_context_profile_flag)
        {
            *pattr++ = GLX_CONTEXT_PROFILE_MASK_ARB;
            *pattr++ = profile;
        }
        if (robustness && GLX_ARB_create_context_robustness_flag)
        {
            *pattr++ = GLX_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB;
            *pattr++ = robustness;
        }
        *pattr = None;

        glx_ctx = glXCreateContextAttribsARB((Display *)dpy, (GLXFBConfig)config, (GLXContext)share_context, True, glx_attribs);
    }
    else
        glx_ctx = glXCreateNewContext((Display *)dpy, (GLXFBConfig)config, GLX_RGBA_TYPE, (GLXContext)share_context, True);

    return (EGLContext)glx_ctx ? (EGLContext)glx_ctx : eglSetError(EGL_BAD_CONFIG, EGL_NO_CONTEXT);
}

EGLBoolean EGLAPIENTRY eglDestroyContext(EGLDisplay dpy, EGLContext ctx)
{
    if (dpy == EGL_NO_DISPLAY)
        return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
    if (!egl.renderable)
        return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);
    if (ctx == EGL_NO_CONTEXT)
        return eglSetError(EGL_BAD_CONTEXT, EGL_FALSE);

    glXDestroyContext((Display *)dpy, (GLXContext)ctx);

    return EGL_TRUE;
}

EGLBoolean EGLAPIENTRY eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{
    if (dpy == EGL_NO_DISPLAY)
        return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
    if (!egl.renderable && (ctx != EGL_NO_CONTEXT || draw != EGL_NO_SURFACE || read != EGL_NO_SURFACE))
        return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);
    if (ctx == EGL_NO_CONTEXT && (draw != EGL_NO_SURFACE || read != EGL_NO_SURFACE))
        return eglSetError(EGL_BAD_MATCH, EGL_FALSE);
    if ((draw == EGL_NO_SURFACE && read != EGL_NO_SURFACE) || (draw != EGL_NO_SURFACE && read == EGL_NO_SURFACE))
        return eglSetError(EGL_BAD_MATCH, EGL_FALSE);

    return glXMakeContextCurrent((Display *)dpy, (GLXDrawable)draw, (GLXDrawable)read, (GLXContext)ctx) ? EGL_TRUE : eglSetError(EGL_BAD_MATCH, EGL_FALSE);
}

EGLContext EGLAPIENTRY eglGetCurrentContext(void)
{
    return (EGLContext)glXGetCurrentContext() ? (EGLContext)glXGetCurrentContext() : EGL_NO_CONTEXT;
}

EGLSurface EGLAPIENTRY eglGetCurrentSurface(EGLint readdraw)
{
    switch (readdraw)
    {
    case EGL_DRAW:
        return (EGLSurface)glXGetCurrentDrawable() ? (EGLSurface)glXGetCurrentDrawable() : EGL_NO_SURFACE;

    case EGL_READ:
        return (EGLSurface)glXGetCurrentReadDrawable() ? (EGLSurface)glXGetCurrentReadDrawable() : EGL_NO_SURFACE;

    default:
        return eglSetError(EGL_BAD_PARAMETER, EGL_NO_SURFACE);
    }
}

EGLDisplay EGLAPIENTRY eglGetCurrentDisplay(void)
{
    return (EGLDisplay)glXGetCurrentDisplay() ? (EGLDisplay)glXGetCurrentDisplay() : EGL_NO_DISPLAY;
}

EGLBoolean EGLAPIENTRY eglQueryContext(EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint *value)
{
    int glx_attrib;
    GLXDrawable glx_drawable;

    if (dpy == EGL_NO_DISPLAY)
        return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
    if (!egl.renderable)
        return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);
    if (ctx == EGL_NO_CONTEXT)
        return eglSetError(EGL_BAD_CONTEXT, EGL_FALSE);

    switch (attribute)
    {
    case EGL_CONFIG_ID:
        glx_attrib = GLX_FBCONFIG_ID;
        break;

    case EGL_CONTEXT_CLIENT_TYPE:
        *value = EGL_UNKNOWN;
        return EGL_TRUE;

    case EGL_CONTEXT_CLIENT_VERSION:
        *value = EGL_UNKNOWN;
        return EGL_TRUE;

    case EGL_RENDER_BUFFER:
        glx_drawable = glXGetCurrentDrawable();
        *value = glx_drawable != None ? eglGetSurfaceType(dpy, (EGLSurface)glx_drawable) == EGL_PIXMAP ? EGL_SINGLE_BUFFER : EGL_BACK_BUFFER : EGL_NONE;
        return EGL_TRUE;

    default:
        return eglSetError(EGL_BAD_ATTRIBUTE, EGL_FALSE);
    }

    return glXQueryContext((Display *)dpy, (GLXContext)ctx, glx_attrib, value) == Success ? EGL_TRUE : eglSetError(EGL_BAD_CONTEXT, EGL_FALSE);
}

EGLBoolean EGLAPIENTRY eglWaitGL(void)
{
    EGLenum api = eglQueryAPI();
    eglBindAPI(EGL_OPENGL_ES_API);
    eglWaitClient();
    eglBindAPI(api);

    return EGL_TRUE;
}

EGLBoolean EGLAPIENTRY eglWaitNative(EGLint engine)
{
    if (engine != EGL_CORE_NATIVE_ENGINE)
        return eglSetError(EGL_BAD_PARAMETER, EGL_FALSE);

    glXWaitX();

    return EGL_TRUE;
}

EGLBoolean EGLAPIENTRY eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
    if (dpy == EGL_NO_DISPLAY)
        return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
    if (!egl.renderable)
        return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);
    if (surface == EGL_NO_SURFACE)
        return eglSetError(EGL_BAD_SURFACE, EGL_FALSE);

    glXSwapBuffers((Display *)dpy, (GLXDrawable)surface);

    return EGL_TRUE;
}

EGLBoolean EGLAPIENTRY eglCopyBuffers(EGLDisplay dpy, EGLSurface surface, EGLNativePixmapType target)
{
    UNUSED(target);

    if (dpy == EGL_NO_DISPLAY)
        return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
    if (!egl.renderable)
        return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);
    if (surface == EGL_NO_SURFACE)
        return eglSetError(EGL_BAD_SURFACE, EGL_FALSE);

    /* TODO */

    return EGL_TRUE;
}

__eglMustCastToProperFunctionPointerType EGLAPIENTRY eglGetProcAddress(const char *procname)
{
    return glXGetProcAddress((GLubyte *)procname);
}
