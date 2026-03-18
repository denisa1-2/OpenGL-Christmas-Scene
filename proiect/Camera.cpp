#include "Camera.hpp"

namespace gps {

	void Camera::updateCameraVectors() {
		cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
		cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, worldUp));
		cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
	}

	Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
		this->cameraPosition = cameraPosition;
		this->cameraTarget = cameraTarget;
		this->worldUp = glm::normalize(cameraUp);
		this->cameraUpDirection = cameraUp;

		updateCameraVectors();
	}

	glm::mat4 Camera::getViewMatrix() {
		return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
	}

	void Camera::move(MOVE_DIRECTION direction, float speed) {
		switch (direction) {
		case MOVE_FORWARD:
			cameraPosition += cameraFrontDirection * speed;
			break;
		case MOVE_BACKWARD:
			cameraPosition -= cameraFrontDirection * speed;
			break;
		case MOVE_RIGHT:
			cameraPosition += cameraRightDirection * speed;
			break;
		case MOVE_LEFT:
			cameraPosition -= cameraRightDirection * speed;
			break;
		}
	}

	void Camera::rotate(float pitch, float yaw) {
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		this->cameraFrontDirection = glm::normalize(front);

		this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, worldUp));
		this->cameraUpDirection = glm::normalize(glm::cross(this->cameraRightDirection, this->cameraFrontDirection));
	}

	glm::vec3 Camera::getPosition() {
		return cameraPosition;
	}

	void Camera::setPosition(const glm::vec3& pos) {
		cameraPosition = pos;
	}

	void Camera::lookAt(const glm::vec3& target){
		cameraTarget = target;
		updateCameraVectors();
	}

}