#ifndef GLAPI
#	if defined(_WIN32)
#		define GLAPI __declspec(dllimport)
#	else
#		define GLAPI extern
#	endif
#endif

#define GLCOREARB_PROTOTYPES
#include "glcorearb.h"
