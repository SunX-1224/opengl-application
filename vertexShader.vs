#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 atexUV;

out vec3 pos;
out vec3 normal;
out vec3 color;
out vec2 texUV;

uniform float u_time;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	texUV = mat2(0.0f, -1.0f, 1.0f, 0.0f) * atexUV;
	normal = mat3(transpose(inverse(model))) * aNormal;
	color = aColor;
	pos = vec3(model * vec4(aPos, 1.0f));
	
	gl_Position = projection * view * vec4(pos, 1.0f);
}