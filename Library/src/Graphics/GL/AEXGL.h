#ifndef AEX_GL_INCLUDES_H_
#define AEX_GL_INCLUDES_H_

// OpenGL libraries
#include "GL\glew.h"
#include "GL\wglew.h"

// Internal Libraries
#include "AEXGL_DLL.h"
#include "AEXCheckGLError.h"
#include "AEXColor.h"
#include "AEXTexture.h"
#include "AEXShader.h"
#include "AEXVertex.h"
#include "AEXModel.h"

namespace clc
{
	namespace GL
	{
		bool API InitializeOpenGL(HWND hWin, int majorVersion, int minorVersion);
		void API TerminateOpenGL();
		void API Present();
	}
}


#endif