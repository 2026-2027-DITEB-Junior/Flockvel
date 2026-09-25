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

in vec3 FragPos;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;
in vec2 TexCoord;

// the 3 targets of the gbuffer
layout(location = 0) out vec4 gPosition; // rgb: cam space pos
layout(location = 1) out vec4 gNormal; // rgb: cam space normal, a: shininess (ns)
layout(location = 2) out vec4 gDiffuse; // rgb: diffuse color, a: specular (ks)

uniform vec4 diffuseColor;
uniform sampler2D diffuseTex;
uniform sampler2D specularTex;
uniform sampler2D normalTex;

void main() {
	vec4 diffuse = diffuseColor * texture(diffuseTex, TexCoord);

	// alpha test
	if (diffuse.a < 0.5)
		discard;

	// blue is the ks and green the ns
	vec3 material = texture(specularTex, TexCoord).rgb;
	float specular = max(material.b, 0.0);
	float shininess = clamp(material.g, 0.0, 1.0) * 20.0 + 5.0;

	// normal map from tangent space to cam space
	vec3 tangentNormal = texture(normalTex, TexCoord).xyz * 2.0 - 1.0;
	mat3 TBN = mat3(normalize(Tangent), normalize(Bitangent), normalize(Normal));

	gPosition = vec4(FragPos, 1.0);
	gNormal = vec4(normalize(TBN * tangentNormal), shininess);
	gDiffuse = vec4(diffuse.rgb, specular);
}