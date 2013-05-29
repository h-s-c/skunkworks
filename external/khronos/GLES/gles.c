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
#	define GL_API __declspec(dllexport)
#elif defined(__WINSCW__) || defined(__SYMBIAN32__)
#	define GL_API EXPORT_C
#else
#	define GL_API
#endif

#include <stdio.h>
#include <string.h>
#include <GLES/gl.h>
#include <EGL/eglplatform.h>

void * EGLAPIENTRY eglGetProcAddress(const char *procname);

#define UNUSED(x)			(void)(x)

#define GL_PROC(r, f, ...)	typedef r GL_APIENTRY (*f##_proc)(__VA_ARGS__);
#include "gles.def"
#undef GL_PROC

GL_API struct
{
#define GL_PROC(r, f, ...)	f##_proc f;
#include "gles.def"
#undef GL_PROC
} gles;

#undef GL_API
#define GL_API KHRONOS_APICALL

#define GL_IMPL(f, r)		(gles.f = gles.f ? : r)
#define GL_IMPL_EXT(f, p)	GL_IMPL(f, (f##_proc)eglGetProcAddress(#f#p))
#define GL_IMPL_FALLBACK(f)	GL_IMPL(f, f##_fallback)

#define I2X(n)				(GLfixed)((n) * 0x10000)
#define X2I(n)				((GLint)(n) / 0x10000)
#define F2X(n)				((n) >= 0x7FFFFFFFP-16F ? (GLfixed)0x7FFFFFFF : (n) <= -0x80000000P-16F ? (GLfixed)-0x80000000 : (GLfixed)((n) * 0x1P16F))
#define X2F(n)				((GLfloat)(n) / 0x1P16)
#define D2X(n)				((n) >= 0x7FFFFFFFP-16 ? (GLfixed)0x7FFFFFFF : (n) <= -0x80000000P-16 ? (GLfixed)-0x80000000 : (GLfixed)((n) * 0x1P16))
#define X2D(n)				((GLdouble)(n) / 0x1P16)
#define CLAMP2I(n)			(GLint)((n) * 0x7FFFFFFF)

typedef double GLdouble, GLclampd;

GL_API void GL_APIENTRY glClipPlane(GLenum plane, const GLdouble *equation);
GL_API void GL_APIENTRY glClearDepth(GLclampd depth);
GL_API void GL_APIENTRY glColor4d(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha);
GL_API void GL_APIENTRY glDepthRange(GLclampd znear, GLclampd zfar);
GL_API void GL_APIENTRY glGetDoublev(GLenum pname, GLdouble *params);
GL_API void GL_APIENTRY glFogi(GLenum pname, GLint param);
GL_API void GL_APIENTRY glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);
GL_API void GL_APIENTRY glGetClipPlane(GLenum plane, GLdouble *equation);
GL_API void GL_APIENTRY glLoadMatrixd(const GLdouble *m);
GL_API void GL_APIENTRY glMultMatrixd(const GLdouble *m);
GL_API void GL_APIENTRY glNormal3d(GLdouble nx, GLdouble ny, GLdouble nz);
GL_API void GL_APIENTRY glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);
GL_API void GL_APIENTRY glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
GL_API void GL_APIENTRY glScaled(GLdouble x, GLdouble y, GLdouble z);
GL_API void GL_APIENTRY glTranslated(GLdouble x, GLdouble y, GLdouble z);

typedef enum GetType
{
	GET_BOOLEAN,
	GET_INT,
	GET_FIXED,
	GET_FLOAT,
	GET_DOUBLE
} GetType;

typedef union GetResult
{
	GLboolean b;
	GLint i;
	GLfixed x;
	GLfloat f;
	GLdouble d;
} GetResult;

static GLenum glGetv(GLenum pname, GetResult *result, GetType *type, GLint *num)
{
	GLboolean b[16];
	GLint i[16];
	GLfloat f[16];
	GLdouble d[16];
	GetType t;
	GLint n;

	switch (pname)
	{
	case GL_DEPTH_WRITEMASK:
	case GL_LIGHT_MODEL_TWO_SIDE:
		t = GET_BOOLEAN;
		n = 1;
		break;

	case GL_COLOR_WRITEMASK:
		t = GET_BOOLEAN;
		n = 4;
		break;

	case GL_ACTIVE_TEXTURE:
	case GL_ALPHA_BITS:
	case GL_ALPHA_TEST_FUNC:
	case GL_ARRAY_BUFFER_BINDING:
	case GL_BLEND_DST:
	case GL_BLEND_SRC:
	case GL_BLUE_BITS:
	case GL_BUFFER_SIZE:
	case GL_CLIENT_ACTIVE_TEXTURE:
	case GL_COLOR_ARRAY_BUFFER_BINDING:
	case GL_COLOR_ARRAY_SIZE:
	case GL_COLOR_ARRAY_STRIDE:
	case GL_COLOR_ARRAY_TYPE:
	case GL_CULL_FACE:
	case GL_CULL_FACE_MODE:
	case GL_DEPTH_BITS:
	case GL_DEPTH_FUNC:
	case GL_ELEMENT_ARRAY_BUFFER_BINDING:
	case GL_FOG_HINT:
	case GL_FOG_MODE:
	case GL_FRONT_FACE:
	case GL_GENERATE_MIPMAP_HINT:
	case GL_GREEN_BITS:
	case GL_LINE_SMOOTH_HINT:
	case GL_LOGIC_OP_MODE:
	case GL_MATRIX_MODE:
	case GL_MAX_CLIP_PLANES:
	case GL_MAX_LIGHTS:
	case GL_MAX_MODELVIEW_STACK_DEPTH:
	case GL_MAX_PROJECTION_STACK_DEPTH:
	case GL_MAX_TEXTURE_SIZE:
	case GL_MAX_TEXTURE_STACK_DEPTH:
	case GL_MAX_TEXTURE_UNITS:
	case GL_MODELVIEW_STACK_DEPTH:
	case GL_NORMAL_ARRAY_BUFFER_BINDING:
	case GL_NORMAL_ARRAY_STRIDE:
	case GL_NORMAL_ARRAY_TYPE:
	case GL_NUM_COMPRESSED_TEXTURE_FORMATS:
	case GL_PACK_ALIGNMENT:
	case GL_PERSPECTIVE_CORRECTION_HINT:
	case GL_POINT_SMOOTH_HINT:
	case GL_PROJECTION_STACK_DEPTH:
	case GL_RED_BITS:
	case GL_SAMPLE_BUFFERS:
	case GL_SAMPLES:
	case GL_SHADE_MODEL:
	case GL_STENCIL_BITS:
	case GL_STENCIL_CLEAR_VALUE:
	case GL_STENCIL_FAIL:
	case GL_STENCIL_FUNC:
	case GL_STENCIL_PASS_DEPTH_FAIL:
	case GL_STENCIL_PASS_DEPTH_PASS:
	case GL_STENCIL_VALUE_MASK:
	case GL_STENCIL_WRITEMASK:
	case GL_SUBPIXEL_BITS:
	case GL_TEXTURE_BINDING_2D:
	case GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING:
	case GL_TEXTURE_COORD_ARRAY_SIZE:
	case GL_TEXTURE_COORD_ARRAY_STRIDE:
	case GL_TEXTURE_COORD_ARRAY_TYPE:
	case GL_TEXTURE_STACK_DEPTH:
	case GL_UNPACK_ALIGNMENT:
	case GL_VERTEX_ARRAY_BUFFER_BINDING:
	case GL_VERTEX_ARRAY_SIZE:
	case GL_VERTEX_ARRAY_STRIDE:
	case GL_VERTEX_ARRAY_TYPE:
		t = GET_INT;
		n = 1;
		break;

	case GL_MAX_VIEWPORT_DIMS:
		t = GET_INT;
		n = 2;
		break;

	case GL_SCISSOR_BOX:
	case GL_VIEWPORT:
		t = GET_INT;
		n = 4;
		break;

	case GL_COMPRESSED_TEXTURE_FORMATS:
		t = GET_INT;
		glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &n);
		break;

	case GL_ALPHA_TEST_REF:
	case GL_DEPTH_CLEAR_VALUE:
	case GL_FOG_DENSITY:
	case GL_FOG_END:
	case GL_FOG_START:
	case GL_LINE_WIDTH:
	case GL_POINT_FADE_THRESHOLD_SIZE:
	case GL_POINT_SIZE:
	case GL_POINT_SIZE_MAX:
	case GL_POINT_SIZE_MIN:
	case GL_POLYGON_OFFSET_FACTOR:
	case GL_POLYGON_OFFSET_UNITS:
	case GL_STENCIL_REF:
		t = GET_DOUBLE;
		n = 1;
		break;

	case GL_ALIASED_LINE_WIDTH_RANGE:
	case GL_ALIASED_POINT_SIZE_RANGE:
	case GL_DEPTH_RANGE:
	case GL_SMOOTH_LINE_WIDTH_RANGE:
	case GL_SMOOTH_POINT_SIZE_RANGE:
		t = GET_DOUBLE;
		n = 2;
		break;

	case GL_CURRENT_NORMAL:
	case GL_POINT_DISTANCE_ATTENUATION:
		t = GET_DOUBLE;
		n = 3;
		break;

	case GL_COLOR_CLEAR_VALUE:
	case GL_CURRENT_COLOR:
	case GL_CURRENT_TEXTURE_COORDS:
	case GL_FOG_COLOR:
	case GL_LIGHT_MODEL_AMBIENT:
		t = GET_DOUBLE;
		n = 4;
		break;

	case GL_MODELVIEW_MATRIX:
	case GL_PROJECTION_MATRIX:
	case GL_TEXTURE_MATRIX:
		t = GET_DOUBLE;
		n = 16;
		break;

	case GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES:
		i[0] = GL_RGBA;
		t = GET_INT;
		n = 1;
		break;

	case GL_IMPLEMENTATION_COLOR_READ_TYPE_OES:
		i[0] = GL_UNSIGNED_BYTE;
		t = GET_INT;
		n = 1;
		break;

	case GL_POINT_SIZE_ARRAY_BUFFER_BINDING_OES:
	case GL_POINT_SIZE_ARRAY_STRIDE_OES:
	case GL_POINT_SIZE_ARRAY_TYPE_OES:
		i[0] = 0;
		t = GET_INT;
		n = 1;
		break;

	default:
		return GL_INVALID_ENUM;
	}

	*num = n;
	*type = t;
	switch (t)
	{
	case GET_BOOLEAN:
		glGetBooleanv(pname, b);
		for (n = 0; n < *num; ++n)
			result[n].b = b[n];
		break;

	case GET_INT:
		glGetIntegerv(pname, i);
		for (n = 0; n < *num; ++n)
			result[n].i = i[n];
		break;

	case GET_FLOAT:
		glGetFloatv(pname, f);
		for (n = 0; n < *num; ++n)
			result[n].f = f[n];
		break;

	case GET_DOUBLE:
		glGetDoublev(pname, d);
		for (n = 0; n < *num; ++n)
			result[n].d = d[n];
		break;
	}
	return GL_NO_ERROR;
}

static void GL_APIENTRY glAlphaFuncx_fallback(GLenum func, GLclampx ref)
{
	glAlphaFunc(func, X2F(ref));
}

static void GL_APIENTRY glClearColorx_fallback(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
	glClearColor(X2F(red), X2F(green), X2F(blue), X2F(alpha));
}

static void GL_APIENTRY glClearDepthf_fallback(GLclampf depth)
{
	glClearDepth(depth);
}

static void GL_APIENTRY glClearDepthx_fallback(GLclampx depth)
{
	glClearDepth(X2D(depth));
}

static void GL_APIENTRY glClipPlanef_fallback(GLenum plane, const GLfloat *equation)
{
	const GLdouble d[] = { equation[0], equation[1], equation[2], equation[3] };
	glClipPlane(plane, d);
}

static void GL_APIENTRY glClipPlanex_fallback(GLenum plane, const GLfixed *equation)
{
	const GLdouble d[] = { X2D(equation[0]), X2D(equation[1]), X2D(equation[2]), X2D(equation[3]) };
	glClipPlane(plane, d);
}

static void GL_APIENTRY glColor4x_fallback(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
	glColor4d(X2D(red), X2D(green), X2D(blue), X2D(alpha));
}

static void GL_APIENTRY glDepthRangef_fallback(GLclampf znear, GLclampf zfar)
{
	glDepthRange(znear, zfar);
}

static void GL_APIENTRY glDepthRangex_fallback(GLclampx znear, GLclampx zfar)
{
	glDepthRange(X2D(znear), X2D(zfar));
}

static void GL_APIENTRY glFogx_fallback(GLenum pname, GLfixed param)
{
	pname != GL_FOG_MODE ? glFogf(pname, X2F(param)) : glFogi(GL_FOG_MODE, param);
}

static void GL_APIENTRY glFogxv_fallback(GLenum pname, const GLfixed *params)
{
	GLfloat f[4];

	if (pname != GL_FOG_MODE)
	{
		f[0] = X2F(params[0]);
		if (pname == GL_FOG_COLOR)
		{
			f[1] = X2F(params[1]);
			f[2] = X2F(params[2]);
			f[3] = X2F(params[3]);
		}
		glFogfv(pname, f);
	}
	else
		glFogi(GL_FOG_MODE, params[0]);
}

static void GL_APIENTRY glFrustumf_fallback(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
	glFrustum(left, right, bottom, top, zNear, zFar);
}

static void GL_APIENTRY glFrustumx_fallback(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
	glFrustum(X2D(left), X2D(right), X2D(bottom), X2D(top), X2D(zNear), X2D(zFar));
}

static void GL_APIENTRY glGetClipPlanef_fallback(GLenum pname, GLfloat eqn[4])
{
	GLdouble d[4];

	glGetClipPlane(pname, d);
	eqn[0] = d[0];
	eqn[1] = d[1];
	eqn[2] = d[2];
	eqn[3] = d[3];
}

static void GL_APIENTRY glGetClipPlanex_fallback(GLenum pname, GLfixed eqn[4])
{
	GLdouble d[4];

	glGetClipPlane(pname, d);
	eqn[0] = D2X(d[0]);
	eqn[1] = D2X(d[1]);
	eqn[2] = D2X(d[2]);
	eqn[3] = D2X(d[3]);
}

static void GL_APIENTRY glGetBooleanv_fallback(GLenum pname, GLboolean *params)
{
	GetResult r[16];
	GetType t;
	GLint n, i;

	if (glGetv(pname, r, &t, &n) != GL_NO_ERROR)
		return;

	switch (t)
	{
	case GET_BOOLEAN:
		for (i = 0; i < n; ++i)
			params[i] = r[i].b;
		break;

	case GET_INT:
		for (i = 0; i < n; ++i)
			params[i] = r[i].i != 0;
		break;

	case GET_FIXED:
		for (i = 0; i < n; ++i)
			params[i] = r[i].x != 0;
		break;

	case GET_FLOAT:
		for (i = 0; i < n; ++i)
			params[i] = r[i].f != 0.F;
		break;

	case GET_DOUBLE:
		for (i = 0; i < n; ++i)
			params[i] = r[i].d != 0.;
		break;
	}
}

static void GL_APIENTRY glGetIntegerv_fallback(GLenum pname, GLint *params)
{
	GetResult r[16];
	GetType t;
	GLint n, i;

	if (glGetv(pname, r, &t, &n) != GL_NO_ERROR)
		return;

	switch (pname)
	{
	case GL_ALPHA_TEST_REF:
	case GL_DEPTH_CLEAR_VALUE:
		params[0] = CLAMP2I(r[0].d);
		break;

	case GL_CURRENT_COLOR:
		params[0] = CLAMP2I(r[0].d);
		params[1] = CLAMP2I(r[1].d);
		params[2] = CLAMP2I(r[2].d);
		params[3] = CLAMP2I(r[3].d);
		break;

	default:
		switch (t)
		{
		case GET_BOOLEAN:
			for (i = 0; i < n; ++i)
				params[i] = r[i].b;
			break;

		case GET_INT:
			for (i = 0; i < n; ++i)
				params[i] = r[i].i;
			break;

		case GET_FIXED:
			for (i = 0; i < n; ++i)
				params[i] = X2I(r[i].x);
			break;

		case GET_FLOAT:
			for (i = 0; i < n; ++i)
				params[i] = (GLint)r[i].f;
			break;

		case GET_DOUBLE:
			for (i = 0; i < n; ++i)
				params[i] = (GLint)r[i].d;
			break;
		}
		break;
	}
}

static void GL_APIENTRY glGetFixedv_fallback(GLenum pname, GLfixed *params)
{
	GetResult r[16];
	GetType t;
	GLint n, i;

	if (glGetv(pname, r, &t, &n) != GL_NO_ERROR)
		return;

	switch (t)
	{
	case GET_BOOLEAN:
		for (i = 0; i < n; ++i)
			params[i] = I2X(r[i].b);
		break;

	case GET_INT:
		for (i = 0; i < n; ++i)
			params[i] = I2X(r[i].i);
		break;

	case GET_FIXED:
		for (i = 0; i < n; ++i)
			params[i] = r[i].x;
		break;

	case GET_FLOAT:
		for (i = 0; i < n; ++i)
			params[i] = F2X(r[i].f);
		break;

	case GET_DOUBLE:
		for (i = 0; i < n; ++i)
			params[i] = D2X(r[i].d);
		break;
	}
}

static void GL_APIENTRY glGetFloatv_fallback(GLenum pname, GLfloat *params)
{
	GetResult r[16];
	GetType t;
	GLint n, i;

	if (glGetv(pname, r, &t, &n) != GL_NO_ERROR)
		return;

	switch (t)
	{
	case GET_BOOLEAN:
		for (i = 0; i < n; ++i)
			params[i] = (GLfloat)r[i].b;
		break;

	case GET_INT:
		for (i = 0; i < n; ++i)
			params[i] = (GLfloat)r[i].i;
		break;

	case GET_FIXED:
		for (i = 0; i < n; ++i)
			params[i] = X2F(r[i].x);
		break;

	case GET_FLOAT:
		for (i = 0; i < n; ++i)
			params[i] = r[i].f;
		break;

	case GET_DOUBLE:
		for (i = 0; i < n; ++i)
			params[i] = r[i].d;
		break;
	}
}

static void GL_APIENTRY glGetLightxv_fallback(GLenum light, GLenum pname, GLfixed *params)
{
	GLfloat f[4];

	glGetLightfv(light, pname, f);
	switch (pname)
	{
	case GL_AMBIENT:
	case GL_DIFFUSE:
	case GL_SPECULAR:
	case GL_POSITION:
		params[3] = F2X(f[3]);
	case GL_SPOT_DIRECTION:
		params[2] = F2X(f[2]);
		params[1] = F2X(f[1]);
	case GL_SPOT_EXPONENT:
	case GL_SPOT_CUTOFF:
	case GL_CONSTANT_ATTENUATION:
	case GL_LINEAR_ATTENUATION:
	case GL_QUADRATIC_ATTENUATION:
		params[0] = F2X(f[0]);
	}
}

static void GL_APIENTRY glGetMaterialxv_fallback(GLenum face, GLenum pname, GLfixed *params)
{
	GLfloat f[4];

	glGetMaterialfv(face, pname, f);
	params[0] = F2X(f[0]);
	if (pname != GL_SHININESS)
	{
		params[1] = F2X(f[1]);
		params[2] = F2X(f[2]);
		params[3] = F2X(f[3]);
	}
}

static void GL_APIENTRY glGetTexEnvxv_fallback(GLenum env, GLenum pname, GLfixed *params)
{
	GLfloat f[4];
	GLint i;

	switch (pname)
	{
	case GL_TEXTURE_ENV_COLOR:
		glGetTexEnvfv(env, pname, f);
		params[3] = F2X(f[3]);
		params[2] = F2X(f[2]);
		params[1] = F2X(f[1]);
	case GL_RGB_SCALE:
	case GL_ALPHA_SCALE:
		params[0] = F2X(f[0]);
		break;

	case GL_COMBINE_RGB:
	case GL_COMBINE_ALPHA:
	case GL_COORD_REPLACE_OES:
	case GL_TEXTURE_ENV_MODE:
	case GL_SRC0_RGB:
	case GL_SRC0_ALPHA:
	case GL_SRC1_RGB:
	case GL_SRC1_ALPHA:
	case GL_SRC2_RGB:
	case GL_SRC2_ALPHA:
	case GL_OPERAND0_RGB:
	case GL_OPERAND0_ALPHA:
	case GL_OPERAND1_RGB:
	case GL_OPERAND1_ALPHA:
	case GL_OPERAND2_RGB:
	case GL_OPERAND2_ALPHA:
		glGetTexEnviv(env, pname, &i);
		params[0] = (GLfixed)i;
		break;
	}
}

static void GL_APIENTRY glGetTexParameterxv_fallback(GLenum target, GLenum pname, GLfixed *params)
{
	GLfloat f;

	glGetTexParameterfv(target, pname, &f);
	params[0] = F2X(f);
}

static void GL_APIENTRY glLightModelx_fallback(GLenum pname, GLfixed param)
{
	glLightModelf(pname, X2F(param));
}

static void GL_APIENTRY glLightModelxv_fallback(GLenum pname, const GLfixed *params)
{
	GLfloat f[4];

	f[0] = X2F(params[0]);
	if (pname != GL_LIGHT_MODEL_TWO_SIDE)
	{
		f[1] = X2F(params[1]);
		f[2] = X2F(params[2]);
		f[3] = X2F(params[3]);
	}
	glLightModelfv(pname, f);
}

static void GL_APIENTRY glLightx_fallback(GLenum light, GLenum pname, GLfixed param)
{
	glLightf(light, pname, X2F(param));
}

static void GL_APIENTRY glLightxv_fallback(GLenum light, GLenum pname, const GLfixed *params)
{
	GLfloat f[4];

	switch (pname)
	{
	case GL_AMBIENT:
	case GL_DIFFUSE:
	case GL_SPECULAR:
	case GL_POSITION:
		f[3] = X2F(params[3]);
	case GL_SPOT_DIRECTION:
		f[2] = X2F(params[2]);
		f[1] = X2F(params[1]);
	case GL_SPOT_EXPONENT:
	case GL_SPOT_CUTOFF:
	case GL_CONSTANT_ATTENUATION:
	case GL_LINEAR_ATTENUATION:
	case GL_QUADRATIC_ATTENUATION:
		f[0] = X2F(params[0]);
	}
	glLightfv(light, pname, f);
}

static void GL_APIENTRY glLineWidthx_fallback(GLfixed width)
{
	glLineWidth(X2F(width));
}

static void GL_APIENTRY glLoadMatrixx_fallback(const GLfixed *m)
{
	const GLdouble d[] =
	{
		X2D(m[0]), X2D(m[1]), X2D(m[2]), X2D(m[3]),
		X2D(m[4]), X2D(m[5]), X2D(m[6]), X2D(m[7]),
		X2D(m[8]), X2D(m[9]), X2D(m[10]), X2D(m[11]),
		X2D(m[12]), X2D(m[13]), X2D(m[14]), X2D(m[15])
	};
	glLoadMatrixd(d);
}

static void GL_APIENTRY glMaterialx_fallback(GLenum face, GLenum pname, GLfixed param)
{
	glMaterialf(face, pname, X2F(param));
}

static void GL_APIENTRY glMaterialxv_fallback(GLenum face, GLenum pname, const GLfixed *params)
{
	GLfloat f[4];

	switch (pname)
	{
	case GL_AMBIENT:
	case GL_DIFFUSE:
	case GL_SPECULAR:
	case GL_EMISSION:
	case GL_AMBIENT_AND_DIFFUSE:
		f[3] = X2F(params[3]);
		f[2] = X2F(params[2]);
		f[1] = X2F(params[1]);
	case GL_SHININESS:
		f[0] = X2F(params[0]);
	}
	glMaterialfv(face, pname, f);
}

static void GL_APIENTRY glMultiTexCoord4x_fallback(GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q)
{
	gles.glMultiTexCoord4f(target, X2F(s), X2F(t), X2F(r), X2F(q));
}

static void GL_APIENTRY glMultMatrixx_fallback(const GLfixed *m)
{
	const GLdouble d[] =
	{
		X2D(m[0]), X2D(m[1]), X2D(m[2]), X2D(m[3]),
		X2D(m[4]), X2D(m[5]), X2D(m[6]), X2D(m[7]),
		X2D(m[8]), X2D(m[9]), X2D(m[10]), X2D(m[11]),
		X2D(m[12]), X2D(m[13]), X2D(m[14]), X2D(m[15])
	};
	glMultMatrixd(d);
}

static void GL_APIENTRY glNormal3x_fallback(GLfixed nx, GLfixed ny, GLfixed nz)
{
	glNormal3d(X2D(nx), X2D(ny), X2D(nz));
}

static void GL_APIENTRY glOrthof_fallback(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
	glOrtho(left, right, bottom, top, zNear, zFar);
}

static void GL_APIENTRY glOrthox_fallback(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
	glOrtho(X2D(left), X2D(right), X2D(bottom), X2D(top), X2D(zNear), X2D(zFar));
}

static void GL_APIENTRY glPointParameterx_fallback(GLenum pname, GLfixed param)
{
	gles.glPointParameterf(pname, X2F(param));
}

static void GL_APIENTRY glPointParameterxv_fallback(GLenum pname, const GLfixed *params)
{
	GLfloat f[3];

	switch (pname)
	{
	case GL_POINT_DISTANCE_ATTENUATION:
		f[2] = X2F(params[2]);
		f[1] = X2F(params[1]);
	case GL_POINT_SIZE_MIN:
	case GL_POINT_SIZE_MAX:
	case GL_POINT_FADE_THRESHOLD_SIZE:
		f[0] = X2F(params[0]);
	}
	gles.glPointParameterfv(pname, f);
}

static void GL_APIENTRY glPointSizex_fallback(GLfixed size)
{
	glPointSize(X2F(size));
}

static void GL_APIENTRY glPolygonOffsetx_fallback(GLfixed factor, GLfixed units)
{
	glPolygonOffset(X2F(factor), X2F(units));
}

static void GL_APIENTRY glRotatex_fallback(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
	glRotated(X2D(angle), X2D(x), X2D(y), X2D(z));
}

static void GL_APIENTRY glSampleCoveragex_fallback(GLclampx value, GLboolean invert)
{
	gles.glSampleCoverage(X2F(value), invert);
}

static void GL_APIENTRY glScalex_fallback(GLfixed x, GLfixed y, GLfixed z)
{
	glScaled(X2D(x), X2D(y), X2D(z));
}

static void GL_APIENTRY glTexEnvx_fallback(GLenum target, GLenum pname, GLfixed param)
{
	switch (pname)
	{
	case GL_RGB_SCALE:
	case GL_ALPHA_SCALE:
		glTexEnvf(target, pname, X2F(param));
		break;

	case GL_COORD_REPLACE_OES:
	case GL_TEXTURE_ENV_MODE:
	case GL_COMBINE_RGB:
	case GL_COMBINE_ALPHA:
	case GL_SRC0_RGB:
	case GL_SRC0_ALPHA:
	case GL_SRC1_RGB:
	case GL_SRC1_ALPHA:
	case GL_SRC2_RGB:
	case GL_SRC2_ALPHA:
	case GL_OPERAND0_RGB:
	case GL_OPERAND0_ALPHA:
	case GL_OPERAND1_RGB:
	case GL_OPERAND1_ALPHA:
	case GL_OPERAND2_RGB:
	case GL_OPERAND2_ALPHA:
		glTexEnvi(target, pname, param);
		break;
	}
}

static void GL_APIENTRY glTexEnvxv_fallback(GLenum target, GLenum pname, const GLfixed *params)
{
	GLfloat f[4];

	switch (pname)
	{
	case GL_TEXTURE_ENV_COLOR:
		f[0] = X2F(params[0]);
		f[1] = X2F(params[1]);
		f[2] = X2F(params[2]);
		f[3] = X2F(params[3]);
		glTexEnvfv(target, pname, f);
		break;

	case GL_RGB_SCALE:
	case GL_ALPHA_SCALE:
		glTexEnvf(target, pname, X2F(params[0]));
		break;

	case GL_COORD_REPLACE_OES:
	case GL_TEXTURE_ENV_MODE:
	case GL_COMBINE_RGB:
	case GL_COMBINE_ALPHA:
	case GL_SRC0_RGB:
	case GL_SRC0_ALPHA:
	case GL_SRC1_RGB:
	case GL_SRC1_ALPHA:
	case GL_SRC2_RGB:
	case GL_SRC2_ALPHA:
	case GL_OPERAND0_RGB:
	case GL_OPERAND0_ALPHA:
	case GL_OPERAND1_RGB:
	case GL_OPERAND1_ALPHA:
	case GL_OPERAND2_RGB:
	case GL_OPERAND2_ALPHA:
		glTexEnvi(target, pname, params[0]);
		break;
	}
}

static void GL_APIENTRY glTexParameterx_fallback(GLenum target, GLenum pname, GLfixed param)
{
	glTexParameteri(target, pname, param);
}

static void GL_APIENTRY glTexParameterxv_fallback(GLenum target, GLenum pname, const GLfixed *params)
{
	gles.glTexParameterx(target, pname, params[0]);
}

static void GL_APIENTRY glTranslatex_fallback(GLfixed x, GLfixed y, GLfixed z)
{
	glTranslated(X2D(x), X2D(y), X2D(z));
}

static void GL_APIENTRY glPointSizePointerOES_fallback(GLenum type, GLsizei stride, const GLvoid *pointer)
{
	UNUSED(type);
	UNUSED(stride);
	UNUSED(pointer);

	/* NOT IMPLEMENTED */
}

GLboolean eglLinkGLES(void)
{
	const char *entry_warning = "[WARNING] Cannot retrieve GLES entry point: %s\n";
	GLboolean supported = GL_TRUE;

	memset(&gles, 0, sizeof (gles));

#define GL_PROC(r, f, ...)	gles.f = (f##_proc)eglGetProcAddress(#f);
#include "gles.def"
#undef GL_PROC

	GL_IMPL_FALLBACK(glAlphaFuncx);
	GL_IMPL_FALLBACK(glClearColorx);
	GL_IMPL_FALLBACK(glClearDepthf);
	GL_IMPL_FALLBACK(glClearDepthx);
	GL_IMPL_FALLBACK(glClipPlanef);
	GL_IMPL_FALLBACK(glClipPlanex);
	GL_IMPL_FALLBACK(glColor4x);
	GL_IMPL_FALLBACK(glDepthRangef);
	GL_IMPL_FALLBACK(glDepthRangex);
	GL_IMPL_FALLBACK(glFogx);
	GL_IMPL_FALLBACK(glFogxv);
	GL_IMPL_FALLBACK(glFrustumf);
	GL_IMPL_FALLBACK(glFrustumx);
	GL_IMPL_FALLBACK(glGetClipPlanef);
	GL_IMPL_FALLBACK(glGetClipPlanex);
	GL_IMPL_FALLBACK(glGetBooleanv);
	GL_IMPL_FALLBACK(glGetIntegerv);
	GL_IMPL_FALLBACK(glGetFixedv);
	GL_IMPL_FALLBACK(glGetFloatv);
	GL_IMPL_FALLBACK(glGetLightxv);
	GL_IMPL_FALLBACK(glGetMaterialxv);
	GL_IMPL_FALLBACK(glGetTexEnvxv);
	GL_IMPL_FALLBACK(glGetTexParameterxv);
	GL_IMPL_FALLBACK(glLightModelx);
	GL_IMPL_FALLBACK(glLightModelxv);
	GL_IMPL_FALLBACK(glLightx);
	GL_IMPL_FALLBACK(glLightxv);
	GL_IMPL_FALLBACK(glLineWidthx);
	GL_IMPL_FALLBACK(glLoadMatrixx);
	GL_IMPL_FALLBACK(glMaterialx);
	GL_IMPL_FALLBACK(glMaterialxv);
	GL_IMPL_FALLBACK(glMultiTexCoord4x);
	GL_IMPL_FALLBACK(glMultMatrixx);
	GL_IMPL_FALLBACK(glNormal3x);
	GL_IMPL_FALLBACK(glOrthof);
	GL_IMPL_FALLBACK(glOrthox);
	GL_IMPL_FALLBACK(glPointParameterx);
	GL_IMPL_FALLBACK(glPointParameterxv);
	GL_IMPL_FALLBACK(glPointSizex);
	GL_IMPL_FALLBACK(glPolygonOffsetx);
	GL_IMPL_FALLBACK(glRotatex);
	GL_IMPL_FALLBACK(glSampleCoveragex);
	GL_IMPL_FALLBACK(glScalex);
	GL_IMPL_FALLBACK(glTexEnvx);
	GL_IMPL_FALLBACK(glTexEnvxv);
	GL_IMPL_FALLBACK(glTexParameterx);
	GL_IMPL_FALLBACK(glTexParameterxv);
	GL_IMPL_FALLBACK(glTranslatex);
	GL_IMPL_FALLBACK(glPointSizePointerOES);

#define GL_PROC(r, f, ...)\
	if (!gles.f)\
	{\
		fprintf(stderr, entry_warning, #f);\
		supported = GL_FALSE;\
	}
#include "gles.def"
#undef GL_PROC

	return supported;
}
