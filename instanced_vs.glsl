#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 atexUV;
layout(location = 4) in mat4 instanceMat;

out vShaderOut{
	vec3 pos;
	vec3 normal;
	vec3 color;
	vec2 texUV;
} vshader_out;

uniform float u_time;
uniform mat4 view;
uniform mat4 projection;

void main() {
	vshader_out.texUV = mat2(0.0f, -1.0f, 1.0f, 0.0f) * atexUV;
	vshader_out.normal = aNormal;
	vshader_out.color = aColor;
	vshader_out.pos = vec3(instanceMat * vec4(aPos, 1.0f));

	gl_Position = projection * view * vec4(vshader_out.pos, 1.0f);
}