#include "mesh.h"


Mesh::Mesh(std::vector<Vertex>& verts, std::vector<GLuint>& inds, std::vector<Texture>& texs){
	m_vertices = verts;
	m_indices = inds;
	m_textures = texs;

	vao.bind();
	VBO vbo(verts);
	EBO ebo(inds);

	vao.linkVBO(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), 0);
	vao.linkVBO(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), 3 * sizeof(float));
	vao.linkVBO(vbo, 2, 3, GL_FLOAT, sizeof(Vertex), 6 * sizeof(float));
	vao.linkVBO(vbo, 3, 2, GL_FLOAT, sizeof(Vertex), 9 * sizeof(float)); 

	vao.unbind();
	vbo.unbind();
	ebo.unbind();
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

	shader.setMat4("model", matrix);

	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
}
