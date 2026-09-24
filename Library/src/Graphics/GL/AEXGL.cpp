#include "pch.h"
#include "AEXGL.h"
#include "AEXGLRtti.h"
#include <cstdio>
namespace clc
{
	namespace GL
	{
		std::string Rtti::GetType() const {

			// lazy init. _typename is empty
			std::string _typename;
			// get the type name using c++ rtti system
			_typename = typeid(*this).name();

			// process to remove 'class' or 'struct'
			std::size_t pos = _typename.find_last_of(' ');
			_typename = _typename.substr(pos + 1);

			// remove namespace as well, if any.
			pos = _typename.find_last_of(':');
			if (pos != std::string::npos)
				_typename = _typename.substr(pos + 1);

			// return stored typename
			return _typename;
		}

		namespace _internal
		{
			static HDC		ghDeviceContext = 0;
			static HGLRC	ghRenderContext = 0;
		}using namespace _internal;

		bool InitializeOpenGL(HWND hWin, int majorVersion, int minorVersion)
		{
			// create render context
			ghDeviceContext = 0;
			ghRenderContext = 0;

			// get the device context
			if ((ghDeviceContext = GetDC(hWin)) == 0)
				return false;

			PIXELFORMATDESCRIPTOR pfd;

			memset(&pfd, 0, sizeof(pfd));

			pfd.nSize = sizeof(pfd);
			pfd.nVersion = 1;
			pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
			pfd.iPixelType = PFD_TYPE_RGBA;
			pfd.cColorBits = 32;
			pfd.cDepthBits = 32;
			pfd.iLayerType = PFD_MAIN_PLANE;
			int iFormat = ChoosePixelFormat(ghDeviceContext, &pfd);

			SetPixelFormat(ghDeviceContext, iFormat, &pfd);

			// Create fake context
			if ((ghRenderContext = wglCreateContext(ghDeviceContext)) == 0)
			{
				ReleaseDC(hWin, ghDeviceContext);
				ghDeviceContext = 0;
				return false;
			}

			// set the current openGL render context
			wglMakeCurrent(ghDeviceContext, ghRenderContext);

			//// initialize glew
			GLenum err = glewInit();
			if (GLEW_OK != err)
			{
				//Problem: glewInit failed, something is seriously wrong.
				fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
				return false;
			}

			// Define context attributes
			int attribs[] =
			{
				WGL_CONTEXT_MAJOR_VERSION_ARB, majorVersion,
				WGL_CONTEXT_MINOR_VERSION_ARB, minorVersion,
				WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
				0
			};

			// Check that GLEW is supported
			if (wglewIsSupported("WGL_ARB_create_context") == 1)
			{
				// Create a new context and delete the temporary one
				HGLRC temp = ghRenderContext;
				ghRenderContext = wglCreateContextAttribsARB(ghDeviceContext, 0, attribs);
				wglMakeCurrent(NULL, NULL);
				wglDeleteContext(temp);
				wglMakeCurrent(ghDeviceContext, ghRenderContext);
			}

			// Check for valid OpenGL version
			char strError[20];
			sprintf_s(strError, "GL_VERSION_%d_%d", majorVersion, minorVersion);
			if (!glewIsSupported(strError))
			{
				char sErrorMessage[255], sErrorTitle[255];
				sprintf_s(sErrorMessage, "OpenGL %d.%d is not supported! Please download latest GPU drivers!", majorVersion, minorVersion);
				sprintf_s(sErrorTitle, "OpenGL %d.%d Not Supported", majorVersion, minorVersion);
				MessageBoxA(hWin, sErrorMessage, sErrorTitle, MB_ICONINFORMATION);
				return false;
			}

			return true;
		}
		void TerminateOpenGL()
		{
			// release the context
			wglMakeCurrent(NULL, NULL);
			if (ghRenderContext)
			{
				wglDeleteContext(ghRenderContext);
				ghRenderContext = NULL;
			}
		}
		void Present()
		{
			::SwapBuffers(ghDeviceContext);
		}
	}
}
