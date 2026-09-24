#version 330

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUV;

uniform vec2 size;

out vec2 UV;		// texture coordinates (output to pixel shader)
//uniform mat4 mtxModel;		// model view projection

void main()
{
	// vertex shaders must at least write to gl_Position
	gl_Position =  vec4(vertexPos.x * size.x, vertexPos.y * size.y, 0.0f ,1.0f); 

	UV = vertexUV;
}