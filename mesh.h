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

		Mesh(std::vector<Vertex>& verts, std::vector<GLuint>& inds, std::vector<Texture>& textures);
		void clear();
		void draw(Shader& shader,Camera& camera, glm::mat4 matrix = glm::mat4(1.0f));

	private:
		uint32_t m_vbo, m_ebo;
		VAO vao;
};

#endif
