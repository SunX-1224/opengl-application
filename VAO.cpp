#include "VAO.h"

VAO::VAO() {
	glGenVertexArrays(1, &m_ID);
}

void VAO::linkVBO(VBO& vbo, GLuint layout, GLuint size, GLenum type, GLuint stride, unsigned int offset) {
	vbo.bind();
	glVertexAttribPointer(layout, size, type, GL_FALSE, stride, (void*)offset);
	glEnableVertexAttribArray(layout);
	vbo.unbind();
}

void VAO::bind() {
	glBindVertexArray(m_ID);
}

void VAO::unbind() {
	glBindVertexArray(0);
}

void VAO::del() {
	glDeleteVertexArrays(1, &m_ID);
}
