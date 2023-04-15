#include "model.h"
#include "framebuffer.h"

#define WIDTH 1200
#define HEIGHT 800

void framebuffer_size_callback(GLFWwindow*, int, int);
void mouseCallback(GLFWwindow*, double, double);
void scrollCallback(GLFWwindow*, double, double);

Camera camera(glm::vec3(0.0f, 4.5f, 4.0f), glm::vec3(0.0f, 0.0f, -1.0f), WIDTH, HEIGHT);

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
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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

	//Framebuffer
	FrameBuffer framebuffer(WIDTH, HEIGHT, 0);

	//Shaders
	Shader shader("vertexShader.glsl", "fragmentShader.glsl");
	Shader frame_shader("frame_vs.glsl", "frame_fs.glsl");

	frame_shader.use();
	frame_shader.setInt("frame", 0);

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
		
		framebuffer.bind();
		glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		ground.draw(shader, camera);
		trees.draw(shader, camera);

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
