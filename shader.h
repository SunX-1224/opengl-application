#ifndef SHADER_H
#define SHADER_H

#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

#include<glad/glad.h>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>

std::string get_file_content(const char*);
unsigned int compile_shader(const char*, GLenum);

class Shader {
public: 
	unsigned int m_ID;

	Shader(const char* vertexPath, const char* fragmentPath);

	void use();
	void del();
	void setBool(const char*, bool);
	void setInt(const char*, int );
	void setFloat(const char*, float);
	void setMat4(const char*, glm::mat4);
	void setVec3(const char*, glm::vec3);
};
#endif
