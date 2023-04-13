#pragma once

#include<json/json.h>
#include "mesh.h"

using json = nlohmann::json;

class Model {
public:
	Model(const char* file);

	void draw(Shader& shader, Camera& camera);

private:
	const char* m_file;
	std::vector<unsigned char> m_data;
	json m_JSON;

	std::vector<std::string> m_loadedTexName;
	std::vector<Texture> m_loadedTextures;

	std::vector<Mesh> m_meshes;
	std::vector<glm::mat4> matricesMeshes;

	void traverseNode(unsigned int nextNode, glm::mat4 matrix = glm::mat4(1.0f));

	void load_mesh(unsigned int index);
	std::vector<unsigned char> get_data();

	std::vector<float> get_floats(json accessor);
	std::vector<GLuint> get_indices(json accessor);
	std::vector<Texture> get_textures();

	std::vector<glm::vec2> group_vec2(std::vector<float> floatVec);
	std::vector<glm::vec3> group_vec3(std::vector<float> floatVec);
	std::vector<glm::vec4> group_vec4(std::vector<float> floatVec);

	std::vector<Vertex> assemble(
		std::vector<glm::vec3> positions,
		std::vector<glm::vec3> normals,
		std::vector<glm::vec2> texUVs
	);
};
