#include "mesh.h"


Mesh::Mesh(std::vector<Vertex>& verts, std::vector<GLuint>& inds, std::vector<Texture>& texs, uint32_t number, std::vector<glm::mat4> instanceMat){
	m_vertices = verts;
	m_indices = inds;
	m_textures = texs;
	m_count = number;

	vao.bind();

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW);
	
	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(GLuint), inds.data(), GL_STATIC_DRAW);

	vao.linkVBO(0, 3, GL_FLOAT, sizeof(Vertex), 0);
	vao.linkVBO(1, 3, GL_FLOAT, sizeof(Vertex), 3 * sizeof(float));
	vao.linkVBO(2, 3, GL_FLOAT, sizeof(Vertex), 6 * sizeof(float));
	vao.linkVBO(3, 2, GL_FLOAT, sizeof(Vertex), 9 * sizeof(float));

	if(m_count != 1){
		glGenBuffers(1, &m_ivbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_ivbo);
		glBufferData(GL_ARRAY_BUFFER, instanceMat.size() * sizeof(instanceMat[0]), instanceMat.data(), GL_STATIC_DRAW);
		//glBindBuffer(GL_ARRAY_BUFFER, m_ivbo);

		vao.linkVBO(4, 4, GL_FLOAT, sizeof(glm::mat4), 0);
		vao.linkVBO(5, 4, GL_FLOAT, sizeof(glm::mat4), sizeof(glm::vec4));
		vao.linkVBO(6, 4, GL_FLOAT, sizeof(glm::mat4), 2 * sizeof(glm::vec4));
		vao.linkVBO(7, 4, GL_FLOAT, sizeof(glm::mat4), 3 * sizeof(glm::vec4));

		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
	}


	vao.unbind();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::clear(){
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ebo);
	vao.del();
}

void Mesh::draw(Shader& shader,Camera& camera, glm::mat4 matrix)
{
	shader.use();
	vao.bind();

	unsigned int diffuse = 0, specular = 0;

	for (int i = 0; i < m_textures.size(); i++) {
		std::string num;
		std::string type = m_textures[i].m_type;
		if (type == "diffuse")
			num = std::to_string(diffuse++);
		else if (type == "specular")
			num = std::to_string(specular++);

		m_textures[i].texUnit(shader, (type + num).c_str());
		m_textures[i].bind();
	}
	
	shader.setVec3("camera_position", camera.m_position);
	camera.setMatrices(shader);

	

	
	if (m_count == 1){
		shader.setMat4("model", matrix);
		glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
	}
	else
		glDrawElementsInstanced(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0, m_count);
}
