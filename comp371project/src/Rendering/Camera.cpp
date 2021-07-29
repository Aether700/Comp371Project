#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "../Core/Application.h"

void Camera::SetOrthographic(float size, float nearClip, float farClip)
{
	m_projectionType = ProjectionType::Orthographic;

	m_orthographicSize = size;
	m_orthographicNear = nearClip;
	m_orthographicFar = farClip;

	RecalculateProjection();
}

void Camera::SetPerspective(float fov, float nearClip, float farClip)
{
	m_projectionType = ProjectionType::Perspective;

	m_perspectiveFOV = fov;
	m_perspectiveNear = nearClip;
	m_perspectiveFar = farClip;

	RecalculateProjection();
}

//sets the viewport size of this camera and recalculates the projection matrix
void Camera::SetViewportSize(unsigned int width, unsigned int height)
{
	m_aspectRatio = (float)width / (float)height;
	RecalculateProjection();
}

//helper function which recalculates the projection matrix 
//according to which projection type is being used
void Camera::RecalculateProjection()
{
	if (m_projectionType == ProjectionType::Perspective)
	{
		m_projectionMatrix = glm::perspective(m_perspectiveFOV, m_aspectRatio, m_perspectiveNear, m_perspectiveFar);
	}
	else //Orthographic
	{
		float orthoLeft = -m_orthographicSize * m_aspectRatio * 0.5f;
		float orthoRight = m_orthographicSize * m_aspectRatio * 0.5f;
		float orthoBottom = -m_orthographicSize * 0.5f;
		float orthoTop = m_orthographicSize * 0.5f;

		m_projectionMatrix = glm::ortho(orthoLeft, orthoRight,
			orthoBottom, orthoTop, m_orthographicNear, m_orthographicFar);
	}
}
