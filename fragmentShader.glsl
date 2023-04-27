#version 460 core

struct PointLight{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float linear;
	float quadratic;
};

struct DirectionLight{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

const float near = 0.1f;
const float far = 10.0f;

out vec4 fragColor;

in vShaderOut{
	vec3 pos;
	vec3 normal;
	vec3 color;
	vec2 texUV;
} vshader_out;

uniform float u_time;
uniform PointLight light;
uniform vec3 camera_position;
uniform sampler2D diffuse0;
uniform sampler2D specular0;

vec3 calculatePointLight(PointLight, vec3, vec3);
vec3 calculateDirLight(DirectionLight, vec3, vec3);
float linearizeDepth(float);
float logisticDepth(float, float, float);


void main(){
	vec3 viewDirection = normalize(camera_position - vshader_out.pos);
	vec3 result = calculatePointLight(light, vshader_out.normal, viewDirection);
	//vec3 result = calculateDirLight(light, vshader_out.normal, viewDirection);

	//fragColor = texture(diffuse0, vshader_out.texUV);
	//fragColor = vec4(result, 1.0f);
	float z = linearizeDepth(gl_FragCoord.z);
	z = logisticDepth(z, 0.5f, 5.0f);

	//result = vec3(1.0f - z);
	result = result * (1.0f - z) + z * vec3(0.1f);
		
	fragColor = vec4(result, 1.0f);
}



vec3 calculateDirLight(DirectionLight light, vec3 normal, vec3 viewDir){
	vec3 lightDir = normalize(light.direction);
	vec3 reflectDir = reflect(lightDir, normal);

	float diffuse = max(0.0f, dot(viewDir, normal));
	float specular = pow(max(0.0f, dot(viewDir, reflectDir)), 32.0f);

	vec3 result = (light.ambient + light.diffuse * diffuse) * vec3(texture(diffuse0, vshader_out.texUV));
	result += specular * light.specular * vec3(texture(specular0, vshader_out.texUV).x);

	return result;
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 viewDir){
	vec3 lightDir = normalize(vshader_out.pos - light.position);
	float distance = length(vshader_out.pos - light.position);
	vec3 reflectDir = reflect(lightDir, normal);

	float diffuse = max(0.0f, dot(-lightDir, normal));
	float specular = pow(max(0.0f, dot(viewDir, reflectDir)), 32.0f);

	vec3 result = (light.ambient + light.diffuse * diffuse) * vec3(texture(diffuse0, vshader_out.texUV));
	result += specular * light.specular * vec3(texture(specular0, vshader_out.texUV).x);

	result /= (1.0f + distance * light.linear + distance*distance * light.quadratic);

	return result;
}

float logisticDepth(float z, float steepness, float offset){
	return 1 / (1.0f + exp(-steepness * (z - offset)));
}

float linearizeDepth(float z){
	return near * far / (far * (1 - z) + near * z);
}