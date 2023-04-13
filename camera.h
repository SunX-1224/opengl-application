#ifndef CAMERA_H
#define CAMERA_H

#include "shader.h"

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<GLFW/glfw3.h>

class Camera{
	public:
		glm::vec3 m_position;
		glm::vec3 m_forward;
		glm::vec3 m_up;
		glm::vec3 m_right;
		float m_aspect;

		Camera(glm::vec3 position, glm::vec3 direction, uint16_t width, uint16_t height);

		void inputHandler(GLFWwindow* window, float deltaTime);
		void processMouse(double xPos, double yPos);
		void processScroll(double offset);
		void setMatrices(Shader& shader);
		glm::mat4 getView();
		glm::mat4 getPerspective();

	private:
		float m_speed = 1.5f;
		float m_sensitivity = 0.3f;
		float m_fov = 45.0f;
		bool first_mouse = true;
		float mouse_x, mouse_y;
		float m_yaw, m_pitch;
};

#endif
