#ifndef VAO_H
#define VAO_H

#include <glad/glad.h>
#include<glm/glm.hpp>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normals;
	glm::vec3 color;
	glm::vec2 texUV;
};

class VAO {
	public:
		GLuint m_ID;

		VAO();

		void linkVBO(GLuint layout, GLuint size, GLenum type, GLuint stride, unsigned int offset);
		void bind();
		void unbind();
		void del();
};

#endif
