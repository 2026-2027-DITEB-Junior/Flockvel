/// ---------------------------------------------------------------------------
/// \file			AEXShader.h
/// \author			Thomas Komair
/// \brief			Declares the interface for OpenGL shaders and programs.
/// ---------------------------------------------------------------------------
#ifndef AEX_SHADER_H_
#define AEX_SHADER_H_
// ---------------------------------------------------------------------------
// INCLUDES & DEPENDENCIES

#include "AEXGL_DLL.h"
#include "AEXGLRtti.h"
#include <glm/glm.hpp>

namespace clc
{
	struct AEMtx44;
	struct AEMtx33;
	struct AEVec2;
	struct AEVec3;
	struct Color;

	namespace GL
	{
		class API   Shader : public virtual GL::Rtti
		{
		public: enum EType { eVertex = 0, eFragment, eTypeCount };
		public:
			Shader();
			Shader(const char* filename, EType shaderType);
			~Shader();
			void Load(const char* filename = NULL);
			void LoadSource(const char* filename = NULL);
			void Compile();

			EType GetShaderType() const { return mShaderType; }
			// this will recreate the opengl shader if necessary
			void SetShaderType(EType st, bool createDeviceShader = false);
			void CreateDeviceShader();

		protected:
			friend class ShaderProgram;
			unsigned int mGLHandle = 0;

			#pragma warning (disable:4251) // std + dll warning
			std::string	mSource;
			std::string mFilename;
			#pragma warning (default:4251) // std + dll warning

			EType mShaderType;
		};

		class API  ShaderProgram : public virtual GL::Rtti
		{
		public:
			ShaderProgram();
			ShaderProgram(Shader* vertexShader, Shader* fragmentShader);
			ShaderProgram(const char * filename);
			~ShaderProgram();

			void Bind()const;
			void Unbind()const;
			void Load(const char* filename);

			void SetUniform(const char* name, const int& value)const;
			void SetUniform(const char* name, const unsigned int& value)const;
			void SetUniform(const char* name, const float& value)const;
			void SetUniform(const char* name, const glm::mat4& value)const;
			void SetUniform(const char* name, const glm::mat3& value)const;
			void SetUniform(const char* name, const glm::vec2& value)const;
			void SetUniform(const char* name, const glm::vec3& value)const;
			void SetUniform(const char* name, const Color& value)const;
			void SetUniform(const char* name, const bool& value)const;


			unsigned int GetOpenGLHandle()const;

		protected:
			friend class Graphics;
			unsigned int mGLHandle;
			void Link(Shader* vertexShader, Shader* fragmentShader)const;

		};
	}
}

//-----------------------------------------------------------------------------
#endif