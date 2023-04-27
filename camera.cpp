#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 direction, uint16_t width, uint16_t height) {
	m_position = position;
	m_forward = glm::normalize(direction);
	m_up = glm::vec3(0.0f, 1.0f, 0.0f);
	m_right = glm::cross(m_forward, m_up);
	m_up = glm::cross(m_right, m_forward);

	m_yaw = -90.0f;
	m_pitch = 0.0f;

	mouse_x = width / 2;
	mouse_y = height / 2;

	this->updateAspect(static_cast<float>(width / height));
}

void Camera::inputHandler(GLFWwindow* window, float deltaTime) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		m_position += m_speed * m_forward * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		m_position -= m_speed * m_forward * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		m_position -= m_speed * glm::normalize(glm::cross(m_forward, m_up)) * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		m_position += m_speed * glm::normalize(glm::cross(m_forward, m_up)) * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		m_position += m_speed * m_up * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		m_position -= m_speed * m_up * deltaTime;
}

void Camera::setMatrices(Shader& shader) {
	shader.setMat4("view", getView());
	shader.setMat4("projection", getPerspective());
}

void Camera::updateAspect(float aspect){
	m_aspect = aspect;
	m_perspective = glm::perspective(glm::radians(m_fov), m_aspect, 0.1f, 100.0f);
}

glm::mat4 Camera::getView() {
	return glm::lookAt(m_position, m_position + m_forward, m_up);
}

glm::mat4 Camera::getPerspective() {
	return m_perspective;
}

void Camera::processMouse(double xPos, double yPos) {
	if (this->first_mouse)
		mouse_x = xPos, mouse_y = yPos, first_mouse = false;

	m_yaw += (xPos - mouse_x) * m_sensitivity;
	m_pitch -= (yPos - mouse_y) * m_sensitivity;
	mouse_x = xPos, mouse_y = yPos;

	m_pitch = m_pitch > 85.0f ? 85.0f : m_pitch < -85.0f ? -85.0f : m_pitch;

	m_forward.x += cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_forward.y += sin(glm::radians(m_pitch));
	m_forward.z += sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

	m_forward = glm::normalize(m_forward);
}

void Camera::processScroll(double offset) {
	m_fov -= offset;
	m_fov = m_fov > 65.0f ? 65.0f : m_fov < 3.0f ? 3.0f : m_fov;
}
