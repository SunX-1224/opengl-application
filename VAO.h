#ifndef VAO_H
#define VAO_H

#include "VBO.h"

class VAO {
	public:
		GLuint m_ID;

		VAO();

		void linkVBO(VBO& vbo, GLuint layout, GLuint size, GLenum type, GLuint stride, unsigned int offset);
		void bind();
		void unbind();
		void del();
};

#endif
