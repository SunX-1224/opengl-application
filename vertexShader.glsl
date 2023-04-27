#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 atexUV;

out vShaderOut{
	vec3 pos;
	vec3 normal;
	vec3 color;
	vec2 texUV;
} vshader_out;

uniform float u_time;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	vshader_out.texUV = mat2(0.0f, -1.0f, 1.0f, 0.0f) * atexUV;
	vshader_out.normal = mat3(transpose(inverse(model))) * aNormal;
	vshader_out.color = aColor;
	vshader_out.pos = vec3(model * 3.0f * vec4(aPos, 1.0f)) - vec3(0.0f, 3.5f, 0.0f);
	
	gl_Position = projection * view * vec4(vshader_out.pos, 1.0f);
}