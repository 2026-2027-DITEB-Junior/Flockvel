#version 330

in vec3 pos;						// texture coord (interpolated per fragment)

uniform samplerCube skyboxTex;

out vec4 color;					// final fragment color (output of that shader)

void main(){
	color = texture(skyboxTex, pos);
	return;
}