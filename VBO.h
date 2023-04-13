#ifndef VBO_H
#define VBO_H

#include <glm/glm.hpp>
#include<glad/glad.h>

#include<vector>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normals;
	glm::vec3 color;
	glm::vec2 texUV;
};

class VBO {
	public:
		unsigned int m_ID;

		VBO(std::vector<Vertex>& vertices);
		VBO();

		void assignData(GLfloat* vertices, GLsizeiptr size);
		void bind();
		void unbind();
		void del();
};

#endif
