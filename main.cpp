/*
	TODO:
		- Instancing
*/
#include "model.h"
#include "framebuffer.h"
#include <math.h>

#define WIDTH 1200
#define HEIGHT 900

void scrollCallback(GLFWwindow*, double, double);
void framebuffer_size_callback(GLFWwindow*, int, int);
void mouseCallback(GLFWwindow*, double, double);


Camera camera(glm::vec3(0.0f, 4.5f, 4.0f), glm::vec3(0.0f, 0.0f, -1.0f), WIDTH, HEIGHT);

float cubeVertices[] =
{
	//   Coordinates
	-1.0f, -1.0f,  1.0f,//        7--------6
	 1.0f, -1.0f,  1.0f,//       /|       /|
	 1.0f, -1.0f, -1.0f,//      4--------5 |
	-1.0f, -1.0f, -1.0f,//      | |      | |
	-1.0f,  1.0f,  1.0f,//      | 3------|-2
	 1.0f,  1.0f,  1.0f,//      |/       |/
	 1.0f,  1.0f, -1.0f,//      0--------1
	-1.0f,  1.0f, -1.0f
};

unsigned int cubeIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

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
	glEnable(GL_MULTISAMPLE);
	
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
	glm::vec3 lightPosition = glm::vec3(0.0f, 10.0f, 0.0f);

	//skybox
	VAO cubeVAO;
	uint32_t vbo, ebo;

	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	cubeVAO.bind();
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), &cubeIndices, GL_STATIC_DRAW);

	cubeVAO.linkVBO(0, 3, GL_FLOAT, 3 * sizeof(float), 0);

	cubeVAO.unbind();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	uint32_t cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	stbi_set_flip_vertically_on_load(false);

	for (int i = 0; i < 6; i++) {
		int width, height, nrChannels;
		std::string filename = "models/space/corona (" + std::to_string(i+1) + ").png";
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, NULL);

		if (data) {
			GLenum format;
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 3)
				format = GL_RGB;
			else if (nrChannels == 4)
				format = GL_RGBA;
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		}
		else {
			std::cout << "ERROR while loading the image : " + filename << std::endl;
		}
		stbi_image_free(data);
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	//Framebuffer
	FrameBuffer framebuffer(WIDTH, HEIGHT, 0);

	//Shaders
	Shader shader("vertexShader.glsl", "fragmentShader.glsl");
	Shader instanced_shader("instanced_vs.glsl", "fragmentShader.glsl");
	Shader frame_shader("frame_vs.glsl", "frame_fs.glsl");
	Shader cubemap_shader("cubemap_vs.glsl", "cubemap_fs.glsl");
	Shader light_shader("light_vs.glsl", "light_fs.glsl");

	frame_shader.use();
	frame_shader.setInt("frame", 0);

	cubemap_shader.use();
	cubemap_shader.setInt("skybox", 0);

	// Instanced drawing
	const int number = 5;
	const float radius = 50.0f;
	const glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);

	std::vector<glm::mat4> instanceMat;

	for (int i = 0; i < number; i++) {
		float x = -1.0f + rand() * 2.0f / RAND_MAX;
		float z = -1.0f + rand() * 2.0f / RAND_MAX;

		glm::vec3 position = center + glm::vec3(x * radius, 0.0f, z * radius);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		instanceMat.push_back(model);
	}


	Model trees("models/trees/scene.gltf" ,number, instanceMat);
	Model ground("models/ground/scene.gltf");
	
	instanced_shader.use();
	instanced_shader.setFloat("light.linear", 0.012f);
	instanced_shader.setFloat("light.quadratic", 0.00044f);

	instanced_shader.setVec3("light.diffuse", glm::vec3(3.0f));
	instanced_shader.setVec3("light.ambient", glm::vec3(0.4f));
	instanced_shader.setVec3("light.specular", glm::vec3(0.2f));

	shader.use();
	shader.setFloat("light.linear", 0.012f);
	shader.setFloat("light.quadratic", 0.00044f);

	shader.setVec3("light.diffuse", glm::vec3(3.0f));
	shader.setVec3("light.ambient", glm::vec3(0.4f));
	shader.setVec3("light.specular", glm::vec3(0.2f));
	//shader.setVec3("light.direction", glm::vec3(0.1f, -0.1f, -0.8f));

	
	while (!glfwWindowShouldClose(window)) {
		delta_time = glfwGetTime() - u_time;
		u_time = static_cast<float> (glfwGetTime());
		if(u_time - int(u_time) > 0.99f){
			std::string title = "opengl : dt_" + std::to_string(delta_time * 1000) + " fps_" + std::to_string(1.0f / delta_time);
			glfwSetWindowTitle(window, title.c_str());
		}
		camera.inputHandler(window, delta_time);

		lightPosition = glm::vec3(20.0f*sin(u_time), 20.0f, 20.0f*cos(u_time));

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.4f));
		model = glm::translate(model, lightPosition);
		
		framebuffer.bind();
		glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		
		shader.use();
		shader.setFloat("u_time", u_time);
		shader.setVec3("light.position", lightPosition);
		ground.draw(shader, camera);

		instanced_shader.use();
		instanced_shader.setFloat("u_time", u_time);
		instanced_shader.setVec3("light.position", lightPosition);
		trees.draw(instanced_shader, camera);
		
		light_shader.use();
		cubeVAO.bind();
		light_shader.setMat4("model", model);
		camera.setMatrices(light_shader);
		glDrawElements(GL_TRIANGLES, sizeof(cubeIndices) / sizeof(cubeIndices[0]), GL_UNSIGNED_INT, 0);
		cubeVAO.unbind();
		

		glDepthFunc(GL_LEQUAL);

		cubemap_shader.use();
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		// We make the mat4 into a mat3 and then a mat4 again in order to get rid of the last row and column
		// The last row and column affect the translation of the skybox (which we don't want to affect)
		view = glm::mat4(glm::mat3(glm::lookAt(camera.m_position, camera.m_position + camera.m_forward, camera.m_up)));
		projection = glm::perspective(glm::radians(45.0f), camera.m_aspect, 0.1f, 100.0f);

		cubemap_shader.setMat4("view", view);
		cubemap_shader.setMat4("projection", projection);

		cubeVAO.bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

		glDrawElements(GL_TRIANGLES, sizeof(cubeIndices) / sizeof(cubeIndices[0]), GL_UNSIGNED_INT, 0);

		cubeVAO.unbind();
		glDepthFunc(GL_LESS);

		framebuffer.unbind();
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
	
		framebuffer.draw(frame_shader);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void scrollCallback(GLFWwindow* window, double xOff, double yOff) {
    camera.processScroll(static_cast<float>(yOff));
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
	camera.updateAspect((float)width / height);
}

void mouseCallback(GLFWwindow* window, double xPos, double yPos) {
    camera.processMouse(xPos, yPos);
}
