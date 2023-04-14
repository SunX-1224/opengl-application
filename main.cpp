#include "model.h"

#define WIDTH 1200
#define HEIGHT 800

void framebuffer_size_callback(GLFWwindow*, int, int);
void mouseCallback(GLFWwindow*, double, double);
void scrollCallback(GLFWwindow*, double, double);

Camera camera(glm::vec3(0.0f, 4.5f, 4.0f), glm::vec3(0.0f, 0.0f, -1.0f), WIDTH, HEIGHT);

float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions   // texCoords
	1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,

	1.0f,  1.0f,  1.0f, 1.0f,
	1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,
};

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "openglTest", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	gladLoadGL();
	glfwSwapInterval(0);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	
	glDepthFunc(GL_LESS);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // blend color buffer with texture color

	glViewport(0, 0, WIDTH, HEIGHT);


	//Some local variables
	float u_time = glfwGetTime();
	float delta_time = 0.0f;
	
	glm::vec3 lightColor = glm::vec3(1.0f);
	glm::vec3 lightPosition;

	//Shaders
	Shader shader("vertexShader.glsl", "fragmentShader.glsl");
	Shader frame_shader("frame_vs.glsl", "frame_fs.glsl");

	frame_shader.use();
	frame_shader.setInt("frame", 0);

	//Frame buffer quad
	unsigned int rectVAO, rectVBO;
	glGenVertexArrays(1, &rectVAO);
	glGenBuffers(1, &rectVBO);
	glBindVertexArray(rectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	//Framebuffer
	uint32_t fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	uint32_t frame_texture;
	glGenTextures(1, &frame_texture);
	glBindTexture(GL_TEXTURE_2D, frame_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_texture, 0);

	uint32_t rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR while attaching the framebuffer" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	Model trees("models/trees/scene.gltf");
	Model ground("models/ground/scene.gltf");
	
	shader.use();
	//shader.setFloat("light.linear", 0.012f);
	//shader.setFloat("light.quadratic", 0.0044f);

	shader.setVec3("light.diffuse", glm::vec3(0.4f));
	shader.setVec3("light.ambient", glm::vec3(0.2f));
	shader.setVec3("light.specular", glm::vec3(0.6f));
	shader.setVec3("light.direction", glm::vec3(0.1f, -0.1f, -0.8f));


	while (!glfwWindowShouldClose(window)) {
		delta_time = glfwGetTime() - u_time;
		u_time = static_cast<float> (glfwGetTime());
		if(u_time - int(u_time) > 0.99f){
			std::string title = "opengl : dt_" + std::to_string(delta_time * 1000) + " fps_" + std::to_string(1.0f / delta_time);
			glfwSetWindowTitle(window, title.c_str());
		}
		camera.inputHandler(window, delta_time);

		//lightPosition = glm::vec3(2.0f*sin(u_time), 2.0f, -1.0f + 2.0f*cos(u_time));

		shader.use();
		shader.setFloat("u_time", u_time);
		//shader.setVec3("light.position", lightPosition);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lightPosition);
		
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		ground.draw(shader, camera);
		trees.draw(shader, camera);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
	
		frame_shader.use();
		glBindVertexArray(rectVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, frame_texture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glDeleteFramebuffers(1, &fbo);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
	camera.m_aspect = (float) width / height;
}

void mouseCallback(GLFWwindow* window, double xPos, double yPos) {
    camera.processMouse(static_cast<float>(xPos), static_cast<float>(yPos));
}

void scrollCallback(GLFWwindow* window, double xOff, double yOff) {
    camera.processScroll(static_cast<float>(yOff));
}
