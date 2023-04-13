#ifndef EBO_H
#define EBO_H

#include<glad/glad.h>
#include<vector>

class EBO{
	public:
		unsigned int m_ID;

		EBO(std::vector<GLuint>& indices);
		~EBO();
		
		void bind();
		void unbind();
		void del();
};

#endif