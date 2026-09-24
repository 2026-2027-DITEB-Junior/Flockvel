#include "pch.h"
#include <GL/glew.h>
#include <GL/wglew.h>
#include "AEXCheckGLError.h"
#include "AEXModel.h"
#include <cstddef>
#include <iostream>
#define TINYOBJLOADER_IMPLEMENTATION
#include "../tiny_obj_loader.h"

namespace clc
{
	namespace GL
	{
		Model::Model()
			: mPrimitiveType(eTriangleList)
			, mDrawMode(eDM_Fill)
		{
			CreateOpenGLModel();
		}
		Model::Model(std::string modelPath)
			: Model()
		{
			LoadFromFile(modelPath.c_str());
		}
		Model::Model(EPrimitiveType primType)
			: mPrimitiveType(primType)
			, mDrawMode(eDM_Fill)
		{
			CreateOpenGLModel();
		}
		Model::~Model()
		{
			Clear();
			DestroyOpenGLModel();
		}

		// TODO
		///--------------------------------------------------------------------
		/// \fn GetVertex (const)
		/// \brief Returns a pointer to the vertex  at the specified offset
		/// \param offset Zero based offset in the vertex array.
		const Vertex* Model::GetVertex(unsigned int offset) const
		{
			// Sanity Check
			if (offset >= GetVertexCount())
				return NULL;

			// return vertex
			return (mVertices.data() + offset);
		}


		// TODO
		///--------------------------------------------------------------------
		/// \fn GetVertex 
		/// \brief Returns a pointer to the vertex  at the specified offset
		/// \param offset Zero based offset in the vertex array.
		Vertex* Model::GetVertex(unsigned int offset)
		{
			// Sanity Check
			if (offset >= GetVertexCount())
				return NULL;

			// return vertex
			return (mVertices.data() + offset);
		}

		// TODO
		///--------------------------------------------------------------------
		/// \fn SetVertexPos
		void Model::SetVertexPos(unsigned int offset, glm::vec3 pos)
		{
			if (Vertex* vtx = GetVertex(offset))
				vtx->mPosition = pos;
		}

		// TODO
		///--------------------------------------------------------------------
		/// \fn SetVertexPos
		void Model::SetVertexPos(unsigned int offset, float x, float y, float z)
		{
			SetVertexPos(offset, glm::vec3(x, y, z));
		}

		// TODO
		///--------------------------------------------------------------------
		/// \fn SetVertexTex
		void Model::SetVertexTex(unsigned int offset, glm::vec2 texCoord)
		{
			if (Vertex* vtx = GetVertex(offset))
				vtx->mTexCoord = texCoord;
		}

		// TODO
		///--------------------------------------------------------------------
		/// \fn SetVertexTex
		void Model::SetVertexTex(unsigned int offset, float u, float v)
		{
			SetVertexTex(offset, glm::vec2(u, v));
		}

		// TODO
		///--------------------------------------------------------------------
		/// \fn SetVertexColor
		void Model::SetVertexColor(unsigned int offset, Color col)
		{
			if (Vertex* vtx = GetVertex(offset))
				vtx->mColor = col;
		}

		// TODO
		///--------------------------------------------------------------------
		/// \fn SetVertexColor
		void Model::SetVertexColor(unsigned int offset, float r, float g, float b, float a)
		{
			SetVertexColor(offset, Color(r, g, b, a));
		}

		// TODO
		///--------------------------------------------------------------------
		/// \fn GetVertexCount (const)
		/// \brief Returns the number of vertices in the model
		unsigned int		Model::GetVertexCount() const
		{
			return (unsigned int)mVertices.size();
		}

		// TODO
		///--------------------------------------------------------------------
		/// \fn		AddVertex
		/// \brief	Inserts the given vertex at the end of the vertex list. 
		/// \param	newVert the vertex to add
		void Model::AddVertex(const Vertex& newVert)
		{
			mVertices.push_back(newVert);
		}
		
		//----------- Done by JosuO (Hope no one sees this) ---------//
		void Model::RemoveLastVertex()
		{
			mVertices.pop_back();
		}

		///--------------------------------------------------------------------
			/// \fn		AddVertex						BY JOSU O
		void Model::RemoveQuadPos(unsigned int i)
		{
			if ((size_t)(i + 5) < mVertices.size())
			{
				auto delVertStart = mVertices.begin() + i;
				auto delVertEnd = mVertices.begin() + i + 6;
				mVertices.erase(delVertStart, delVertEnd);
			}
		}

		// TODO
		///--------------------------------------------------------------------
		/// \fn		Clear
		/// \brief	Clears all vertices
		void Model::Clear()
		{
			mVertices.clear();
		}

		/// \fn		LoadFromFile
		/// \brief	Loads an .obj and uploads it to the GPU
		void Model::LoadFromFile(const char* filename)
		{
			tinyobj::attrib_t attrib;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> materials;
			std::string warn;
			std::string err;

			if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename))
			{
				std::cerr << "failed to load .obj file" << std::endl;
				return;
			}

			Clear();

			// one vertex per index
			for (auto& s : shapes)
			{
				for (auto& i : s.mesh.indices)
				{
					Vertex vtx;

					vtx.mPosition = glm::vec3(attrib.vertices[3 * i.vertex_index + 0],
						attrib.vertices[3 * i.vertex_index + 1], attrib.vertices[3 * i.vertex_index + 2]);

					if (i.texcoord_index >= 0)
					{
						vtx.mTexCoord = glm::vec2(attrib.texcoords[2 * i.texcoord_index + 0],
							attrib.texcoords[2 * i.texcoord_index + 1]);
					}

					if (i.normal_index >= 0)
					{
						vtx.mNormal = glm::vec3(attrib.normals[3 * i.normal_index + 0],
							attrib.normals[3 * i.normal_index + 1], attrib.normals[3 * i.normal_index + 2]);
					}

					// white if the obj has no colors
					vtx.mColor = Color(attrib.colors[3 * i.vertex_index + 0],
						attrib.colors[3 * i.vertex_index + 1], attrib.colors[3 * i.vertex_index + 2]);

					AddVertex(vtx);
				}
			}

			UploadToGPU();
		}

		// Create OpenGL
		void Model::CreateOpenGLModel()
		{
			GLuint vao;
			GLuint vertexBuffer;
			GLuint indexBuffer;

			// Create vertex array object and bind
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			// Create vertex buffer
			glGenBuffers(1, &vertexBuffer);

			// Create index buffer
			glGenBuffers(1, &indexBuffer);


			mGLVAO = vao;
			mGLVertexBuffer = vertexBuffer;
			mGLIndexBuffer = indexBuffer;
		}
		void Model::DestroyOpenGLModel()
		{
			glDeleteBuffers(1, &mGLIndexBuffer);
			glDeleteBuffers(1, &mGLVertexBuffer);
			glDeleteVertexArrays(1, &mGLVAO);
		}

		///--------------------------------------------------------------------
		/// \fn		UploadVertexFormatToGPU
		void Model::UploadVertexFormatToGPU()const
		{
			unsigned int		vertexSize = sizeof(Vertex);
			// Specify how the data for the vertices is layed out
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, mPosition))); // position
			check_gl_error();
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, mTexCoord))); // texture coord
			check_gl_error();
			glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(sizeof(glm::vec2) + sizeof(glm::vec3))); // color
			check_gl_error();
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, mNormal))); // normal
			check_gl_error();
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);
			check_gl_error();
		}

		///--------------------------------------------------------------------
		/// \fn		UploadToGPU
		/// \brief	
		void Model::UploadToGPU()const
		{
			// Sanity Checks
			if (mGLVAO == 0)
				return;

			GLuint	vertexBuffer = (GLuint)(mGLVertexBuffer);
			GLuint	indexBuffer = (GLuint)(mGLIndexBuffer);

			// Bind
			Bind();

			// upload vertex data as is
			unsigned int		vertexSize = sizeof(Vertex);
			unsigned int		vertexBufferSize = vertexSize * (unsigned int)mVertices.size();
			glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, reinterpret_cast<const void*>(mVertices.data()), GL_STATIC_DRAW);
			check_gl_error();

			// Specify how the data for the vertices is layed out
			UploadVertexFormatToGPU();

			// compute index dataand upload to GL device
			unsigned int	indexBufferSize = (unsigned int)mVertices.size();
			unsigned int* indexBufferData = new unsigned int[indexBufferSize];
			for (unsigned int i = 0; i < indexBufferSize; ++i)
				indexBufferData[i] = i;

			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexBufferSize, reinterpret_cast<void*>(indexBufferData), GL_STATIC_DRAW);
			check_gl_error();

			// delete the index buffer data now that we've sent it to the GPU
			delete[] indexBufferData;

			// Unbind
			Unbind();
		}

		void Model::ReloadToGPU(int start, int end)
		{
			// Sanity check
			if (end < start || start >= (int)GetVertexCount())
				return;

			// upload all
			if (start == -1 && end == -1)
				end = (int)GetVertexCount() - 1; // set end to last vertex

			// clamp to end
			if (end >= (int)GetVertexCount())
				end = (int)GetVertexCount() - 1;

			// count of vertices to upload
			unsigned int toUploadCount = end - start + 1;

			// get the vertex buffer handles
			GLuint	vertexBuffer = (GLuint)(mGLVertexBuffer);

			// Bind
			Bind();

			// upload vertex data as is
			unsigned int		vertexSize = sizeof(Vertex);
			unsigned int		vertexBufferSize = vertexSize * toUploadCount;
			unsigned int		startOffset = start * vertexSize;
			//glBufferSubData(GL_ARRAY_BUFFER, startOffset, vertexBufferSize, reinterpret_cast<void*>(mVertices.data() + startOffset));
			glBufferSubData(GL_ARRAY_BUFFER, startOffset, vertexBufferSize, reinterpret_cast<void*>(((char*)mVertices.data()) + startOffset));
			//check_gl_error();
		}

		///--------------------------------------------------------------------
		/// \fn		Bind
		/// \brief	
		void Model::Bind()const
		{
			if (mGLVAO) {
				glBindVertexArray(mGLVAO);
				glBindBuffer(GL_ARRAY_BUFFER, (GLuint)(mGLVertexBuffer));
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)(mGLIndexBuffer));
			}
		}

		///--------------------------------------------------------------------
		/// \fn		Unbind
		/// \brief	
		void Model::Unbind()const
		{
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}


		///--------------------------------------------------------------------
		/// \fn		GetGLHandleVAO()
		/// \brief	Returns the VAO handle
		unsigned int Model::GetGLHandleVAO()const
		{
			return mGLVAO;
		}

		// TODO
		///--------------------------------------------------------------------
		/// \fn		Draw
		/// \brief	Draws the model taken into account mPrimitiveType
		void Model::Draw(int startOffset, int endOffset)const
		{

			// Sanity check
			if (endOffset < startOffset || startOffset >= (int)GetVertexCount())
				return;

			// draw all
			if (startOffset == -1 && endOffset == -1)
			{
				startOffset = 0;
				endOffset = (int)GetVertexCount() - 1; // set end to last vertex
			}

			// clamp to end
			if (endOffset >= (int)GetVertexCount())
				endOffset = (int)GetVertexCount() - 1;

			// count of vertices to draw
			unsigned int toDrawCount = endOffset - startOffset + 1;

			// Bind the vertex arrays
			Bind();
			//check_gl_error();

			// Set the fill mode
			// TODO(Thomas): Prevent redundant calls. 
			switch (mDrawMode)
			{
			case eDM_Wireframe:
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;
			case eDM_Fill:
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				break;
			}

			// Draw
			
			switch (mPrimitiveType)
			{
			case eTriangleList:
				glDrawArrays(GL_TRIANGLES, startOffset, toDrawCount);
				break;
			case eLineList:
				glDrawArrays(GL_LINES, startOffset, toDrawCount);
				break;
			}
			check_gl_error();
		}

		///--------------------------------------------------------------------
		/// \fn		DrawInstanced
		void Model::DrawInstanced(unsigned int instanceCount, int startOffset, int endOffset)const
		{
			// Sanity check
			if (0 == instanceCount && endOffset < startOffset || startOffset >= (int)GetVertexCount())
				return;

			// draw all
			if (startOffset == -1 && endOffset == -1)
			{
				startOffset = 0;
				endOffset = (int)GetVertexCount() - 1; // set end to last vertex
			}

			// clamp to end
			if (endOffset >= (int)GetVertexCount())
				endOffset = (int)GetVertexCount() - 1;

			// count of vertices to draw
			unsigned int toDrawCount = endOffset - startOffset + 1;

			// Bind the vertex arrays
			Bind();
			check_gl_error();

			// Set the fill mode
			// TODO(Thomas): Move this to graphics system 
			switch (mDrawMode)
			{
			case eDM_Wireframe:
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;
			case eDM_Fill:
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				break;
			}

			// Draw

			switch (mPrimitiveType)
			{
			case eTriangleList:
				glDrawElementsInstancedBaseVertex(GL_TRIANGLES, toDrawCount, GL_UNSIGNED_INT, 0, instanceCount, startOffset);
				break;
			case eLineList:
				glDrawElementsInstancedBaseVertex(GL_LINES, toDrawCount, GL_UNSIGNED_INT, 0, instanceCount, startOffset);
				break;
			}
			check_gl_error();

		}
	}
}// namespace Rasterizer