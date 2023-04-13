#include "VBO.h"

VBO::VBO(std::vector<Vertex> &vertices) {
	glGenBuffers(1, &m_ID);
	glBindBuffer(GL_ARRAY_BUFFER, m_ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}

VBO::VBO() {
	glGenBuffers(1, &m_ID);
}

void VBO::assignData(GLfloat* vertices, GLsizeiptr size) {
	glBindBuffer(GL_ARRAY_BUFFER, m_ID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void VBO::bind() {
	glBindBuffer(GL_ARRAY_BUFFER, m_ID);
}

void VBO::unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::del() {
	glDeleteBuffers(1, &m_ID);
}
