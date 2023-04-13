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

		Texture(const char* path, const char* type, unsigned int slot);

		void texUnit(Shader& shader, const char* uniform);
		void bind();
		void unbind();
};

#endif
