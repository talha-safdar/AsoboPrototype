#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera() : m_position(0.0f, 0.0f, 3.0f), m_forward(0.0f, 0.0f, -1.0f), m_up(0.0f, 1.0f, 0.0f)
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
