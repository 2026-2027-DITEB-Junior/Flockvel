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
#include "AEXShader.h"
#include "AEXColor.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include "../../Engine/Serialization.h"
#include <cassert>
#include <filesystem>
#include "../../Resources/ResourceManager.h"

namespace clc
{
	namespace GL
	{
        namespace
        {
            std::string ResolveShaderPath(const char* descriptor, const std::string& source)
            {
                // Primero buscar junto al .shader; despues, en los recursos del proyecto.
                std::filesystem::path folder = std::filesystem::u8path(descriptor).parent_path();
                std::filesystem::path candidate = folder / std::filesystem::u8path(source);
                if (std::filesystem::is_regular_file(candidate))
                    return candidate.u8string();
                return clcResources.ResolvePath(source);
            }
        }

		Shader::Shader()
			: mShaderType(eTypeCount)
			, mGLHandle(0)
		{}
		Shader::Shader(const char* filename, EType shaderType)
			: mShaderType(shaderType), mGLHandle(0)
		{
			mFilename = filename;

			// Load
			Load(filename);
		}
		Shader::~Shader()
		{
			if (mGLHandle)
				glDeleteShader(mGLHandle);
		}
		void Shader::LoadSource(const char* filename)
		{
			if (filename)
				mFilename = filename;

			// open the shader file & load its content
			std::fstream shaderFile(std::filesystem::u8path(mFilename), std::ios::in);
			std::stringstream shaderSource;
			shaderSource << shaderFile.rdbuf();

			// save
			mSource = shaderSource.str();
		}
		void Shader::Compile()
		{
			if (mSource.empty())
				return;

			// create shader (delete if necessary)
			// NOTE(Thomas):this is here because the shader might not have been 
			// created yet (when calling default constructor for eample)
			bool created = mGLHandle != 0;
			SetShaderType(mShaderType, !created);

			// pass the code to OpenGl
			const char* shaderSourceStr = mSource.c_str();
			glShaderSource(mGLHandle, 1, &shaderSourceStr, NULL);

			// compile
			glCompileShader(mGLHandle);

			// sanity check
			GLint result;
			glGetShaderiv(mGLHandle, GL_COMPILE_STATUS, &result);
			if (GL_FALSE == result)
			{
				// error log
				GLint logLen;
				glGetShaderiv(mGLHandle, GL_INFO_LOG_LENGTH, &logLen);
				if (logLen > 0)
				{
					char* log = (char*)malloc(logLen);
					GLsizei written;
					glGetShaderInfoLog(mGLHandle, logLen, &written, log);
					char str[100];
					sprintf_s(str, "Shader Compilation Error: %s", mFilename.c_str());
					MessageBoxA(NULL, log, str, MB_TASKMODAL | MB_SETFOREGROUND | MB_ICONERROR);
					free(log);
				}
			}

		}
		void Shader::Load(const char* filename)
		{
			LoadSource(filename);
			Compile();
		}
		void Shader::CreateDeviceShader()
		{
			if (mGLHandle)
				glDeleteShader(mGLHandle);
			// create a new shader
			GLenum type = mShaderType == eVertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
			mGLHandle = glCreateShader(type);
		}
		void Shader::SetShaderType(Shader::EType shaderType, bool createDeviceShader)
		{
			// store type
			mShaderType = shaderType;

			// new shader otherwise - note that if the shader is attached
			// to a shader program, this program might stop working properly.
			// because the shader is no longer valid. 
			if (createDeviceShader)
				CreateDeviceShader();
		}
		ShaderProgram::ShaderProgram()
			: mGLHandle(0)
		{
			// create the shader program
			mGLHandle = glCreateProgram();
		}
		ShaderProgram::ShaderProgram(Shader* vertexShader, Shader* fragmentShader)
			: ShaderProgram()
		{
			
			// Link with shaders right away
			Link(vertexShader, fragmentShader);
		}
		ShaderProgram::ShaderProgram(const char* filename)
			:ShaderProgram()
		{
			Load(filename);
		}
		ShaderProgram::~ShaderProgram()
		{
			if (mGLHandle)
				glDeleteProgram(mGLHandle);
		}
		void ShaderProgram::Link(Shader* vertexShader, Shader* fragmentShader)const
		{
			glAttachShader(mGLHandle, vertexShader->mGLHandle);
			glAttachShader(mGLHandle, fragmentShader->mGLHandle);

			// link the shader
			glLinkProgram(mGLHandle);

			// error check
			GLint status;
			glGetProgramiv(mGLHandle, GL_LINK_STATUS, &status);
			if (GL_FALSE == status)
			{
				GLint logLen;
				glGetProgramiv(mGLHandle, GL_INFO_LOG_LENGTH, &logLen);
				if (logLen > 0)
				{
					char* log = (char*)malloc(logLen);
					GLsizei written;
					glGetProgramInfoLog(mGLHandle, logLen, &written, log);
					MessageBoxA(NULL, log, "Shader Linking Error", MB_TASKMODAL | MB_SETFOREGROUND | MB_ICONERROR);
					free(log);
				}
			}

			// detach shaders
			glDetachShader(mGLHandle, vertexShader->mGLHandle);
			glDetachShader(mGLHandle, fragmentShader->mGLHandle);
		}
		void ShaderProgram::Bind()const
		{
			glUseProgram(mGLHandle);
		}
		void ShaderProgram::Unbind()const
		{
			glUseProgram(NULL);
		}
		void ShaderProgram::Load(const char * filename)
		{
			// read shader program file into memory (json parser). 
			nlohmann::json j;
			if (!ReadJsonFile(filename, j))
				return;

			// missing files -> nothing to link
			if (!j.contains("vertexShader") || !j.contains("fragmentShader")) {
				std::cout << "bad shader program: " << filename << std::endl;
				return;
			}

            // read vertex and fragment shader files
			Shader* vert = nullptr, * frag = nullptr;
			{
				// load vertex shader
				std::string shaderFile = j["vertexShader"];						// read vert shader file path from json
				shaderFile = ResolveShaderPath(filename, shaderFile);
				vert = new Shader(shaderFile.c_str(), Shader::eVertex);

				// load fragment shader
				shaderFile = j["fragmentShader"];								// read fragment shader file path from json
				shaderFile = ResolveShaderPath(filename, shaderFile);
				frag = new Shader(shaderFile.c_str(), Shader::eFragment);
			}

			// sanity check
			assert(vert && frag);
			if (vert && frag)
			{
				Link(vert, frag);
			}

			// clean up
			if (vert)delete vert;
			if (frag)delete frag;
		}

		unsigned int ShaderProgram::GetOpenGLHandle()const
		{
			return mGLHandle;
		}

		void ShaderProgram::SetUniform(const char* name, const int& value)const
		{
			GLint loc = glGetUniformLocation(mGLHandle, name);
			if (loc != -1) {
				check_gl_error();
				glUniform1iv(loc, 1, &value);
				check_gl_error();
			}
			//else
				//std::cout << "ShaderProgram: Uniform " << name << "not found" <<"\n";
		}
		void ShaderProgram::SetUniform(const char* name, const unsigned int& value)const
		{
			GLint loc = glGetUniformLocation(mGLHandle, name);
			if (loc != -1) {
				check_gl_error();
				glUniform1uiv(loc, 1, &value);
				check_gl_error();
			}
			else
				std::cout << "ShaderProgram: Uniform " << name << "not found" <<"\n";
		}
		void ShaderProgram::SetUniform(const char* name, const float& value)const
		{
			GLint loc = glGetUniformLocation(mGLHandle, name);
			if (loc != -1) {
				check_gl_error();
			glUniform1fv(loc, 1, &value);
			check_gl_error();
			}
			else
				std::cout << "ShaderProgram: Uniform " << name << "not found" <<"\n";
		}
		void ShaderProgram::SetUniform(const char* name, const glm::mat4& value)const
		{
			GLint loc = glGetUniformLocation(mGLHandle, name);
			if (loc != -1) {
				check_gl_error();
			glUniformMatrix4fv(loc, 1, GL_FALSE, &value[0].x); // <--------------- If you are here you are searching for this
			check_gl_error();
			}
			else
				std::cout << "ShaderProgram: Uniform " << name << "not found" <<"\n";
		}
		void ShaderProgram::SetUniform(const char* name, const glm::mat3& value)const
		{
			GLint loc = glGetUniformLocation(mGLHandle, name);
			if (loc != -1) {
				check_gl_error();
			glUniformMatrix3fv(loc, 1, GL_FALSE, &value[0].x); // < --------------- Not repeating it twice
			check_gl_error();
			}
			else
				std::cout << "ShaderProgram: Uniform " << name << "not found" <<"\n";
		}
		void ShaderProgram::SetUniform(const char* name, const glm::vec2& value)const
		{
			GLint loc = glGetUniformLocation(mGLHandle, name);
			if (loc != -1) {
				check_gl_error();
			glUniform2fv(loc, 1, &value.x);
			check_gl_error();
			}
			else
				std::cout << "ShaderProgram: Uniform " << name << "not found" <<"\n";
		}
		void ShaderProgram::SetUniform(const char* name, const glm::vec3& value)const
		{
			GLint loc = glGetUniformLocation(mGLHandle, name);
			if (loc != -1) {
				check_gl_error();
			glUniform3fv(loc, 1, &value.x);
			check_gl_error();
			}
			else
				std::cout << "ShaderProgram: Uniform " << name << "not found" <<"\n";
		}
		void ShaderProgram::SetUniform(const char* name, const Color& value)const
		{
			GLint loc = glGetUniformLocation(mGLHandle, name);
			if (loc != -1) {
				check_gl_error();
				glUniform4fv(loc, 1, value.v);
				check_gl_error();
			}
			else
				std::cout << "ShaderProgram: Uniform " << name << "not found" <<"\n";

		}
		void ShaderProgram::SetUniform(const char* name, const bool& value)const
		{
			int ib = value ? 1 : 0;
			SetUniform(name, ib);
		}
	}
}