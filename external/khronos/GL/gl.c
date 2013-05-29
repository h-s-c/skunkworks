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
#	define GLAPI __declspec(dllexport)
#elif defined(__WINSCW__) || defined(__SYMBIAN32__)
#	define GLAPI EXPORT_C
#else
#	define GLAPI
#endif

#include <stdio.h>
#include <string.h>
#include <GL/glcore.h>
#include <EGL/eglplatform.h>

void * EGLAPIENTRY eglGetProcAddress(const char *procname);

#define UNUSED(x)			(void)(x)

#define GL_PROC(r, f, ...)	typedef r APIENTRY (*f##_proc)(__VA_ARGS__);
#include "glcorearb.def"
#undef GL_PROC

GLAPI struct
{
#define GL_PROC(r, f, ...)	f##_proc f;
#include "glcorearb.def"
#undef GL_PROC
} gl;

#undef GL_APICALL
#define GL_APICALL KHRONOS_APICALL

GLboolean eglLinkGL(void)
{
	memset(&gl, 0, sizeof (gl));

#define GL_PROC(r, f, ...)	gl.f = (f##_proc)eglGetProcAddress(#f);
#include "glcorearb.def"
#undef GL_PROC

	return GL_TRUE;
}
