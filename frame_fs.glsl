#version 460 core

#define WIDTH 1200

out vec4 fragColor;

in vec2 texUV;

uniform sampler2D frame;

float offset = 1.0f/WIDTH;

vec2 offsets[9] = {
	vec2(-offset, offset),
	vec2(0.0f, offset),
	vec2(offset, offset),
	vec2(-offset, 0.0f),
	vec2(0.0f, 0.0f),
	vec2(offset, 0.0f),
	vec2(-offset, -offset),
	vec2(0.0f, -offset),
	vec2(offset, -offset)
};

float kernel[9] = {
    -1, -1, -1,
    -1,  9, -1,
    -1, -1, -1	
};	

void main(){
	vec3 color = vec3(0.0f);

	for(int i=0; i < 9; i++){
		color += vec3(texture(frame, texUV + offsets[i])) * kernel[i];
	}

	fragColor = vec4(color, 1.0f);
	//fragColor = vec4(1.0f, 0.0f, 0.0f, 0.0f);
}
