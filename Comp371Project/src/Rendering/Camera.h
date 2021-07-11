#pragma once
#include "Transform.h"

class Camera
{
public:
	enum class ProjectionType
	{
		Perspective = 0, Orthographic = 1
	};

	Camera() { }
	~Camera() { }

	void SetPosition(const glm::vec3& pos)
	{
		m_transform.position = pos;
	}

	void SetRotation(const glm::vec3& rot)
	{
		m_transform.rotation = rot;
	}

	const Transform& GetTransform() const { return m_transform; }
	Transform& GetTransform() { return m_transform; }

	//sets the orthographic/perspective data for the camera, note that this will also change 
	//the camera to that type of projection
	void SetOrthographic(float size, float nearClip, float farClip);
	void SetPerspective(float fov, float nearClip, float farClip);

	void SetViewportSize(unsigned int width, unsigned int height);

	float GetPerspectiveVerticalFOV() const { return m_perspectiveFOV; }
	void SetPerspectiveVerticalFOV(float fov) { m_perspectiveFOV = fov; RecalculateProjection(); }
	float GetPerspectiveNearClip() const { return m_perspectiveNear; }
	void SetPerspectiveNearClip(float nearClip) { m_perspectiveNear = nearClip; RecalculateProjection(); }
	float GetPerspectiveFarClip() const { return m_perspectiveFar; }
	void SetPerspectiveFarClip(float farClip) { m_perspectiveFar = farClip; RecalculateProjection(); }


	float GetOrthographicSize() const { return m_orthographicSize; }
	void SetOrthographicSize(float size) { m_orthographicSize = size; RecalculateProjection(); }
	float GetOrthographicNearClip() const { return m_orthographicNear; }
	void SetOrthographicNearClip(float nearClip) { m_orthographicNear = nearClip; RecalculateProjection(); }
	float GetOrthographicFarClip() const { return m_orthographicFar; }
	void SetOrthographicFarClip(float farClip) { m_orthographicFar = farClip; RecalculateProjection(); }

	ProjectionType GetProjectionType() const { return m_projectionType; }
	void SetProjectionType(ProjectionType type) { m_projectionType = type; RecalculateProjection(); }

	const glm::mat4& GetProjectionMatrix() const { return m_projectionMatrix; }

	void UpdateAspectRatio();

private:
	void RecalculateProjection();

	glm::mat4 m_projectionMatrix = glm::mat4(1.0f);
	Transform m_transform;

	//default to 3D
	ProjectionType m_projectionType = ProjectionType::Perspective; 

	float m_orthographicSize = 10.0f;
	float m_orthographicNear = -1.0f, m_orthographicFar = 1.0f;

	float m_perspectiveFOV = glm::radians(45.0f);
	float m_perspectiveNear = 0.01f, m_perspectiveFar = 1000.0f;

	float m_aspectRatio = 0.0f;
};

