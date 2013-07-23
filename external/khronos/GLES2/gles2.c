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

#if defined(_WIN32) && !defined(__SCITECH_SNAP__)
#   define GL_APICALL __declspec(dllexport)
#elif defined(__WINSCW__) || defined(__SYMBIAN32__)
#   define GL_APICALL EXPORT_C
#else
#   define GL_APICALL
#endif

#include <stdio.h>
#include <string.h>
#include <GLES2/gl2.h>
#include <EGL/eglplatform.h>

void * EGLAPIENTRY eglGetProcAddress(const char *procname);

#define UNUSED(x)           (void)(x)

#define GL_PROC(r, f, ...)  typedef r GL_APIENTRY (*f##_proc)(__VA_ARGS__);
#include "gles2.def"
#undef GL_PROC

GL_APICALL struct
{
#define GL_PROC(r, f, ...)  f##_proc f;
#include "gles2.def"
#undef GL_PROC
} gles2;

#undef GL_APICALL
#define GL_APICALL KHRONOS_APICALL

#define GL_IMPL(f, r)       (gles2.f = gles2.f ? gles2.f : r)
#define GL_IMPL_EXT(f, p)   GL_IMPL(f, (f##_proc)eglGetProcAddress(#f#p))
#define GL_IMPL_FALLBACK(f) GL_IMPL(f, f##_fallback)

typedef double GLdouble, GLclampd;

GL_APICALL void GL_APIENTRY glClearDepth(GLclampd depth);
GL_APICALL void GL_APIENTRY glDepthRange(GLclampd znear, GLclampd zfar);

static void GL_APIENTRY glClearDepthf_fallback(GLclampf depth)
{
    glClearDepth(depth);
}

static void GL_APIENTRY glDepthRangef_fallback(GLclampf znear, GLclampf zfar)
{
    glDepthRange(znear, zfar);
}

static void GL_APIENTRY glGetShaderPrecisionFormat_fallback(GLenum shader_type, GLenum precision_type, GLint *range, GLint *precision)
{
    UNUSED(shader_type);

    switch (precision_type)
    {
    case GL_LOW_FLOAT:
        range[0] = -62;
        range[1] = 62;
        *precision = -16;
        break;

    case GL_MEDIUM_FLOAT:
        range[0] = -14;
        range[1] = 14;
        *precision = -10;
        break;

    case GL_HIGH_FLOAT:
        range[0] = -8;
        range[1] = 1;
        *precision = -8;
        break;

    case GL_LOW_INT:
        range[0] = -62;
        range[1] = 62;
        *precision = -16;
        break;

    case GL_MEDIUM_INT:
        range[0] = -14;
        range[1] = 14;
        *precision = -10;
        break;

    case GL_HIGH_INT:
        range[0] = -8;
        range[1] = 1;
        *precision = -8;
        break;
    }
}

static void GL_APIENTRY glReleaseShaderCompiler_fallback(void)
{
    /* NOP */
}

static void GL_APIENTRY glShaderBinary_fallback(GLsizei n, const GLuint *shaders, GLenum binaryformat, const void *binary, GLsizei length)
{
    UNUSED(n);
    UNUSED(shaders);
    UNUSED(binaryformat);
    UNUSED(binary);
    UNUSED(length);

    /* NOP */
}

GLboolean eglLinkGLES2(void)
{
    const char *entry_warning = "[WARNING] Cannot retrieve GLES2 entry point: %s\n";
    GLboolean supported = GL_TRUE;

    memset(&gles2, 0, sizeof (gles2));

#define GL_PROC(r, f, ...)  gles2.f = (f##_proc)eglGetProcAddress(#f);
#include "gles2.def"
#undef GL_PROC

    /* GL_ARB_framebuffer_object */
    GL_IMPL_EXT(glIsRenderbuffer, ARB);
    GL_IMPL_EXT(glBindRenderbuffer, ARB);
    GL_IMPL_EXT(glDeleteRenderbuffers, ARB);
    GL_IMPL_EXT(glGenRenderbuffers, ARB);
    GL_IMPL_EXT(glRenderbufferStorage, ARB);
    GL_IMPL_EXT(glGetRenderbufferParameteriv, ARB);
    GL_IMPL_EXT(glIsFramebuffer, ARB);
    GL_IMPL_EXT(glBindFramebuffer, ARB);
    GL_IMPL_EXT(glDeleteFramebuffers, ARB);
    GL_IMPL_EXT(glGenFramebuffers, ARB);
    GL_IMPL_EXT(glCheckFramebufferStatus, ARB);
    GL_IMPL_EXT(glFramebufferTexture2D, ARB);
    GL_IMPL_EXT(glFramebufferRenderbuffer, ARB);
    GL_IMPL_EXT(glGetFramebufferAttachmentParameteriv, ARB);
    GL_IMPL_EXT(glGenerateMipmap, ARB);

    /* GL_ARB_ES2_compatibility */
    GL_IMPL_EXT(glReleaseShaderCompiler, ARB);
    GL_IMPL_EXT(glShaderBinary, ARB);
    GL_IMPL_EXT(glGetShaderPrecisionFormat, ARB);
    GL_IMPL_EXT(glDepthRangef, ARB);
    GL_IMPL_EXT(glClearDepthf, ARB);

    /* For drivers not providing GL_ARB_ES2_compatibility... */
    GL_IMPL_FALLBACK(glReleaseShaderCompiler);
    GL_IMPL_FALLBACK(glShaderBinary);
    GL_IMPL_FALLBACK(glGetShaderPrecisionFormat);
    GL_IMPL_FALLBACK(glDepthRangef);
    GL_IMPL_FALLBACK(glClearDepthf);

#define GL_PROC(r, f, ...)\
    if (!gles2.f)\
    {\
        fprintf(stderr, entry_warning, #f);\
        supported = GL_FALSE;\
    }
#include "gles2.def"
#undef GL_PROC

    return supported;
}
