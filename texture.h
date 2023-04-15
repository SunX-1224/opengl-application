#ifndef TEXTURE_H
#define TEXTURE_H

#include<glad/glad.h>
#include "shader.h"
#include<stb/stb_image.h>

class Texture {
	public:
		GLuint m_ID;
		const char* m_type;
		unsigned int m_slot;

		Texture(const char* path, const char* type, unsigned int slot, int _width = 800, int _height = 800);

		void texUnit(Shader& shader, const char* uniform);
		void bind();
		void unbind();
		void set_property(GLenum target, GLenum pname, GLenum parameter);
		uint32_t ID();
};

#endif
