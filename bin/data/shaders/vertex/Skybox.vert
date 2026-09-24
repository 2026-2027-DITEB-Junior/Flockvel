#version 330

layout(location = 0) in vec3 vertexPos;

out vec3 pos;		// texture coordinates (output to pixel shader)

uniform mat4 mtxModel;		// model view projection
uniform mat4 mtxView;	// model view projection
uniform mat4 mtxProj;	// model view projection

void main()
{
	// vertex shaders must at least write to gl_Position
	gl_Position =  mtxProj * vec4((mtxView * mtxModel * vec4(vertexPos,0)).xyz, 1);
	pos = vec3(vertexPos.x, -vertexPos.y, vertexPos.z);
}