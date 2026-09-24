#ifndef AEX_VERTEX_H_
#define AEX_VERTEX_H_

#include "AEXGL_DLL.h"
#include "AEXColor.h"

namespace clc
{
	namespace GL
	{
		/// \class	Vertex
		///	\brief	Represents a vertex with position, texture coordinates and color
		struct  API Vertex
		{
			Vertex(glm::vec3 pos = glm::vec3(0), glm::vec2 tex = glm::vec3(0), Color col = Color(), glm::vec3 normal = glm::vec3(0))
				:mPosition(pos), mTexCoord(tex), mColor(col), mNormal(normal) {}
			glm::vec3 mPosition;	//! x,y,z
			glm::vec2 mTexCoord;	//! u,v
			Color  mColor;		//! r,g,b,a
			glm::vec3 mNormal;	//! nx,ny,nz
		};
	}
}

#endif