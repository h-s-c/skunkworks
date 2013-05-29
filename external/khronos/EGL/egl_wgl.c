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

#define EGLAPI				__declspec(dllexport)

#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GL/gl.h>
#include "wglext.h"

#ifndef NDEBUG
#	define do_assert(x)		assert(x)
#else
#	define do_assert(x)		(void)(x)
#endif

#define countof(a)			(sizeof (a) / sizeof (a[0]))

#define UNUSED(x)			(void)(x)

#define _STRINGIZE(x)		#x
#define STRINGIZE(x)		_STRINGIZE(x)

#define EGL_MAJOR			1
#define EGL_MINOR			4

#define WGL_EXT(e, m)
#define WGL_PROC(r, f, ...)	typedef r WINAPI (*f##_proc)(__VA_ARGS__);
#include "wgl.def"
#undef WGL_EXT
#undef WGL_PROC

#define WGL_EXT(e, m)	static EGLBoolean e##_flag;
#define WGL_PROC(r, f, ...)	static f##_proc f;
#include "wgl.def"
#undef WGL_EXT
#undef WGL_PROC

#define WGL_NONE			0

static struct
{
	EGLenum renderable, caveat;
	EGLNativeDisplayType default_display;
	EGLNativeWindowType display_window;
	ATOM window_class;
}
egl =
{
	.renderable = 0,
	.caveat = EGL_NONE,
	.default_display = NULL,
	.display_window = NULL,
	.window_class = 0
};

static __thread struct
{
	EGLint error;
	EGLenum api;
	EGLSurface current_draw, current_read;
	HDC draw_hdc, read_hdc;
}
egl_thread =
{
	.error = EGL_SUCCESS,
	.api = EGL_OPENGL_ES_API,
	.current_draw = EGL_NO_SURFACE,
	.current_read = EGL_NO_SURFACE,
	.draw_hdc = NULL,
	.read_hdc = NULL
};

extern EGLBoolean eglLinkGL(void);
extern EGLBoolean eglLinkGLES(void);
extern EGLBoolean eglLinkGLES2(void);
extern EGLBoolean eglLinkGLES3(void);

static EGLenum eglLink(EGLNativeDisplayType dpy)
{
	const char *exts, *missing_warning = "[WARNING] Missing WGL extension: %s.\n", *missing_error = "[ERROR] Missing WGL extension: %s.\n", *entry_error = "[ERROR] Missing WGL entry point: %s.\n";
	EGLBoolean failed = EGL_FALSE, mandatory;
	HGLRC hglrc;
	EGLenum renderable = 0;

	hglrc = wglCreateContext(dpy);
	if (!hglrc)
	{
		fprintf(stderr, "[ERROR] Unable to create dummy OpenGL rendering context.\n");
		return 0;
	}
	do_assert (wglMakeCurrent(dpy, hglrc));

	wglGetExtensionsStringARB = (wglGetExtensionsStringARB_proc)wglGetProcAddress("wglGetExtensionsStringARB");
	if (!wglGetExtensionsStringARB)
	{
		fprintf(stderr, missing_error, "WGL_ARB_extensions_string");
		goto end;
	}
	exts = wglGetExtensionsStringARB(dpy);

#define WGL_EXT(e, m)\
	e##_flag = strstr(exts, #e) ? EGL_TRUE : EGL_FALSE;\
	if (!e##_flag)\
	{\
		fprintf(stderr, m ? missing_error : missing_warning, #e);\
		failed |= m;\
	}
#define WGL_PROC(r, f, ...)
#include "wgl.def"
#undef WGL_EXT
#undef WGL_PROC
	if (failed)
		goto end;

#define WGL_EXT(e, m)		mandatory = m;
#define WGL_PROC(r, f, ...)\
	f = (f##_proc)wglGetProcAddress(#f);\
	if (!f && mandatory)\
	{\
		fprintf(stderr, entry_error, #f);\
		failed = EGL_TRUE;\
	}
#include "wgl.def"
#undef WGL_EXT
#undef WGL_PROC
	if (failed)
		goto end;

	if (eglLinkGL())
		renderable |= EGL_OPENGL_BIT;
	if (eglLinkGLES())
		renderable |= EGL_OPENGL_ES_BIT;
	if (eglLinkGLES2())
		renderable |= EGL_OPENGL_ES2_BIT;
	if (eglLinkGLES3())
		renderable |= EGL_OPENGL_ES3_BIT_KHR;

end:
	do_assert (wglMakeCurrent(dpy, NULL));
	do_assert (wglDeleteContext(hglrc));

	return renderable;
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
	WNDCLASSEX wcx;
	PIXELFORMATDESCRIPTOR pfd;
	INT pf;

	if (!egl.window_class)
	{
		ZeroMemory(&wcx, sizeof (wcx));
		wcx.cbSize = sizeof (wcx);
		wcx.lpfnWndProc = DefWindowProc;
		wcx.hInstance = GetModuleHandle(NULL);
		wcx.lpszClassName = TEXT("EGL");
		egl.window_class = RegisterClassEx(&wcx);
	}

	if (display_id == EGL_DEFAULT_DISPLAY)
	{
		if (!egl.default_display)
		{
			egl.display_window = CreateWindowEx(0, (LPTSTR)(size_t)egl.window_class, NULL, 0, 0, 0, 0, 0, NULL, NULL, GetModuleHandle(NULL), NULL);
			egl.default_display = GetDC(egl.display_window);
		}
		display_id = egl.default_display;
	}

	ZeroMemory(&pfd, sizeof (pfd));
	pfd.nSize = sizeof (pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	pf = ChoosePixelFormat(display_id, &pfd);
	if (!pf)
	{
		fprintf(stderr, "[ERROR] Cannot find suitable pixel format (OpenGL may not be supported by this display).");
		if (display_id == egl.default_display)
		{
			do_assert (ReleaseDC(egl.display_window, egl.default_display));
			do_assert (DestroyWindow(egl.display_window));
			egl.display_window = NULL;
			egl.default_display = NULL;
		}
		return EGL_NO_DISPLAY;
	}
	do_assert (SetPixelFormat(display_id, pf, NULL));

	do_assert (DescribePixelFormat(display_id, pf, sizeof (pfd), &pfd));
	egl.caveat = (pfd.dwFlags & PFD_GENERIC_FORMAT) ? EGL_SLOW_CONFIG : EGL_NONE;
	if (pfd.dwFlags & PFD_GENERIC_FORMAT)
	{
		fprintf(stderr, "[ERROR] No accelerated WGL implementation was found. Please install the latest ICD available for your display adapter.\n");
		return EGL_NO_DISPLAY;
	}

	return (EGLDisplay)display_id;
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
		do_assert (ReleaseDC(egl.display_window, egl.default_display));
		do_assert (DestroyWindow(egl.display_window));
		egl.display_window = NULL;
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
		return "WGL";

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
	const int numpf = WGL_NUMBER_PIXEL_FORMATS_ARB;
	const int wgl_attribs[] =
	{
		WGL_SUPPORT_OPENGL_ARB, TRUE,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_NONE
	};

	int *wgl_formats, *pconf, n;

	if (dpy == EGL_NO_DISPLAY)
		return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
	if (!egl.renderable)
		return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);
	if (!num_config)
		return eglSetError(EGL_BAD_PARAMETER, EGL_FALSE);

	if (!wglGetPixelFormatAttribivARB((HDC)dpy, 0, 0, 1, &numpf, &n))
		return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
	wgl_formats = alloca(n * sizeof (wgl_formats[0]));
	if (!wglChoosePixelFormatARB((HDC)dpy, wgl_attribs, NULL, (UINT)n, wgl_formats, (UINT *)&n))
		return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);

	if (configs)
	{
		pconf = wgl_formats;
		n = config_size < n ? config_size : n;
		*num_config = n;
		while (n--)
			*configs++ = (EGLConfig)*pconf++;
	}
	else
		*num_config = n;

	return EGL_TRUE;
}

EGLBoolean EGLAPIENTRY eglChooseConfig(EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config)
{
	const int numpf = WGL_NUMBER_PIXEL_FORMATS_ARB;
	const int supported = WGL_SUPPORT_OPENGL_ARB;

	int wgl_attribs[128], *pattr = wgl_attribs, *wgl_formats, *pconf, supported_value, layer = 0, n;

	if (dpy == EGL_NO_DISPLAY)
		return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
	if (!egl.renderable)
		return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);
	if (!num_config)
		return eglSetError(EGL_BAD_PARAMETER, EGL_FALSE);

	while (attrib_list && *attrib_list != EGL_NONE && pattr < wgl_attribs + countof (wgl_attribs))
	{
		switch (*attrib_list++)
		{
		case EGL_BUFFER_SIZE:
			*pattr++ = WGL_COLOR_BITS_ARB;
			*pattr++ = *attrib_list++;
			break;

		case EGL_ALPHA_SIZE:
			*pattr++ = WGL_ALPHA_BITS_ARB;
			*pattr++ = *attrib_list++;
			break;

		case EGL_BLUE_SIZE:
			*pattr++ = WGL_BLUE_BITS_ARB;
			*pattr++ = *attrib_list++;
			break;

		case EGL_GREEN_SIZE:
			*pattr++ = WGL_GREEN_BITS_ARB;
			*pattr++ = *attrib_list++;
			break;

		case EGL_RED_SIZE:
			*pattr++ = WGL_RED_BITS_ARB;
			*pattr++ = *attrib_list++;
			break;

		case EGL_DEPTH_SIZE:
			*pattr++ = WGL_DEPTH_BITS_ARB;
			*pattr++ = *attrib_list++;
			break;

		case EGL_STENCIL_SIZE:
			*pattr++ = WGL_STENCIL_BITS_ARB;
			*pattr++ = *attrib_list++;
			break;

		case EGL_CONFIG_CAVEAT:
			attrib_list++;
			break;

		case EGL_CONFIG_ID:
			if (wglGetPixelFormatAttribivARB((HDC)dpy, *attrib_list, 0, 1, &supported, &supported_value) && supported_value)
			{
				if (configs)
					configs[0] = (EGLConfig)*attrib_list;
				*num_config = 1;
			}
			else
				*num_config = 0;
			return EGL_TRUE;

		case EGL_LEVEL:
			layer = *attrib_list++;
			break;

		case EGL_NATIVE_RENDERABLE:
			*pattr++ = WGL_SUPPORT_GDI_ARB;
			*pattr++ = *attrib_list++;
			break;

		case EGL_NATIVE_VISUAL_TYPE:
			attrib_list++;
			break;

		case EGL_SAMPLES:
			*pattr++ = WGL_SAMPLES_ARB;
			*pattr++ = *attrib_list++;
			break;

		case EGL_SAMPLE_BUFFERS:
			*pattr++ = WGL_SAMPLE_BUFFERS_ARB;
			*pattr++ = *attrib_list++;
			break;

		case EGL_SURFACE_TYPE:
			if (*attrib_list & EGL_PBUFFER_BIT)
			{
				*pattr++ = WGL_DRAW_TO_PBUFFER_ARB;
				*pattr++ = TRUE;
			}
			if (*attrib_list & EGL_PIXMAP_BIT)
			{
				*pattr++ = WGL_DRAW_TO_BITMAP_ARB;
				*pattr++ = TRUE;
			}
			if (*attrib_list & EGL_WINDOW_BIT)
			{
				*pattr++ = WGL_DRAW_TO_WINDOW_ARB;
				*pattr++ = TRUE;
			}
			if (*attrib_list & (EGL_PBUFFER_BIT | EGL_WINDOW_BIT))
			{
				*pattr++ = WGL_DOUBLE_BUFFER_ARB;
				*pattr++ = TRUE;
			}
			attrib_list++;
			break;

		case EGL_TRANSPARENT_TYPE:
			*pattr++ = WGL_TRANSPARENT_ARB;
			switch (*attrib_list++)
			{
			case EGL_TRANSPARENT_RGB:
				*pattr++ = TRUE;
				break;

			default:
				*pattr++ = FALSE;
				break;
			}
			break;

		case EGL_TRANSPARENT_BLUE_VALUE:
			*pattr++ = WGL_TRANSPARENT_BLUE_VALUE_ARB;
			*pattr++ = *attrib_list++;
			break;

		case EGL_TRANSPARENT_GREEN_VALUE:
			*pattr++ = WGL_TRANSPARENT_GREEN_VALUE_ARB;
			*pattr++ = *attrib_list++;
			break;

		case EGL_TRANSPARENT_RED_VALUE:
			*pattr++ = WGL_TRANSPARENT_RED_VALUE_ARB;
			*pattr++ = *attrib_list++;
			break;

		case EGL_BIND_TO_TEXTURE_RGB:
			*pattr++ = WGL_BIND_TO_TEXTURE_RGB_ARB;
			*pattr++ = *attrib_list++;
			break;

		case EGL_BIND_TO_TEXTURE_RGBA:
			*pattr++ = WGL_BIND_TO_TEXTURE_RGBA_ARB;
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
	*pattr++ = WGL_SUPPORT_OPENGL_ARB;
	*pattr++ = TRUE;
	*pattr++ = WGL_ACCELERATION_ARB;
	*pattr++ = WGL_FULL_ACCELERATION_ARB;
	*pattr++ = WGL_PIXEL_TYPE_ARB;
	*pattr++ = WGL_TYPE_RGBA_ARB;
	*pattr = WGL_NONE;

	if (!wglGetPixelFormatAttribivARB((HDC)dpy, 0, 0, 1, &numpf, &n))
		return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
	wgl_formats = alloca(n * sizeof (int));
	if (!wglChoosePixelFormatARB((HDC)dpy, wgl_attribs, NULL, (UINT)n, wgl_formats, (UINT *)&n))
		return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);

	if (configs)
	{
		pconf = wgl_formats;
		n = config_size < n ? config_size : n;
		*num_config = n;
		while (n--)
			*configs++ = (EGLConfig)*pconf++;
	}
	else
		*num_config = n;

	return EGL_TRUE;
}

EGLBoolean EGLAPIENTRY eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value)
{
	const int surface_type[] = { WGL_DRAW_TO_PBUFFER_ARB, WGL_DRAW_TO_BITMAP_ARB, WGL_DRAW_TO_WINDOW_ARB };

	int wgl_attrib, surface_value[3];

	if (dpy == EGL_NO_DISPLAY)
		return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
	if (!egl.renderable)
		return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);

	switch (attribute)
	{
	case EGL_BUFFER_SIZE:
		wgl_attrib = WGL_COLOR_BITS_ARB;
		break;

	case EGL_ALPHA_SIZE:
		wgl_attrib = WGL_ALPHA_BITS_ARB;
		break;

	case EGL_BLUE_SIZE:
		wgl_attrib = WGL_BLUE_BITS_ARB;
		break;

	case EGL_GREEN_SIZE:
		wgl_attrib = WGL_GREEN_BITS_ARB;
		break;

	case EGL_RED_SIZE:
		wgl_attrib = WGL_RED_BITS_ARB;
		break;

	case EGL_DEPTH_SIZE:
		wgl_attrib = WGL_DEPTH_BITS_ARB;
		break;

	case EGL_STENCIL_SIZE:
		wgl_attrib = WGL_STENCIL_BITS_ARB;
		break;

	case EGL_CONFIG_CAVEAT:
		*value = egl.caveat;
		return EGL_TRUE;

	case EGL_CONFIG_ID:
		*value = (int)config;
		return EGL_TRUE;

	case EGL_LEVEL:
		*value = 0;
		return EGL_TRUE;

	case EGL_MAX_PBUFFER_HEIGHT:
		wgl_attrib = WGL_MAX_PBUFFER_HEIGHT_ARB;
		break;

	case EGL_MAX_PBUFFER_PIXELS:
		wgl_attrib = WGL_MAX_PBUFFER_PIXELS_ARB;
		break;

	case EGL_MAX_PBUFFER_WIDTH:
		wgl_attrib = WGL_MAX_PBUFFER_WIDTH_ARB;
		break;

	case EGL_NATIVE_RENDERABLE:
		wgl_attrib = WGL_SUPPORT_GDI_ARB;
		break;

	case EGL_NATIVE_VISUAL_ID:
		*value = (int)config;
		return EGL_TRUE;

	case EGL_NATIVE_VISUAL_TYPE:
		*value = EGL_NONE;
		return EGL_TRUE;

	case EGL_SAMPLES:
		wgl_attrib = WGL_SAMPLES_ARB;
		break;

	case EGL_SAMPLE_BUFFERS:
		wgl_attrib = WGL_SAMPLE_BUFFERS_ARB;
		break;

	case EGL_SURFACE_TYPE:
		if (!wglGetPixelFormatAttribivARB((HDC)dpy, (int)config, 0, countof (surface_type), surface_type, surface_value))
			return eglSetError(EGL_BAD_CONFIG, EGL_FALSE);
		*value = (surface_value[0] ? EGL_PBUFFER_BIT : 0) | (surface_value[1] ? EGL_PIXMAP_BIT : 0) | (surface_value[2] ? EGL_WINDOW_BIT : 0);
		return EGL_TRUE;

	case EGL_TRANSPARENT_TYPE:
		wgl_attrib = WGL_TRANSPARENT_ARB;
		break;

	case EGL_TRANSPARENT_BLUE_VALUE:
		wgl_attrib = WGL_TRANSPARENT_BLUE_VALUE_ARB;
		break;

	case EGL_TRANSPARENT_GREEN_VALUE:
		wgl_attrib = WGL_TRANSPARENT_GREEN_VALUE_ARB;
		break;

	case EGL_TRANSPARENT_RED_VALUE:
		wgl_attrib = WGL_TRANSPARENT_RED_VALUE_ARB;
		break;

	case EGL_BIND_TO_TEXTURE_RGB:
		wgl_attrib = WGL_BIND_TO_TEXTURE_RGB_ARB;
		break;

	case EGL_BIND_TO_TEXTURE_RGBA:
		wgl_attrib = WGL_BIND_TO_TEXTURE_RGBA_ARB;
		break;

	case EGL_MIN_SWAP_INTERVAL:
		*value = 0;
		return EGL_TRUE;

	case EGL_MAX_SWAP_INTERVAL:
		*value = WGL_EXT_swap_control_flag ? 1 : 0;
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

	if (!wglGetPixelFormatAttribivARB((HDC)dpy, (int)config, 0, 1, &wgl_attrib, value))
		return eglSetError(EGL_BAD_CONFIG, EGL_FALSE);

	switch (wgl_attrib)
	{
	case WGL_TRANSPARENT_ARB:
		*value = *value ? EGL_TRANSPARENT_RGB : EGL_NONE;
		break;
	}

	return EGL_TRUE;
}

EGLSurface EGLAPIENTRY eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list)
{
	HDC hdc;

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

	hdc = GetDC(win);
	if (hdc && GetPixelFormat(hdc) == 0 && !SetPixelFormat(hdc, (int)config, NULL))
	{
		do_assert (ReleaseDC(win, hdc));
		return eglSetError(EGL_BAD_MATCH, EGL_NO_SURFACE);
	}

	return (EGLSurface)hdc ? : eglSetError(EGL_BAD_NATIVE_WINDOW, EGL_NO_SURFACE);
}

EGLSurface EGLAPIENTRY eglCreatePbufferSurface(EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list)
{
	int wgl_attribs[32], *pattr = wgl_attribs, w = 0, h = 0;
	HPBUFFERARB wgl_pbuffer;

	if (dpy == EGL_NO_DISPLAY)
		return eglSetError(EGL_BAD_DISPLAY, EGL_NO_SURFACE);
	if (!egl.renderable)
		return eglSetError(EGL_NOT_INITIALIZED, EGL_NO_SURFACE);

	while (attrib_list && *attrib_list != EGL_NONE && pattr < wgl_attribs + countof (wgl_attribs))
	{
		switch (*attrib_list++)
		{
		case EGL_HEIGHT:
			h = *attrib_list++;
			break;

		case EGL_WIDTH:
			w = *attrib_list++;
			break;

		case EGL_LARGEST_PBUFFER:
			*pattr++ = WGL_PBUFFER_LARGEST_ARB;
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
	*pattr = WGL_NONE;

	wgl_pbuffer = wglCreatePbufferARB((HDC)dpy, (int)config, w, h, wgl_attribs);

	return (EGLSurface)wgl_pbuffer ? : eglSetError(EGL_BAD_MATCH, EGL_NO_SURFACE);
}

EGLSurface EGLAPIENTRY eglCreatePixmapSurface(EGLDisplay dpy, EGLConfig config, EGLNativePixmapType pixmap, const EGLint *attrib_list)
{
	HDC hdc;

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

	hdc = CreateCompatibleDC(dpy);
	if (!hdc)
		return eglSetError(EGL_BAD_DISPLAY, EGL_NO_SURFACE);
	if (!SelectObject(hdc, pixmap))
	{
		DeleteDC(hdc);
		return eglSetError(EGL_BAD_NATIVE_PIXMAP, EGL_NO_SURFACE);
	}
	if (!SetPixelFormat(hdc, (int)config, NULL))
	{
		DeleteDC(hdc);
		return eglSetError(EGL_BAD_MATCH, EGL_NO_SURFACE);
	}

	return (EGLSurface)hdc;
}

EGLBoolean EGLAPIENTRY eglDestroySurface(EGLDisplay dpy, EGLSurface surface)
{
	BOOL success;

	if (dpy == EGL_NO_DISPLAY)
		return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
	if (!egl.renderable)
		return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);
	if (surface == EGL_NO_SURFACE)
		return eglSetError(EGL_BAD_SURFACE, EGL_FALSE);

	switch (GetObjectType((HDC)surface))
	{
	case OBJ_DC:
		success = ReleaseDC(WindowFromDC((HDC)surface), (HDC)surface);
		break;

	case OBJ_MEMDC:
		success = DeleteDC((HDC)surface);
		break;

	default:
		success = wglDestroyPbufferARB((HANDLE)surface);
		break;
	}

	return success ? EGL_TRUE : eglSetError(EGL_BAD_SURFACE, EGL_FALSE);
}

EGLBoolean EGLAPIENTRY eglQuerySurface(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint *value)
{
	DWORD type;
	HDC hdc;
	HWND hwnd;
	RECT rect;
	HBITMAP hbitmap;
	BITMAP bitmap;

	if (dpy == EGL_NO_DISPLAY)
		return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
	if (!egl.renderable)
		return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);
	if (surface == EGL_NO_SURFACE)
		return eglSetError(EGL_BAD_SURFACE, EGL_FALSE);

	type = GetObjectType((HDC)surface);
	switch (attribute)
	{
	case EGL_CONFIG_ID:
		switch (type)
		{
		case OBJ_DC:
		case OBJ_MEMDC:
			*value = GetPixelFormat((HDC)surface);
			break;

		default:
			hdc = wglGetPbufferDCARB((HPBUFFERARB)surface);
			if (!hdc)
				return eglSetError(EGL_BAD_SURFACE, EGL_FALSE);
			*value = GetPixelFormat(hdc);
			do_assert (wglReleasePbufferDCARB((HPBUFFERARB)surface, hdc));
			break;
		}
		break;

	case EGL_WIDTH:
		switch (type)
		{
		case OBJ_DC:
			hwnd = WindowFromDC((HDC)surface);
			if (!hwnd || !GetClientRect(hwnd, &rect))
				return eglSetError(EGL_BAD_SURFACE, EGL_FALSE);
			*value = rect.right;
			break;

		case OBJ_MEMDC:
			hbitmap = GetCurrentObject((HDC)surface, OBJ_BITMAP);
			if (!hbitmap || GetObject(hbitmap, sizeof (bitmap), &bitmap) == 0)
				return eglSetError(EGL_BAD_SURFACE, EGL_FALSE);
			*value = bitmap.bmWidth;
			break;

		default:
			if (!wglQueryPbufferARB((HPBUFFERARB)surface, WGL_PBUFFER_WIDTH_ARB, value))
				return eglSetError(EGL_BAD_SURFACE, EGL_FALSE);
			break;
		}
		break;

	case EGL_HEIGHT:
		switch (type)
		{
		case OBJ_DC:
			hwnd = WindowFromDC((HDC)surface);
			if (!hwnd || !GetClientRect(hwnd, &rect))
				return eglSetError(EGL_BAD_SURFACE, EGL_FALSE);
			*value = rect.bottom;
			break;

		case OBJ_MEMDC:
			hbitmap = GetCurrentObject((HDC)surface, OBJ_BITMAP);
			if (!hbitmap || GetObject(hbitmap, sizeof (bitmap), &bitmap) == 0)
				return eglSetError(EGL_BAD_SURFACE, EGL_FALSE);
			*value = bitmap.bmHeight;
			break;

		default:
			if (!wglQueryPbufferARB((HPBUFFERARB)surface, WGL_PBUFFER_HEIGHT_ARB, value))
				return eglSetError(EGL_BAD_SURFACE, EGL_FALSE);
			break;
		}
		break;

	case EGL_LARGEST_PBUFFER:
		break;

	case EGL_HORIZONTAL_RESOLUTION:
		*value = GetDeviceCaps((HDC)dpy, HORZRES);
		break;

	case EGL_VERTICAL_RESOLUTION:
		*value = GetDeviceCaps((HDC)dpy, VERTRES);
		break;

	case EGL_PIXEL_ASPECT_RATIO:
		*value = GetDeviceCaps((HDC)dpy, ASPECTXY);
		break;

	case EGL_MULTISAMPLE_RESOLVE:
		*value = EGL_MULTISAMPLE_RESOLVE_DEFAULT;
		break;

	case EGL_RENDER_BUFFER:
		*value = type == OBJ_MEMDC ? EGL_SINGLE_BUFFER : EGL_BACK_BUFFER;
		break;

	case EGL_SWAP_BEHAVIOR:
		*value = EGL_BUFFER_DESTROYED;
		break;

	case EGL_TEXTURE_FORMAT:
	case EGL_TEXTURE_TARGET:
	case EGL_MIPMAP_TEXTURE:
	case EGL_MIPMAP_LEVEL:
		break;

	default:
		return eglSetError(EGL_BAD_ATTRIBUTE, EGL_FALSE);
	}

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
	glFinish();

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
	int wgl_buffer;

	if (dpy == EGL_NO_DISPLAY)
		return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
	if (!egl.renderable)
		return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);
	if (surface == EGL_NO_SURFACE)
		return eglSetError(EGL_BAD_SURFACE, EGL_FALSE);
	if (!WGL_ARB_render_texture_flag)
		return eglSetError(EGL_BAD_MATCH, EGL_FALSE);

	switch (buffer)
	{
	case EGL_BACK_BUFFER:
		wgl_buffer = WGL_BACK_LEFT_ARB;
		break;

	default:
		return eglSetError(EGL_BAD_PARAMETER, EGL_FALSE);
	}

	return wglBindTexImageARB((HPBUFFERARB)surface, wgl_buffer) ? EGL_TRUE : eglSetError(EGL_BAD_SURFACE, EGL_TRUE);
}

EGLBoolean EGLAPIENTRY eglReleaseTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
	int wgl_buffer;

	if (dpy == EGL_NO_DISPLAY)
		return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
	if (!egl.renderable)
		return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);
	if (surface == EGL_NO_SURFACE)
		return eglSetError(EGL_BAD_SURFACE, EGL_FALSE);
	if (!WGL_ARB_render_texture_flag)
		return eglSetError(EGL_BAD_MATCH, EGL_FALSE);

	switch (buffer)
	{
	case EGL_BACK_BUFFER:
		wgl_buffer = WGL_BACK_LEFT_ARB;
		break;

	default:
		return eglSetError(EGL_BAD_PARAMETER, EGL_FALSE);
	}

	return wglReleaseTexImageARB((HPBUFFERARB)surface, wgl_buffer) ? EGL_TRUE : eglSetError(EGL_BAD_SURFACE, EGL_FALSE);
}

EGLBoolean EGLAPIENTRY eglSwapInterval(EGLDisplay dpy, EGLint interval)
{
	if (dpy == EGL_NO_DISPLAY)
		return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
	if (!egl.renderable)
		return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);

	return WGL_EXT_swap_control_flag && wglSwapIntervalEXT(interval) ? EGL_TRUE : eglSetError(EGL_BAD_PARAMETER, EGL_FALSE);
}

EGLContext EGLAPIENTRY eglCreateContext(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list)
{
	const int surface_type[] = { WGL_DRAW_TO_PBUFFER_ARB, WGL_DRAW_TO_BITMAP_ARB, WGL_DRAW_TO_WINDOW_ARB };
	const int pbuffer_attribs[] = { WGL_NONE };

	int wgl_attribs[32], *pattr = wgl_attribs, major = 1, minor = 0, flags = 0, profile = 0, robustness = 0;
	int surface_value[3];
	HGLRC wgl_ctx = NULL;
	HDC hdc;
	HPBUFFERARB hpbuffer;
	HWND hwnd;

	if (dpy == EGL_NO_DISPLAY)
		return eglSetError(EGL_BAD_DISPLAY, EGL_NO_CONTEXT);
	if (!egl.renderable)
		return eglSetError(EGL_NOT_INITIALIZED, EGL_NO_CONTEXT);

	if (egl_thread.api == EGL_OPENGL_ES_API)
	{
		while (attrib_list && *attrib_list != EGL_NONE && pattr < wgl_attribs + countof (wgl_attribs))
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

		if (WGL_EXT_create_context_es2_profile_flag)
			profile |= WGL_CONTEXT_ES2_PROFILE_BIT_EXT;
	}
	else if (egl_thread.api == EGL_OPENGL_API)
	{
		while (attrib_list && *attrib_list != EGL_NONE && pattr < wgl_attribs + countof (wgl_attribs))
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
					flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
				if (*attrib_list & EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR)
					flags |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
				if (*attrib_list & EGL_CONTEXT_OPENGL_ROBUST_ACCESS_BIT_KHR && WGL_ARB_create_context_robustness_flag)
					flags |= WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB;
				attrib_list++;
				break;

			case EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR:
				if (*attrib_list & EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR)
					profile |= WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
				if (*attrib_list & EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT_KHR)
					profile |= WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
				attrib_list++;
				break;

			case EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY_KHR:
				switch (*attrib_list++)
				{
				case EGL_NO_RESET_NOTIFICATION_KHR:
					robustness = WGL_NO_RESET_NOTIFICATION_ARB;
					break;

				case EGL_LOSE_CONTEXT_ON_RESET_KHR:
					robustness = WGL_LOSE_CONTEXT_ON_RESET_ARB;
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
		while (attrib_list && *attrib_list != EGL_NONE && pattr < wgl_attribs + countof (wgl_attribs))
		{
			switch (*attrib_list++)
			{
			default:
				return eglSetError(EGL_BAD_ATTRIBUTE, EGL_NO_CONTEXT);
			}
		}
	}

	/*
	 * Now, this is really silly... Since the pixel format information is passed
	 * to wglCreateContext() through the HDC rather than directly, we *have* to
	 * create a throwaway surface whenever we require a context with a different
	 * pixel format than the one employed by EGLDisplay.
	 */

	if (GetPixelFormat((HDC)dpy) != (int)config)
	{
		if (!wglGetPixelFormatAttribivARB((HDC)dpy, (int)config, 0, countof (surface_type), surface_type, surface_value))
			return eglSetError(EGL_BAD_CONFIG, EGL_NO_CONTEXT);

		if (surface_value[0])
		{
			hpbuffer = wglCreatePbufferARB((HDC)dpy, (int)config, 1, 1, pbuffer_attribs);
			if (!hpbuffer)
				return eglSetError(EGL_BAD_ALLOC, EGL_NO_CONTEXT);
			hdc = wglGetPbufferDCARB(hpbuffer);
		}
		else
		{
			if (surface_value[2])
			{
				hwnd = CreateWindowEx(0, (LPTSTR)(size_t)egl.window_class, NULL, 0, 0, 0, 0, 0, NULL, NULL, GetModuleHandle(NULL), NULL);
				if (!hwnd)
					return eglSetError(EGL_BAD_ALLOC, EGL_NO_CONTEXT);
				hdc = GetDC(hwnd);
			}
			else
				hdc = CreateCompatibleDC((HDC)dpy);

			if (!SetPixelFormat(hdc, (int)config, NULL))
				goto cleanup;
		}
	}
	else
		hdc = (HDC)dpy;

	if (WGL_ARB_create_context_flag)
	{
		if (major != 1 || minor != 0)
		{
			*pattr++ = WGL_CONTEXT_MAJOR_VERSION_ARB;
			*pattr++ = major;
			*pattr++ = WGL_CONTEXT_MINOR_VERSION_ARB;
			*pattr++ = minor;
		}
		if (flags)
		{
			*pattr++ = WGL_CONTEXT_FLAGS_ARB;
			*pattr++ = flags;
		}
		if (profile && WGL_ARB_create_context_profile_flag)
		{
			*pattr++ = WGL_CONTEXT_PROFILE_MASK_ARB;
			*pattr++ = profile;
		}
		if (robustness && WGL_ARB_create_context_robustness_flag)
		{
			*pattr++ = WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB;
			*pattr++ = robustness;
		}
		*pattr = WGL_NONE;

		wgl_ctx = wglCreateContextAttribsARB(hdc, (HGLRC)share_context, wgl_attribs);
	}
	else
	{
		wgl_ctx = wglCreateContext(hdc);
		if (wgl_ctx && share_context != EGL_NO_CONTEXT)
			wglShareLists(wgl_ctx, (HGLRC)share_context);
	}

cleanup:

	if (hdc != (HDC)dpy)
	{
		if (surface_value[0])
		{
			do_assert (wglReleasePbufferDCARB(hpbuffer, hdc));
			do_assert (wglDestroyPbufferARB(hpbuffer));
		}
		else if (surface_value[2])
		{
			do_assert (ReleaseDC(hwnd, hdc));
			do_assert (DestroyWindow(hwnd));
		}
		else
			do_assert (DeleteDC(hdc));
	}

	return (EGLContext)wgl_ctx ? : eglSetError(EGL_BAD_CONFIG, EGL_NO_CONTEXT);
}

EGLBoolean EGLAPIENTRY eglDestroyContext(EGLDisplay dpy, EGLContext ctx)
{
	if (dpy == EGL_NO_DISPLAY)
		return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
	if (!egl.renderable)
		return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);
	if (ctx == EGL_NO_CONTEXT)
		return eglSetError(EGL_BAD_CONTEXT, EGL_FALSE);

	return wglDeleteContext((HGLRC)ctx) ? EGL_TRUE : eglSetError(EGL_BAD_CONTEXT, EGL_FALSE);
}

EGLBoolean EGLAPIENTRY eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{
	DWORD draw_type, read_type;
	HDC draw_hdc, read_hdc;

	if (dpy == EGL_NO_DISPLAY)
		return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
	if (!egl.renderable && (ctx != EGL_NO_CONTEXT || draw != EGL_NO_SURFACE || read != EGL_NO_SURFACE))
		return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);
	if (ctx == EGL_NO_CONTEXT && (draw != EGL_NO_SURFACE || read != EGL_NO_SURFACE))
		return eglSetError(EGL_BAD_MATCH, EGL_FALSE);
	if ((draw == EGL_NO_SURFACE && read != EGL_NO_SURFACE) || (draw != EGL_NO_SURFACE && read == EGL_NO_SURFACE))
		return eglSetError(EGL_BAD_MATCH, EGL_FALSE);

	if (draw != EGL_NO_SURFACE)
	{
		draw_type = GetObjectType((HDC)draw);
		draw_hdc = draw_type == OBJ_DC || draw_type == OBJ_MEMDC ? (HDC)draw : wglGetPbufferDCARB((HPBUFFERARB)draw);
	}
	else
		draw_hdc = NULL;

	if (read != EGL_NO_SURFACE)
	{
		read_type = GetObjectType((HDC)read);
		read_hdc = read_type == OBJ_DC || read_type == OBJ_MEMDC ? (HDC)read : wglGetPbufferDCARB((HPBUFFERARB)read);
	}
	else
		read_hdc = NULL;

	if (WGL_ARB_make_current_read_flag ? !wglMakeContextCurrentARB(draw_hdc, read_hdc, (HGLRC)ctx) : !wglMakeCurrent(draw_hdc, (HGLRC)ctx))
	{
		if (draw_hdc && draw_hdc != (HDC)draw)
			do_assert (wglReleasePbufferDCARB((HPBUFFERARB)draw, draw_hdc));
		if (read_hdc && read_hdc != (HDC)read)
			do_assert (wglReleasePbufferDCARB((HPBUFFERARB)read, read_hdc));
		return eglSetError(EGL_BAD_MATCH, EGL_FALSE);
	}

	if (egl_thread.current_draw != EGL_NO_SURFACE && egl_thread.current_draw != egl_thread.draw_hdc)
		do_assert (wglReleasePbufferDCARB((HPBUFFERARB)egl_thread.current_draw, egl_thread.draw_hdc));
	if (egl_thread.current_read != EGL_NO_SURFACE && egl_thread.current_read != egl_thread.read_hdc)
		do_assert (wglReleasePbufferDCARB((HPBUFFERARB)egl_thread.current_read, egl_thread.read_hdc));

	egl_thread.current_draw = draw;
	egl_thread.current_read = read;
	egl_thread.draw_hdc = draw_hdc;
	egl_thread.read_hdc = read_hdc;

	return EGL_TRUE;
}

EGLContext EGLAPIENTRY eglGetCurrentContext(void)
{
	return (EGLContext)wglGetCurrentContext() ? : EGL_NO_CONTEXT;
}

EGLSurface EGLAPIENTRY eglGetCurrentSurface(EGLint readdraw)
{
	switch (readdraw)
	{
	case EGL_DRAW:
		return egl_thread.current_draw;

	case EGL_READ:
		return egl_thread.current_read;

	default:
		return eglSetError(EGL_BAD_PARAMETER, EGL_NO_SURFACE);
	}
}

EGLDisplay EGLAPIENTRY eglGetCurrentDisplay(void)
{
	return (EGLDisplay)wglGetCurrentDC() ? : EGL_NO_DISPLAY;
}

EGLBoolean EGLAPIENTRY eglQueryContext(EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint *value)
{
	if (dpy == EGL_NO_DISPLAY)
		return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
	if (!egl.renderable)
		return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);
	if (ctx == EGL_NO_CONTEXT)
		return eglSetError(EGL_BAD_CONTEXT, EGL_FALSE);

	switch (attribute)
	{
	case EGL_CONFIG_ID:
		*value = egl_thread.draw_hdc ? GetPixelFormat(egl_thread.draw_hdc) : EGL_UNKNOWN;
		break;

	case EGL_CONTEXT_CLIENT_TYPE:
		*value = EGL_UNKNOWN;
		break;

	case EGL_CONTEXT_CLIENT_VERSION:
		*value = EGL_UNKNOWN;
		break;

	case EGL_RENDER_BUFFER:
		*value = egl_thread.current_draw ? GetObjectType(egl_thread.current_draw) == OBJ_MEMDC ? EGL_SINGLE_BUFFER : EGL_BACK_BUFFER : EGL_NONE;
		break;

	default:
		return eglSetError(EGL_BAD_ATTRIBUTE, EGL_FALSE);
	}

	return EGL_TRUE;
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

	GdiFlush();

	return EGL_TRUE;
}

EGLBoolean EGLAPIENTRY eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
	DWORD type;
	HDC hdc;
	BOOL success;

	if (dpy == EGL_NO_DISPLAY)
		return eglSetError(EGL_BAD_DISPLAY, EGL_FALSE);
	if (!egl.renderable)
		return eglSetError(EGL_NOT_INITIALIZED, EGL_FALSE);
	if (surface == EGL_NO_SURFACE)
		return eglSetError(EGL_BAD_SURFACE, EGL_FALSE);

	type = GetObjectType((HDC)surface);
	if (type != OBJ_DC && type != OBJ_MEMDC)
	{
		hdc = wglGetPbufferDCARB((HPBUFFERARB)surface);
		if (!hdc)
			return eglSetError(EGL_BAD_SURFACE, EGL_FALSE);
	}
	else
		hdc = (HDC)surface;

	success = SwapBuffers(hdc);

	if (type != OBJ_DC && type != OBJ_MEMDC)
		do_assert (wglReleasePbufferDCARB((HPBUFFERARB)surface, hdc));

	return success ? EGL_TRUE : eglSetError(EGL_BAD_SURFACE, EGL_FALSE);
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
	static HMODULE opengl = NULL;

	if (!opengl)
		opengl = LoadLibrary("opengl32.dll");

	return (__eglMustCastToProperFunctionPointerType)(wglGetProcAddress(procname) ? : GetProcAddress(opengl, procname));
}
