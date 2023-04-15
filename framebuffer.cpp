#include "framebuffer.h"

FrameBuffer::FrameBuffer(int width, int height, int slot) {
	m_slot = slot;

	glGenFramebuffers(1, &m_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

	Texture texture(NULL, "frame", slot, width, height);
	m_texture = texture.ID();

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

	glGenRenderbuffers(1, &m_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR while attaching the framebuffer" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float quadVertices[] = {
		//positions    texture_UVs
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,

		 1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
	};

	unsigned int  rectVBO;
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &rectVBO);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

FrameBuffer::~FrameBuffer(){
	glDeleteFramebuffers(1, &m_ID);
	glDeleteVertexArrays(1, &m_vao);
	glDeleteRenderbuffers(1, &m_rbo);
	glDeleteTextures(1, &m_texture);
}

void FrameBuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
}

void FrameBuffer::unbind(){
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::draw(Shader shader){
	shader.use();
	glBindVertexArray(m_vao);
	glActiveTexture(GL_TEXTURE0 + m_slot);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}
