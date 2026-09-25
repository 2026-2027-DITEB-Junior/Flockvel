/* ---------------------------------------------------------------------------------------------------------
Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
Language: OpenGL
Platform: Windows 11
Project: cs462_s.portilla_1
Author: Sergio Portilla, s.portilla, 540002021
----------------------------------------------------------------------------------------------------------*/
#version 400

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in vec4 aTangent;

out vec3 FragPos; // cam space
out vec3 Normal;
out vec3 Tangent;
out vec3 Bitangent;
out vec2 TexCoord;

uniform mat4 modelView;
uniform mat4 projection;

void main() {
	TexCoord = aUV;

	// vtx pos to cam space
	vec4 camSpacePos = modelView * vec4(aPos, 1.0);
	FragPos = camSpacePos.xyz;

	// normal and tangent to cam space
	mat3 normalMatrix = mat3(transpose(inverse(modelView)));

	Normal = normalize(normalMatrix * aNormal);
	Tangent = normalize(mat3(modelView) * aTangent.xyz);

	// w is the handedness
	Bitangent = cross(Normal, Tangent) * aTangent.w;

	gl_Position = projection * camSpacePos;
}