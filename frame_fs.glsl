#version 460 core

out vec4 fragColor;

in vec2 texUV;

uniform sampler2D frame;

void main(){
	vec3 color = vec3(texture(frame, texUV));
	fragColor = vec4(color, 1.0f);
}
