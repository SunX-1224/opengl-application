#include "EBO.h"

EBO::EBO(std::vector<GLuint>& indices) {
	glGenBuffers(1, &m_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}


EBO::~EBO() {
	glDeleteBuffers(1, &m_ID);
}

void EBO::bind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
}

void EBO::unbind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::del() {
	glDeleteBuffers(1, &m_ID);
}
