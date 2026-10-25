#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <algorithm>

Camera::Camera() : 
    m_position(0.0f, 2.0f, 5.0f),
    m_forward(0.0f, 0.0f, -1.0f),
    m_right(1.0f, 0.0f, 0.0f),
    m_yaw(-90.0f),
    m_pitch(-20.0f), // slightly downward
    m_distance(5.0f) // 5 world units away
{
    UpdateVectors();
}

glm::mat4 Camera::GetViewMatrix(const glm::vec3& target) const
{
    /*
        if
        position = (0,0,3)
        forward  = (0,0,-1)
        then
        target = (0,0,2)

        Where am I?
        → m_position

        What am I looking at?
        → player

        Which direction is up?
        → world Y
    */

    glm::vec3 targetPosition = target + glm::vec3(0.0f, 0.5f, 0.0f);
    return glm::lookAt(m_position, targetPosition, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 Camera::GetForward() const
{
    /*
        Camera forward:
        (x, y, z)

        Player movement:
        (x, 0, z)
    */

    glm::vec3 forward(m_forward.x, 0.0f, m_forward.z); // y=0 prevents sending character underground

    return glm::normalize(forward);
}

glm::vec3 Camera::GetRight() const
{
    return m_right;
}

const glm::vec3& Camera::GetPosition() const
{
    return m_position;
}

void Camera::SetPosition(const glm::vec3& position)
{
    m_position = position;
}

void Camera::Update(const glm::vec3& target)
{
    glm::vec3 targetPosition = target + glm::vec3(0.0f, 0.5f, 0.0f); // look at upper body

    // Take the player position and move backwards along the camera's viewing direction by 5 units
    m_position = targetPosition - (m_forward * m_distance);
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset)
{
	// horizontal= yaw (turn head left/right), vertical= pitch (look up/down)

    const float sensitivity = 0.1f;

    m_yaw += xOffset * sensitivity;
    m_pitch += yOffset * sensitivity;

    m_pitch = std::clamp(m_pitch, -80.0f, 60.0f);  // orbit the camera around the player

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