#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Transform.h"

//camera object which stores the necessary 
//data to construct the view projection matrix
class Camera
{
public:
	//this camera supports both 2D and 3D and this enum is used to specify which projection is used
	enum class ProjectionType
	{
		Perspective = 0, Orthographic = 1
	};

	Camera() { }
	~Camera() { }

	//retieves the transform object of this camera object
	const glm::mat4& GetTransform() const { return m_transform; }
	glm::mat4& GetTransform() { return m_transform; }

	//sets the orthographic/perspective data for the camera, note that this will also change 
	//the camera to that type of projection
	void SetOrthographic(float size, float nearClip, float farClip);
	void SetPerspective(float fov, float nearClip, float farClip);

	//sets the viewport size of this camera and recalculates the projection matrix
	void SetViewportSize(unsigned int width, unsigned int height);

	//getters and setters for the different settings of the camera when using 
	//perspective projection (vertical fov, near and far clip)
	float GetPerspectiveVerticalFOV() const { return m_perspectiveFOV; }
	void SetPerspectiveVerticalFOV(float fov) { m_perspectiveFOV = fov; RecalculateProjection(); }
	float GetPerspectiveNearClip() const { return m_perspectiveNear; }
	void SetPerspectiveNearClip(float nearClip) { m_perspectiveNear = nearClip; RecalculateProjection(); }
	float GetPerspectiveFarClip() const { return m_perspectiveFar; }
	void SetPerspectiveFarClip(float farClip) { m_perspectiveFar = farClip; RecalculateProjection(); }


	//getters and setters for the different settings of the camera when using 
	//orthographic projection (size, near and far clip)
	float GetOrthographicSize() const { return m_orthographicSize; }
	void SetOrthographicSize(float size) { m_orthographicSize = size; RecalculateProjection(); }
	float GetOrthographicNearClip() const { return m_orthographicNear; }
	void SetOrthographicNearClip(float nearClip) { m_orthographicNear = nearClip; RecalculateProjection(); }
	float GetOrthographicFarClip() const { return m_orthographicFar; }
	void SetOrthographicFarClip(float farClip) { m_orthographicFar = farClip; RecalculateProjection(); }

	//getters and setters for the projection type
	ProjectionType GetProjectionType() const { return m_projectionType; }
	void SetProjectionType(ProjectionType type) { m_projectionType = type; RecalculateProjection(); }

	//retrives the projection matrix for the given settings of the camera
	const glm::mat4& GetProjectionMatrix() const { return m_projectionMatrix; }

private:
	//helper function which recalculates the projection matrix 
	//according to which projection type is being used
	void RecalculateProjection();

	glm::mat4 m_projectionMatrix = glm::mat4(1.0f);
	glm::mat4 m_transform = glm::mat4(1.0f);

	//while the camera supports both perspective and orthographic 
	//projection it defaults to projection as required by the assignment
	ProjectionType m_projectionType = ProjectionType::Perspective; 

	float m_orthographicSize = 10.0f;
	float m_orthographicNear = -1.0f, m_orthographicFar = 1.0f;

	float m_perspectiveFOV = glm::radians(45.0f);
	float m_perspectiveNear = 0.01f, m_perspectiveFar = 1000.0f;

	float m_aspectRatio = 0.0f;
};

#endif
