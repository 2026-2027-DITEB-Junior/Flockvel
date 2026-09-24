#pragma once
#include <glm/glm.hpp>
#include "AEXGL_DLL.h"
#include "AEXColor.h"
#include "AEXVertex.h"
#include "AEXGLRtti.h"
#include <vector>

namespace clc
{
	namespace GL
	{
		/// \class	Model
		/// \brief	Model class use to represent a list of primitives. Lists that are accepted are
		///			pointlist, linelist, triangle list and circle and ellipse list. 
		class  API Model : public virtual GL::Rtti
		{
		public:
			const char* mFilePath = "quad";		// File path
			enum EPrimitiveType
			{
				ePointList,
				eLineList,
				eTriangleList,
			};

			enum EDrawMode
			{
				eDM_Fill = 0,
				eDM_Wireframe,
				eDM_Count
			};

			// PROVIDED: Dflt and only Constructor
			// The vertices should be added using the functions below
			Model();
			Model(std::string modelPath);
			Model(EPrimitiveType primType);
			virtual ~Model();

			#pragma region// Getters/Setters
			EPrimitiveType GetPrimitiveType() const			{ return mPrimitiveType; }
			void SetPrimitiveType(EPrimitiveType primType)	{ mPrimitiveType = primType; }
			EDrawMode GetDrawMode()	const					{ return mDrawMode; }
			void SetDrawMode(EDrawMode drawMode)			{ mDrawMode = drawMode; }
			#pragma endregion

			///--------------------------------------------------------------------
			/// \fn GetVertex (const)
			const Vertex * GetVertex(unsigned int offset = 0) const;

			///--------------------------------------------------------------------
			/// \fn GetVertex 
			Vertex * GetVertex(unsigned int offset = 0);	

			///--------------------------------------------------------------------
			/// \fn SetVertexPos
			void SetVertexPos(unsigned int offset, glm::vec3 pos);

			///--------------------------------------------------------------------
			/// \fn SetVertexPos
			void SetVertexPos(unsigned int offset, float x, float y, float z);

			///--------------------------------------------------------------------
			/// \fn SetVertexTex
			void SetVertexTex(unsigned int offset, glm::vec2 texCoord);

			///--------------------------------------------------------------------
			/// \fn SetVertexTex
			void SetVertexTex(unsigned int offset, float u, float v);

			///--------------------------------------------------------------------
			/// \fn SetVertexColor
			void SetVertexColor(unsigned int offset, Color col);

			///--------------------------------------------------------------------
			/// \fn SetVertexColor
			void SetVertexColor(unsigned int offset, float r, float g, float b, float a);


			///--------------------------------------------------------------------
			/// \fn GetVertexCount (const)
			unsigned int		GetVertexCount() const;

			///--------------------------------------------------------------------
			/// \fn		AddVertex
			void AddVertex(const Vertex & newVert);

			///--------------------------------------------------------------------
			/// \fn		AddVertex						BY JOSU O <-- Punpun dies
			void RemoveLastVertex();

			///--------------------------------------------------------------------
			/// \fn		AddVertex						BY JOSU O
			void RemoveQuadPos(unsigned int i = 0);


			///--------------------------------------------------------------------
			/// \fn		Clear
			void Clear();

			///--------------------------------------------------------------------
			/// \fn		LoadFromFile
			void LoadFromFile(const char* filename);

			///--------------------------------------------------------------------
			/// \fn		Draw
			void Draw(int startOffset = -1, int endOffset = -1)const;

			///--------------------------------------------------------------------
			/// \fn		DrawInstanced
			void DrawInstanced(unsigned int instanceCount, int startOffset = -1, int endOffset = -1)const;

			///--------------------------------------------------------------------
			// \brief Device Interface functions

			///--------------------------------------------------------------------
			/// \fn		UploadVertexFormatToGPU
			void UploadVertexFormatToGPU()const;

			///--------------------------------------------------------------------
			/// \fn		UploadToGPU
			void UploadToGPU()const;

			///--------------------------------------------------------------------
			/// \fn		ReloadToGPU
			void ReloadToGPU(int start = -1, int end = -1);

			///--------------------------------------------------------------------
			/// \fn		Bind
			void Bind() const;

			///--------------------------------------------------------------------
			/// \fn		Unbind
			void Unbind() const;

			///--------------------------------------------------------------------
			/// \fn		GetGLHandleVAO()
			/// \brief	Returns the VAO handle
			unsigned int GetGLHandleVAO() const;

		protected:
			// Primitive type
			EPrimitiveType		mPrimitiveType;

			// Vertex Buffer - Interleaved
			// Note(Thomas): The format of the vertex is set. 
			// Unless your game requires mesh manipulation 
			// or advanced shading. This Vertex is more than enough.
			#pragma warning (disable:4251) // std + dll warning
			std::vector<Vertex> mVertices;
			#pragma warning (default:4251) // std + dll warning

			// Draw mode
			EDrawMode				mDrawMode;

			// OpenGL data
			unsigned int	mGLVAO;				// vertex array object
			unsigned int mGLVertexBuffer;	// vertex buffer object
			unsigned int mGLIndexBuffer;		// index buffer object

			// Create OpenGL 
			void CreateOpenGLModel();
			void DestroyOpenGLModel();
		};
	}
}

