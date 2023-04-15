#include "texture.h"

Texture::Texture(const char* path, const char* type, unsigned int slot, int _width, int _height) {
	int width, height, nrChannels;
	unsigned char* data;
	GLenum format = GL_RED;

	m_slot = slot;
	m_type = type;

	if (path == NULL) {
		data = NULL;
		nrChannels = 3;
		width = _width;
		height = _height;
	}
	else {
		stbi_set_flip_vertically_on_load(true);
		data = stbi_load(path, &width, &height, &nrChannels, 0);
		if (not data)
			std::cout << "ERROR while loading the image on path : " << path << std::endl;
	}

	if (nrChannels == 1)
		format = GL_RED;
	else if (nrChannels == 3)
		format = GL_RGB;
	else if (nrChannels == 4)
		format = GL_RGBA;

	glGenTextures(1, &m_ID);
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	if (data)
		stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::texUnit(Shader& shader, const char* uniform) {
	shader.use();
	shader.setInt(uniform, m_slot);
}

void Texture::bind() {
	glActiveTexture(GL_TEXTURE0 + m_slot);
	glBindTexture(GL_TEXTURE_2D, m_ID);
}

void Texture::unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::set_property(GLenum target, GLenum pname, GLenum parameter) {
	glTexParameteri(target, pname, parameter);
}

uint32_t Texture::ID(){
	return m_ID;
}
