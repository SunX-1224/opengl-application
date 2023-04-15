#pragma once

#include<glad/glad.h>
#include<iostream>

#include "shader.h"

class FrameBuffer {
	public:
		FrameBuffer(int width, int height, int slot);

		void bind();
		void unbind();
		void draw(Shader shader);

	private:
		uint32_t m_ID;
		uint32_t m_vao;
		uint32_t m_rbo;
		uint32_t m_slot;
		uint32_t m_texture;
};
