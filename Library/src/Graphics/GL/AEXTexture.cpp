/// ---------------------------------------------------------------------------
/// \file			AEXTexture.cpp
/// \author			Thomas Komair
/// \brief			Implementation of the texture interface
/// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// INCLUDES & DEPENDENCIES
#include "pch.h"
#include <GL/glew.h>
#include <GL/wglew.h>
#include "AEXCheckGLError.h"
#include "AEXTexture.h"

// image loading library
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace clc
{
	namespace GL
	{
		// ---------------------------------------------
		#pragma region// SIMPLE TEXTURE

		Texture::Texture()
			: mWidth(0)
			, mHeight(0)
			, mPixels(0)
			, mGLHandle(0)
		{
			CreateOpenGLTexture();
		}

		// constructor
		Texture::Texture(unsigned int width, unsigned int height, unsigned char* pixels)
			: mWidth(width)
			, mHeight(height)
			, mPixels(pixels)
			, mGLHandle(0)
		{
			CreateOpenGLTexture();
		}
		Texture::Texture(const char* filename)
			: mWidth(0)
			, mHeight(0)
			, mPixels(NULL)
			, mGLHandle(0)
			, mFilePath(filename)
		{
			LoadFromFile(filename);
		}
		Texture::~Texture()
		{
			FreeData();
			if (mGLHandle)
			{
				glBindTexture(GL_TEXTURE_2D, mGLHandle); // bind
				glDeleteTextures(1, (GLuint*)(&mGLHandle)); // delete
				glBindTexture(GL_TEXTURE_2D, 0); // unbind
			}
		}

		void Texture::LoadFromFile(const char* filename)
		{
			if (!filename)
				return;

			// delete pixels if any
			FreeData();

			bool res = LoadImageFromFile(filename, mPixels, mWidth, mHeight);
			if (res)// success
			{
				// upload to GPU
				CreateOpenGLTexture();
				UploadToGPU();
			}
		}

		const char* Texture::GetFilePath()
		{
			return mFilePath.c_str();
		}

		// Upload the texture data to openGL
		void Texture::UploadToGPU() const
		{
			if (mGLHandle && (mWidth * mHeight) != 0)
			{
				// bind texture object
				Bind();

				// This will allocate memory for it
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<const void*>(mPixels));

				// NOTE ON OPENGL: 
				// The line below will replace the data without allocating memory for it
				// This is more efficient to call when you know the texture size hasn't changed, only the contents of the texture. 
				//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(mPixels));

				// sanity check
				check_gl_error();

				// Free the data when done
				//FreeData();
			}
		}

		// Creates the texture in OpenGL
		void Texture::CreateOpenGLTexture()
		{
			if (mGLHandle) // already created
				return;

			// if we have a texture associated already -> kill it
			if (mGLHandle)
				glDeleteTextures(1, (GLuint*)&mGLHandle);

			// Create texture and bind it
			glGenTextures(1, (GLuint*)&mGLHandle);
			glBindTexture(GL_TEXTURE_2D, mGLHandle);
			check_gl_error();

			// set texture look-up parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			check_gl_error();
		}
		void Texture::FreeData()
		{
			if (mPixels) {
				delete[] mPixels;
				mPixels = NULL;
			}
		}
		void Texture::Bind(int tex_unit) const
		{
			if (mGLHandle) {
				glActiveTexture(GL_TEXTURE0 + tex_unit);
				glBindTexture(GL_TEXTURE_2D, mGLHandle);
			}
		}
		void Texture::Unbind(int tex_unit) const
		{
			glActiveTexture(GL_TEXTURE0 + tex_unit);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		#pragma endregion 

		// ---------------------------------------------
		#pragma region// RENDER TEXTURE

		FrameBuffer::FrameBuffer(unsigned int width, unsigned int height, bool depthBuffer)
		{
			glGenFramebuffers(1, &framebuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			check_gl_error();

			// color buffer (implemented as a regular texture)
			tex = new Texture();
			tex->mWidth = width;
			tex->mHeight = height;
			tex->Bind();

			// allocate memory
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

			// set to the frame buffer
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->GetGLHandle(), 0);
			check_gl_error();

			// The depth buffer
			if (depthBuffer)
			{
				glGenRenderbuffers(1, &depthRenderBuffer);
				glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
				check_gl_error();
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
				check_gl_error();
			}

			// quetion: is it ok to unbind those once they've been attached to the frame buffer?
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			tex->Unbind();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			check_gl_error();

		}
		FrameBuffer::~FrameBuffer()
		{
			UnBind();
			glDeleteRenderbuffers(1, &depthRenderBuffer);
			delete tex;
			glDeleteFramebuffers(1, &framebuffer);
		}
		void FrameBuffer::Bind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			check_gl_error();

		}
		void FrameBuffer::UnBind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		#pragma endregion 

		// External Image loading // works with PNG, JPG and TGA
		bool LoadImageFromFile(const char* filename, unsigned char*& outPixels, unsigned int& outWidth, unsigned int& outHeight)
		{
			// Load the data
			int x, y, n, reqComp = 4;	// out parameters
			unsigned char* tempPixels = stbi_load(filename, &x, &y, &n, reqComp);	// 4 - forces output data to be of the form RGBA

			// Error
			if (tempPixels == NULL)
			{
				// TODO: provide error message
				return false;
			}

			//
			// Image loaded correctly

			// store width and height
			outWidth = (unsigned int)x;
			outHeight = (unsigned int)y;

			// allocate memory for our image
			outPixels = new unsigned char[outWidth * outHeight * reqComp];

			// copy the data but flip the height to match openGL
			for (unsigned int i = 0; i < outHeight; ++i)
			{
				for (unsigned int j = 0; j < outWidth; ++j)
				{
					unsigned int op = reqComp * (i * outWidth + j);
					unsigned int ip = reqComp * ((outHeight - 1 - i) * outWidth + j);
					outPixels[op] = tempPixels[ip];
					outPixels[op + 1] = tempPixels[ip + 1];
					outPixels[op + 2] = tempPixels[ip + 2];
					outPixels[op + 3] = tempPixels[ip + 3];
				}
			}

			// free the image from stbi
			stbi_image_free(tempPixels);

			// success
			return true;
		}
	}
}