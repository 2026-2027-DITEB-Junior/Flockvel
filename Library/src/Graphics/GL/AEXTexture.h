/// ---------------------------------------------------------------------------
/// \file			AEXTexture.h
/// \author			Thomas Komair
/// \brief			Declares the interface for a texture object
/// ---------------------------------------------------------------------------
#pragma once
// ---------------------------------------------------------------------------
// INCLUDES & DEPENDENCIES

#include "AEXGL_DLL.h"
#include "AEXGLRtti.h"

namespace clc
{
	namespace GL
	{
		// External Image loading // works with PNG, JPG and TGA
		bool API LoadImageFromFile(const char* filename, unsigned char*& outPixels, unsigned int& outWidth, unsigned int& outHeight);

		// --------------------------------------------------------------------------
		// Texture: Represents an image to be used when mapping onto a triangle mesh
		class  API Texture : public virtual GL::Rtti
		{
			// --------------------------------------------------------------------
			// Data memebers
		protected:
			friend class Graphics;
			friend class FrameBuffer;
			std::string  mFilePath;
			unsigned int mWidth;
			unsigned int mHeight;
			unsigned char* mPixels; // pixels in RGBA format
			unsigned int	mGLHandle;// openGL handle

			// Creates the texture in OpenGL
			void CreateOpenGLTexture();
			void FreeData();

			// --------------------------------------------------------------------
			// Methods
		public:

			// constructor
			Texture();
			Texture(unsigned int width, unsigned int height, unsigned char* pixels = nullptr);
			Texture(const char* filename);

			// Destructor
			virtual ~Texture();

			// Load from file
			void LoadFromFile(const char* filename);

			// Get File Path
			const char* GetFilePath();

			// Upload the texture data to openGL
			void UploadToGPU()const;
			void Bind(int tex_unit = 0)const;
			void Unbind(int tex_unit = 0)const;

			// getters and setters
			unsigned int GetWidth() const{ return mWidth; }
			unsigned int GetHeight() const { return mHeight; }
			unsigned char* GetPixels(unsigned int x = 0, unsigned int y = 0) { return mPixels + ((mWidth * y + x) * 4); }
			const unsigned char* GetPixels(unsigned int x = 0, unsigned int y = 0) const{ return mPixels + ((mWidth * y + x) * 4); }
			unsigned int GetGLHandle() const{ return mGLHandle; }
		};

		// --------------------------------------------------------------------------
		// FrameBuffer: Represents an a framebuffer where we can store results of 
		// rendering operations. Note: OpenGL comes with a default frame buffer and
		// so it's not necessary to create one ourselves to draw on screen.
		class API FrameBuffer : public virtual GL::Rtti
		{
		public:
		public:
			unsigned int framebuffer, depthRenderBuffer;
			Texture* tex;
			FrameBuffer(unsigned int width, unsigned int height, bool depthBuffer = true);
			~FrameBuffer();
			virtual void Bind();
			virtual void UnBind();
		};
	}
}
//-----------------------------------------------------------------------------