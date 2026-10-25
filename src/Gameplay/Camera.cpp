#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <algorithm>

Camera::Camera() : m_position(0.0f, 0.0f, 3.0f), m_forward(0.0f, 0.0f, -1.0f), m_up(0.0f, 1.0f, 0.0f), m_right(1.0f, 0.0f, 0.0f), m_yaw(-90.0f), m_pitch(0.0f)
{

}

glm::mat4 Camera::GetViewMatrix() const
{
    /*
        if
        position = (0,0,3)
        forward  = (0,0,-1)
        then
        target = (0,0,2)
    */


    return glm::lookAt(m_position, m_position + m_forward, m_up);
}

void Camera::MoveForward(float amount)
{
	m_position += m_forward * amount; // positive=forward, negative=backward
}

void Camera::MoveRight(float amount)
{
	m_position += m_right * amount; // positive=right, negative=left
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset)
{
	// horizontal= yaw (turn head left/right), vertical= pitch (look up/down)

    const float sensitivity = 0.1f;

    m_yaw += xOffset * sensitivity;
    m_pitch -= yOffset * sensitivity;

    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

    UpdateVectors();
}

void Camera::UpdateVectors()
{
    /*
        Mouse
         ↓
        yaw + pitch
         ↓
        calculate forward vector
         ↓
        calculate right vector
         ↓
        calculate up vector
    */

    glm::vec3 forward;

    forward.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    forward.y = sin(glm::radians(m_pitch));

    forward.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    m_forward = glm::normalize(forward);

    m_right = glm::normalize(glm::cross(m_forward, glm::vec3(0.0f, 1.0f, 0.0f))); // cross product: calculate a perpendicular direction

    m_up = glm::normalize(glm::cross(m_right, m_forward)); // cross product: calculate a perpendicular direction
}