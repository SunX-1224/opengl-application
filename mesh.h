#ifndef MESH_H
#define MESH_H

#include<string>
#include<vector>

#include "VAO.h"
#include "camera.h"
#include "texture.h"

class Mesh {
	public:
		std::vector<Vertex> m_vertices;
		std::vector<GLuint> m_indices;
		std::vector<Texture> m_textures;
		std::vector<glm::mat4> m_instanceMat;
		uint32_t m_count;

		Mesh(
			std::vector<Vertex>& verts,
			std::vector<GLuint>& inds,
			std::vector<Texture>& textures,
			uint32_t number = 1,
			std::vector<glm::mat4> instanceMat = {}
		);
		void clear();
		void draw(Shader& shader,Camera& camera, glm::mat4 matrix = glm::mat4(1.0f));

	private:
		uint32_t m_vbo, m_ebo, m_ivbo;
		VAO vao;
};

#endif
