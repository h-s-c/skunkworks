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

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>

#define SURFACE_SIZE		64

#define UNUSED(x)			(void)(x)

#define EGL_CHECK(f, ...)\
({\
	static const char *msg = "%s(%d): %s() failed with error %s(%04X)\n";\
	typeof (f(__VA_ARGS__)) r;\
	r = f(__VA_ARGS__);\
	if (!r)\
	{\
		EGLint e = eglGetError();\
		fprintf(stderr, msg, __FILE__, __LINE__, #f, eglGetErrorString(e), e);\
	}\
	r;\
})

static const char *eglGetErrorString(EGLint error)
{
	const char *s;

#define _E2S(e)	case e: s = #e; break;
	switch (error)
	{
	_E2S(EGL_SUCCESS)
	_E2S(EGL_NOT_INITIALIZED)
	_E2S(EGL_BAD_ACCESS)
	_E2S(EGL_BAD_ALLOC)
	_E2S(EGL_BAD_ATTRIBUTE)
	_E2S(EGL_BAD_CONFIG)
	_E2S(EGL_BAD_CONTEXT)
	_E2S(EGL_BAD_CURRENT_SURFACE)
	_E2S(EGL_BAD_DISPLAY)
	_E2S(EGL_BAD_MATCH)
	_E2S(EGL_BAD_NATIVE_PIXMAP)
	_E2S(EGL_BAD_NATIVE_WINDOW)
	_E2S(EGL_BAD_PARAMETER)
	_E2S(EGL_BAD_SURFACE)
	_E2S(EGL_CONTEXT_LOST)
	default: s = "????"; break;
	}
#undef _E2S

	return s;
}

#if defined(_WIN32)

EGLNativeDisplayType eglOpenNativeDisplay(const char *display_name)
{
	UNUSED(display_name);

	WNDCLASSEX wcx;
	HWND hwnd;

	ZeroMemory(&wcx, sizeof (wcx));
	wcx.cbSize = sizeof (wcx);
	wcx.lpfnWndProc = DefWindowProc;
	wcx.hInstance = GetModuleHandle(NULL);
	wcx.lpszClassName = TEXT("eglinfo");
	RegisterClassEx(&wcx);

	hwnd = CreateWindowEx(0, TEXT("eglinfo"), NULL, 0, 0, 0, 0, 0, NULL, NULL, wcx.hInstance, NULL);

	return GetDC(hwnd);
}

void eglCloseNativeDisplay(EGLNativeDisplayType native_display)
{
	HWND hwnd;

	hwnd = WindowFromDC(native_display);
	ReleaseDC(hwnd, native_display);
	DestroyWindow(hwnd);
}

EGLNativeWindowType eglCreateNativeWindow(EGLNativeDisplayType native_display, EGLint native_visual_id, EGLint width, EGLint height)
{
	UNUSED(native_display);
	UNUSED(native_visual_id);

	return CreateWindowEx(0, TEXT("eglinfo"), NULL, 0, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, GetModuleHandle(NULL), NULL);
}

void eglDestroyNativeWindow(EGLNativeDisplayType native_display, EGLNativeWindowType native_window)
{
	UNUSED(native_display);

	DestroyWindow(native_window);
}

EGLNativePixmapType eglCreateNativePixmap(EGLNativeDisplayType native_display, EGLint native_visual_id, EGLint width, EGLint height)
{
	UNUSED(native_display);
	UNUSED(native_visual_id);

	return CreateCompatibleBitmap(native_display, width, height);
}

void eglDestroyNativePixmap(EGLNativeDisplayType native_display, EGLNativePixmapType native_pixmap)
{
	UNUSED(native_display);

	DeleteObject(native_pixmap);
}

#else

EGLNativeDisplayType eglOpenNativeDisplay(const char *display_name)
{
	return XOpenDisplay(display_name);
}

void eglCloseNativeDisplay(EGLNativeDisplayType native_display)
{
	XCloseDisplay(native_display);
}

EGLNativeWindowType eglCreateNativeWindow(EGLNativeDisplayType native_display, EGLint native_visual_id, EGLint width, EGLint height)
{
	Window native_window;
	XVisualInfo *visual_info, visual_template;
	XSetWindowAttributes window_attribs;
	int n;

	visual_template.visualid = native_visual_id;
	visual_info = XGetVisualInfo(native_display, VisualIDMask, &visual_template, &n);
	window_attribs.colormap = XCreateColormap(native_display, RootWindow(native_display, visual_info->screen), visual_info->visual, AllocNone);
	window_attribs.border_pixel = 0;
	native_window = XCreateWindow(native_display, RootWindow(native_display, visual_info->screen), 0, 0, width, height, 0, visual_info->depth, InputOutput, visual_info->visual, CWBorderPixel | CWColormap, &window_attribs);
	XFree(visual_info);

	return native_window;
}

void eglDestroyNativeWindow(EGLNativeDisplayType native_display, EGLNativeWindowType native_window)
{
	XDestroyWindow(native_display, native_window);
}

EGLNativePixmapType eglCreateNativePixmap(EGLNativeDisplayType native_display, EGLint native_visual_id, EGLint width, EGLint height)
{
	Pixmap native_pixmap;
	XVisualInfo *visual_info, visual_template;
	int n;

	visual_template.visualid = native_visual_id;
	visual_info = XGetVisualInfo(native_display, VisualIDMask, &visual_template, &n);
	native_pixmap =  XCreatePixmap(native_display, RootWindow(native_display, visual_info->screen), width, height, visual_info->depth);
	XFree(visual_info);

	return native_pixmap;
}

void eglDestroyNativePixmap(EGLNativeDisplayType native_display, EGLNativePixmapType native_pixmap)
{
	XFreePixmap(native_display, native_pixmap);
}

#endif

EGLBoolean test(EGLNativeDisplayType native_display, EGLDisplay display, EGLConfig config)
{
	EGLNativeWindowType native_window = 0;
	EGLNativePixmapType native_pixmap = 0;
	EGLContext context;
	EGLSurface surface;
	EGLint config_id, native_visual_id, surface_type;
	EGLint context_config_id, surface_config_id, render_buffer, width, height;

	context = EGL_CHECK(eglCreateContext, display, config, EGL_NO_CONTEXT, NULL);
	if (!context)
		goto e0;

	if (!EGL_CHECK(eglGetConfigAttrib, display, config, EGL_CONFIG_ID, &config_id)
	||	!EGL_CHECK(eglGetConfigAttrib, display, config, EGL_NATIVE_VISUAL_ID, &native_visual_id)
	||	!EGL_CHECK(eglGetConfigAttrib, display, config, EGL_SURFACE_TYPE, &surface_type))
		goto e1;

	if (surface_type & EGL_WINDOW_BIT)
	{
		native_window = eglCreateNativeWindow(native_display, native_visual_id, SURFACE_SIZE, SURFACE_SIZE);
		surface = EGL_CHECK(eglCreateWindowSurface, display, config, native_window, NULL);
		if (!surface)
			goto e2;

		if (!EGL_CHECK(eglMakeCurrent, display, surface, surface, context))
			goto e3;

		if (!EGL_CHECK(eglQueryContext, display, context, EGL_CONFIG_ID, &context_config_id)
		||	!EGL_CHECK(eglQuerySurface, display, surface, EGL_CONFIG_ID, &surface_config_id)
		||	!EGL_CHECK(eglQuerySurface, display, surface, EGL_RENDER_BUFFER, &render_buffer)
		||	!EGL_CHECK(eglQuerySurface, display, surface, EGL_WIDTH, &width)
		||	!EGL_CHECK(eglQuerySurface, display, surface, EGL_HEIGHT, &height))
			goto e3;

		assert (EGL_CHECK(eglGetCurrentContext) == context);
		assert (EGL_CHECK(eglGetCurrentSurface, EGL_DRAW) == surface);
		assert (EGL_CHECK(eglGetCurrentSurface, EGL_READ) == surface);
		assert (context_config_id == surface_config_id);
		assert (render_buffer == EGL_BACK_BUFFER);
		/*assert (width == SURFACE_SIZE);
		assert (height == SURFACE_SIZE);*/

		EGL_CHECK(eglMakeCurrent, display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		EGL_CHECK(eglDestroySurface, display, surface);
		eglDestroyNativeWindow(native_display, native_window);
		native_window = 0;
	}
	if (surface_type & EGL_PIXMAP_BIT)
	{
		native_pixmap = eglCreateNativePixmap(native_display, native_visual_id, SURFACE_SIZE, SURFACE_SIZE);
		surface = eglCreatePixmapSurface(display, config, native_pixmap, NULL);
		if (!surface)
			goto e2;

		if (!EGL_CHECK(eglMakeCurrent, display, surface, surface, context))
			goto e3;

		if (!EGL_CHECK(eglQueryContext, display, context, EGL_CONFIG_ID, &context_config_id)
		||	!EGL_CHECK(eglQuerySurface, display, surface, EGL_CONFIG_ID, &surface_config_id)
		||	!EGL_CHECK(eglQuerySurface, display, surface, EGL_RENDER_BUFFER, &render_buffer)
		||	!EGL_CHECK(eglQuerySurface, display, surface, EGL_WIDTH, &width)
		||	!EGL_CHECK(eglQuerySurface, display, surface, EGL_HEIGHT, &height))
			goto e3;

		assert (EGL_CHECK(eglGetCurrentContext) == context);
		assert (EGL_CHECK(eglGetCurrentSurface, EGL_DRAW) == surface);
		assert (EGL_CHECK(eglGetCurrentSurface, EGL_READ) == surface);
		assert (context_config_id == surface_config_id);
		assert (render_buffer == EGL_SINGLE_BUFFER);
		assert (width == SURFACE_SIZE);
		assert (height == SURFACE_SIZE);

		EGL_CHECK(eglMakeCurrent, display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		EGL_CHECK(eglDestroySurface, display, surface);
		eglDestroyNativePixmap(native_display, native_pixmap);
		native_pixmap = 0;
	}
	if (surface_type & EGL_PBUFFER_BIT)
	{
		const EGLint pbuffer_attribs[] =
		{
			EGL_WIDTH, SURFACE_SIZE,
			EGL_HEIGHT, SURFACE_SIZE,
			EGL_NONE
		};
		surface = eglCreatePbufferSurface(display, config, pbuffer_attribs);
		if (!surface)
			goto e2;

		if (!EGL_CHECK(eglMakeCurrent, display, surface, surface, context))
			goto e3;

		if (!EGL_CHECK(eglQueryContext, display, context, EGL_CONFIG_ID, &context_config_id)
		||	!EGL_CHECK(eglQuerySurface, display, surface, EGL_CONFIG_ID, &surface_config_id)
		||	!EGL_CHECK(eglQuerySurface, display, surface, EGL_RENDER_BUFFER, &render_buffer)
		||	!EGL_CHECK(eglQuerySurface, display, surface, EGL_WIDTH, &width)
		||	!EGL_CHECK(eglQuerySurface, display, surface, EGL_HEIGHT, &height))
			goto e3;

		assert (EGL_CHECK(eglGetCurrentContext) == context);
		assert (EGL_CHECK(eglGetCurrentSurface, EGL_DRAW) == surface);
		assert (EGL_CHECK(eglGetCurrentSurface, EGL_READ) == surface);
		assert (context_config_id == surface_config_id);
		assert (render_buffer == EGL_BACK_BUFFER);
		assert (width == SURFACE_SIZE);
		assert (height == SURFACE_SIZE);

		EGL_CHECK(eglMakeCurrent, display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		EGL_CHECK(eglDestroySurface, display, surface);
	}
	EGL_CHECK(eglDestroyContext, display, context);

	return EGL_TRUE;

e3:	EGL_CHECK(eglDestroySurface, display, surface);
e2:	if (native_window)
		eglDestroyNativeWindow(native_display, native_window);
	if (native_pixmap)
		eglDestroyNativePixmap(native_display, native_pixmap);
e1:	EGL_CHECK(eglDestroyContext, display, context);

e0:	return EGL_FALSE;
}

int main(int argc, char *argv[])
{
	const EGLint config_attribs[] =
	{
		EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_NONE
	};

	EGLNativeDisplayType native_display;
	EGLNativeWindowType native_window;
	EGLDisplay display;
	EGLConfig config, *configs;
	EGLSurface surface;
	EGLContext context;
	EGLint num_config, config_id, i;

	const char *display_name = NULL;
	EGLBoolean do_test = EGL_FALSE;
	int success = EXIT_FAILURE;

	for (i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
		{
			printf("Usage: eglinfo [OPTION]\n");
			printf("Output detailed informations on supported EGL configuration.\n\n");
			printf("  -d, --display <name>       use given display\n");
			printf("  -t, --test                 perform extensive testing of all configs\n");
			printf("  -h, --help                 print this message\n");
			return EXIT_SUCCESS;
		}
		if (strcmp(argv[i], "--display") == 0 || strcmp(argv[i], "-d") == 0)
		{
			if (++i < argc)
				display_name = argv[i];
			else
			{
				fprintf(stderr, "eglinfo: missing display name argument\n");
				goto e0;
			}
		}
		else if (strcmp(argv[i], "--test") == 0 || strcmp(argv[i], "-t") == 0)
		{
			do_test = EGL_TRUE;
		}
		else
		{
			fprintf(stderr, "eglinfo: unrecognized option: `%s'\n", argv[i]);
			goto e0;
		}
	}

	if (!EGL_CHECK(eglBindAPI, EGL_OPENGL_API))
		goto e0;

	native_display = eglOpenNativeDisplay(display_name);
	display = EGL_CHECK(eglGetDisplay, native_display);
	if (!display || !EGL_CHECK(eglInitialize, display, NULL, NULL))
		goto e1;

	if (!EGL_CHECK(eglChooseConfig, display, config_attribs, &config, 1, &num_config))
		goto e2;
	if (num_config == 0 || !EGL_CHECK(eglGetConfigAttrib, display, config, EGL_NATIVE_VISUAL_ID, &config_id))
		goto e2;

	native_window = eglCreateNativeWindow(native_display, config_id, 1, 1);
	surface = EGL_CHECK(eglCreateWindowSurface, display, config, native_window, NULL);
	if (!surface)
		goto e3;

	context = EGL_CHECK(eglCreateContext, display, config, EGL_NO_CONTEXT, NULL);
	if (!context)
		goto e4;
	if (!EGL_CHECK(eglMakeCurrent, display, surface, surface, context))
		goto e5;

	if (!EGL_CHECK(eglGetConfigs, display, NULL, 0, &num_config))
		goto e5;
	configs = alloca(num_config * sizeof (configs[0]));
	if (!EGL_CHECK(eglGetConfigs, display, configs, num_config, &num_config))
		goto e5;

	printf("EGL Vendor: %s\n", eglQueryString(display, EGL_VENDOR));
	printf("EGL Version: %s\n", eglQueryString(display, EGL_VERSION));
	printf("EGL Client APIs: %s\n", eglQueryString(display, EGL_CLIENT_APIS));
	printf("EGL Extensions: %s\n", eglQueryString(display, EGL_EXTENSIONS));
	printf("\n");

	printf("GL Vendor: %s\n", glGetString(GL_VENDOR));
	printf("GL Renderer: %s\n", glGetString(GL_RENDERER));
	printf("GL Version: %s\n", glGetString(GL_VERSION));
	printf("GL Extensions: %s\n", glGetString(GL_EXTENSIONS));
	printf("\n");

	printf("+------+------------------------+-----------------------+-------+----+------+\n");
	printf("|      |         visual         |         color         | dp st | ms |  cav |\n");
	printf("|   id |  type srf nr fmt xp lv |  sz  r  g  b  a  l am | th cl | aa |  eat |\n");
	printf("+------+------------------------+-----------------------+-------+----+------+\n");

	for (i = 0; i < num_config; ++i)
	{
		EGLint config_id;
		EGLint renderable_type, surface_type, native_renderable, format, transparent_type, level;
		EGLint buffer_size, red_size, green_size, blue_size, alpha_size, luminance_size, alpha_mask_size;
		EGLint depth_size, stencil_size;
		EGLint samples;
		EGLint caveat;

		if (!EGL_CHECK(eglGetConfigAttrib, display, configs[i], EGL_CONFIG_ID, &config_id)
		||	!EGL_CHECK(eglGetConfigAttrib, display, configs[i], EGL_RENDERABLE_TYPE, &renderable_type)
		||	!EGL_CHECK(eglGetConfigAttrib, display, configs[i], EGL_SURFACE_TYPE, &surface_type)
		||	!EGL_CHECK(eglGetConfigAttrib, display, configs[i], EGL_NATIVE_RENDERABLE, &native_renderable)
		||	!EGL_CHECK(eglGetConfigAttrib, display, configs[i], EGL_COLOR_BUFFER_TYPE, &format)
		||	!EGL_CHECK(eglGetConfigAttrib, display, configs[i], EGL_TRANSPARENT_TYPE, &transparent_type)
		||	!EGL_CHECK(eglGetConfigAttrib, display, configs[i], EGL_LEVEL, &level)
		||	!EGL_CHECK(eglGetConfigAttrib, display, configs[i], EGL_BUFFER_SIZE, &buffer_size)
		||	!EGL_CHECK(eglGetConfigAttrib, display, configs[i], EGL_RED_SIZE, &red_size)
		||	!EGL_CHECK(eglGetConfigAttrib, display, configs[i], EGL_GREEN_SIZE, &green_size)
		||	!EGL_CHECK(eglGetConfigAttrib, display, configs[i], EGL_BLUE_SIZE, &blue_size)
		||	!EGL_CHECK(eglGetConfigAttrib, display, configs[i], EGL_ALPHA_SIZE, &alpha_size)
		||	!EGL_CHECK(eglGetConfigAttrib, display, configs[i], EGL_LUMINANCE_SIZE, &luminance_size)
		||	!EGL_CHECK(eglGetConfigAttrib, display, configs[i], EGL_ALPHA_MASK_SIZE, &alpha_mask_size)
		||	!EGL_CHECK(eglGetConfigAttrib, display, configs[i], EGL_DEPTH_SIZE, &depth_size)
		||	!EGL_CHECK(eglGetConfigAttrib, display, configs[i], EGL_STENCIL_SIZE, &stencil_size)
		||	!EGL_CHECK(eglGetConfigAttrib, display, configs[i], EGL_SAMPLES, &samples)
		||	!EGL_CHECK(eglGetConfigAttrib, display, configs[i], EGL_CONFIG_CAVEAT, &caveat))
			goto e5;

		printf("| %4d ", config_id);
		printf("| %c%c%c%c%c ", (renderable_type & EGL_OPENGL_BIT) ? 'o' : '-', (renderable_type & EGL_OPENGL_ES_BIT) ? '1' : '-', (renderable_type & EGL_OPENGL_ES2_BIT) ? '2' : '-', (renderable_type & EGL_OPENGL_ES3_BIT_KHR) ? '3' : '-', (renderable_type & EGL_OPENVG_BIT) ? 'v' : '-');
		printf("%c%c%c ", (surface_type & EGL_WINDOW_BIT) ? 'w' : '-', (surface_type & EGL_PBUFFER_BIT) ? 'p' : '-', (surface_type & EGL_PIXMAP_BIT) ? 'm' : '-');
		printf(" %c ", native_renderable ? 'y' : 'n');
		printf("%s ", format == EGL_RGB_BUFFER ? "rgb" : format == EGL_LUMINANCE_BUFFER ? "lum" : "???");
		printf(" %c ", transparent_type == EGL_TRANSPARENT_RGB ? 'y' : 'n');
		printf("%2d ", level);
		printf("| %3d %2d %2d %2d %2d %2d %2d ", buffer_size, red_size, green_size, blue_size, alpha_size, luminance_size, alpha_mask_size);
		depth_size ? printf("| %2d ", depth_size) : printf("|  - ");
		stencil_size ? printf("%2d ", stencil_size) : printf(" - ");
		samples ? printf("| %2d ", samples) : printf("|  - ");
		printf("| %s ", caveat == EGL_SLOW_CONFIG ? "slow" : caveat == EGL_NON_CONFORMANT_CONFIG ? "ncon" : "none");
		printf("|\n");
	}

	printf("+------+------------------------+-----------------------+-------+----+------+\n");

	if (do_test)
	{
		for (i = 0; i < num_config; ++i)
		{
			if (!test(native_display, display, configs[i]))
			{
				EGL_CHECK(eglGetConfigAttrib, display, configs[i], EGL_CONFIG_ID, &config_id);
				fprintf(stderr, "Config ID %d failed.\n\n", config_id);
			}
		}
	}

e5:	EGL_CHECK(eglDestroyContext, display, context);
e4:	EGL_CHECK(eglDestroySurface, display, surface);
e3:	eglDestroyNativeWindow(native_display, native_window);
e2:	EGL_CHECK(eglTerminate, display);
e1:	eglCloseNativeDisplay(native_display);

e0:	return success;
}
