#ifndef AEX_GL_ERROR_H_
#define AEX_GL_ERROR_H_
#include "AEXGL_DLL.h"

#ifdef _DEBUG
	#define check_gl_error() clc::_check_gl_error(__FILE__, __LINE__)
#else
	#define check_gl_error() 
#endif
namespace clc
{
	void  API _check_gl_error(const char * file, int line);
}
#endif