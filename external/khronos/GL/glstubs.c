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

#include <KHR/khrplatform.h>
#include <GL/glcore.h>
#include "../utils/genstubs.h"

KHRONOS_APICALL struct
{
#define GL_PROC(r, f, ...)	r APIENTRY (*f)(__VA_ARGS__);
#include "glcorearb.def"
#undef GL_PROC
} gl;

#define GL_PROC(r, f, ...)\
GLAPI r APIENTRY VA_DECL(VA_NARGS(__VA_ARGS__), r, f, __VA_ARGS__)\
{\
	return gl.VA_CALL(VA_NARGS(__VA_ARGS__), r, f, __VA_ARGS__);\
}
#include "glcorearb.def"
#undef GL_PROC
