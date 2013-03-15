#version 330 core

uniform mat4 MVPMatrix;

layout(location = 0) in vec3 in_Position;

void main(void){
	
	gl_Position = MVPMatrix * vec4(1.0 * in_Position, 1.0);
}