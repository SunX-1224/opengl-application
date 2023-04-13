#include "model.h"

Model::Model(const char* file) {
	std::string text = get_file_content(file);
	m_JSON = json::parse(text);

	m_file = file;
	m_data = get_data();

	traverseNode(0);
}

void Model::draw(Shader& shader, Camera& camera){
	glm::mat4 model;
	for (unsigned int i = 0; i < m_meshes.size(); i++) {
		m_meshes[i].draw(shader, camera, matricesMeshes[i]);
	}
}

void Model::traverseNode(unsigned int nextNode, glm::mat4 matrix){
	json node = m_JSON["nodes"][nextNode];

	// Get translation if it exists
	glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
	if (node.find("translation") != node.end())
	{
		float transValues[3];
		for (unsigned int i = 0; i < node["translation"].size(); i++)
			transValues[i] = (node["translation"][i]);
		translation = glm::make_vec3(transValues);
	}
	// Get quaternion if it exists
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	if (node.find("rotation") != node.end())
	{
		float rotValues[4] =
		{
			node["rotation"][3],
			node["rotation"][0],
			node["rotation"][1],
			node["rotation"][2]
		};
		rotation = glm::make_quat(rotValues);
	}
	// Get scale if it exists
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	if (node.find("scale") != node.end())
	{
		float scaleValues[3];
		for (unsigned int i = 0; i < node["scale"].size(); i++)
			scaleValues[i] = (node["scale"][i]);
		scale = glm::make_vec3(scaleValues);
	}
	// Get matrix if it exists
	glm::mat4 matNode = glm::mat4(1.0f);
	if (node.find("matrix") != node.end())
	{
		float matValues[16];
		for (unsigned int i = 0; i < node["matrix"].size(); i++)
			matValues[i] = (node["matrix"][i]);
		matNode = glm::make_mat4(matValues);
	}

	// Initialize matrices
	glm::mat4 trans = glm::mat4(1.0f);
	glm::mat4 rot = glm::mat4(1.0f);
	glm::mat4 sca = glm::mat4(1.0f);

	// Use translation, rotation, and scale to change the initialized matrices
	trans = glm::translate(trans, translation);
	rot = glm::mat4_cast(rotation);
	sca = glm::scale(sca, scale);

	// Multiply all matrices together
	glm::mat4 matNextNode = matrix * matNode * trans * rot * sca;

	// Check if the node contains a mesh and if it does load it
	if (node.find("mesh") != node.end())
	{
		matricesMeshes.push_back(matNextNode);

		load_mesh(node["mesh"]);
		//std::cout << "Loaded a mesh : " << node["name"] << std::endl;
	}

	// Check if the node has children, and if it does, apply this function to them with the matNextNode
	if (node.find("children") != node.end())
	{
		for (unsigned int i = 0; i < node["children"].size(); i++)
			traverseNode(node["children"][i], matNextNode);
	}
}

void Model::load_mesh(unsigned int index){
	unsigned int posAccInd = m_JSON["meshes"][index]["primitives"][0]["attributes"]["POSITION"];
	unsigned int norAccInd = m_JSON["meshes"][index]["primitives"][0]["attributes"]["NORMAL"];
	unsigned int texAccInd = m_JSON["meshes"][index]["primitives"][0]["attributes"]["TEXCOORD_0"];
	unsigned int indAccInd = m_JSON["meshes"][index]["primitives"][0]["indices"];

	std::vector<float> posFL = get_floats(m_JSON["accessors"][posAccInd]);
	std::vector<glm::vec3> positions = group_vec3(posFL);

	std::vector<float> norFL = get_floats(m_JSON["accessors"][norAccInd]);
	std::vector<glm::vec3> normals = group_vec3(norFL);

	std::vector<float> texFL = get_floats(m_JSON["accessors"][texAccInd]);
	std::vector<glm::vec2> texUVs = group_vec2(texFL);

	std::vector<Vertex> vertices = assemble(positions, normals, texUVs);
	std::vector<GLuint> indices = get_indices(m_JSON["accessors"][indAccInd]);
	std::vector<Texture> textures = get_textures();

	m_meshes.push_back(Mesh(vertices, indices, textures));
}

std::vector<unsigned char> Model::get_data() {
	std::string bytes;
	std::string uri = m_JSON["buffers"][0]["uri"];

	std::string file = std::string(m_file);
	std::string directory = file.substr(0, file.find_last_of('/') + 1);
	bytes = get_file_content((directory + uri).c_str());

	std::vector<unsigned char> data(bytes.begin(), bytes.end());
	return data;
}

std::vector<float> Model::get_floats(json accessor)
{
	std::vector<float> floatVec;

	unsigned int bufferViewInd = accessor.value("bufferView", 1);
	unsigned int count = accessor["count"];
	unsigned int accByteOffset = accessor.value("byteOffset", 0);
	std::string type = accessor["type"];

	json bufferView = m_JSON["bufferViews"][bufferViewInd];
	unsigned int byteOffset = bufferView["byteOffset"];

	unsigned int numPerVerts;
	if (type == "SCALAR")
		numPerVerts = 1;
	else if (type == "VEC2")
		numPerVerts = 2;
	else if (type == "VEC3")
		numPerVerts = 3;
	else if (type == "VEC4")
		numPerVerts = 4;
	else
		throw std::invalid_argument("Invalid data type (not scalar, vec2, vec3 or vec4)");

	unsigned int begin = byteOffset + accByteOffset;
	unsigned int length = count * numPerVerts * 4;

	for (unsigned int i = begin; i < begin + length; i) {
		unsigned char bytes[] = { m_data[i++], m_data[i++], m_data[i++], m_data[i++] };
		float value;
		std::memcpy(&value, bytes, sizeof(float));
		floatVec.push_back(value);
	}

	return floatVec;
}

std::vector<GLuint> Model::get_indices(json accessor)
{
	std::vector<GLuint> indices;

	unsigned int bufferViewInd = accessor.value("bufferView", 0);
	unsigned int count = accessor["count"];
	unsigned int accByteOffset = accessor.value("byteOffset", 0);
	unsigned int componentType = accessor["componentType"];

	json bufferView = m_JSON["bufferViews"][bufferViewInd];
	unsigned int byteOffset = bufferView["byteOffset"];

	unsigned int begin = byteOffset + accByteOffset;
	if (componentType == 5125) {
		for (unsigned int i = begin; i < begin + count * 4; i) {
			unsigned char bytes[] = { m_data[i++], m_data[i++], m_data[i++], m_data[i++] };
			unsigned int value;
			std::memcpy(&value, bytes, sizeof(unsigned int));

			indices.push_back((GLuint)value);
		}
	}
	else if (componentType == 5123) {

		for (unsigned int i = begin; i < begin + count * 2; i) {
			unsigned char bytes[] = { m_data[i++], m_data[i++]};
			unsigned short value;
			std::memcpy(&value, bytes, sizeof(unsigned short));

			indices.push_back((GLuint)value);
		}
	}
	else if (componentType == 5121) {

		for (unsigned int i = begin; i < begin + count * 4; i) {
			unsigned char bytes[] = { m_data[i++], m_data[i++]};
			short value;
			std::memcpy(&value, bytes, sizeof(short));

			indices.push_back((GLuint)value);
		}
	}

	return indices;
}

std::vector<Texture> Model::get_textures()
{
	std::vector<Texture> textures;

	std::string file = m_file;
	std::string directory = file.substr(0, file.find_last_of('/') + 1);

	for (unsigned int i = 0; i < m_JSON["images"].size(); i++) {
		std::string texture_path = m_JSON["images"][i]["uri"];

		bool skip = false;
		for (unsigned int j = 0; j < m_loadedTexName.size(); j++) {
			if (m_loadedTexName[j] == texture_path) {
				skip = true;
				textures.push_back(m_loadedTextures[j]);
				break;
			}
		}

		if (skip)
			continue;

		if (texture_path.find("diffuse") != std::string::npos) {
			Texture diffuse((directory + texture_path).c_str(), "diffuse", m_loadedTextures.size());
			textures.push_back(diffuse);
			m_loadedTextures.push_back(diffuse);
			m_loadedTexName.push_back(texture_path);
		}
		else if (texture_path.find("specular") != std::string::npos) {
			Texture specular((directory + texture_path).c_str(), "specular", m_loadedTextures.size());
			textures.push_back(specular);
			m_loadedTextures.push_back(specular);
			m_loadedTexName.push_back(texture_path);
		}
		
	}

	return textures;
}

std::vector<glm::vec2> Model::group_vec2(std::vector<float> floatVec)
{
	std::vector<glm::vec2> vecs;
	for (unsigned int i = 0; i < floatVec.size(); i) {
		vecs.push_back(glm::vec2(floatVec[i++], floatVec[i++]));
	}
	return vecs;
}

std::vector<glm::vec3> Model::group_vec3(std::vector<float> floatVec)
{
	std::vector<glm::vec3> vecs;
	for (unsigned int i = 0; i < floatVec.size(); i) {
		vecs.push_back(glm::vec3(floatVec[i++], floatVec[i++], floatVec[i++]));
	}
	return vecs;
}

std::vector<glm::vec4> Model::group_vec4(std::vector<float> floatVec)
{
	std::vector<glm::vec4> vecs;
	for (unsigned int i = 0; i < floatVec.size(); i) {
		vecs.push_back(glm::vec4(floatVec[i++], floatVec[i++], floatVec[i++], floatVec[i++]));
	}
	return vecs;
}

std::vector<Vertex> Model::assemble(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> texUVs)
{
	std::vector<Vertex> vertices;

	for (unsigned int i = 0; i < positions.size(); i++) {
		vertices.push_back(
			Vertex{
				positions[i],
				normals[i],
				glm::vec3(0.8f),
				texUVs[i]
			}
		);
	}
	return vertices;
}
