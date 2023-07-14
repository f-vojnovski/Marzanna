#include "perspective_camera.h"

namespace mz {
	PerspectiveCamera::PerspectiveCamera(float fovy, float aspectRatio, float nearClip, float farClip)
	{
		m_fov = fovy;
		m_aspectRatio = aspectRatio;
		m_nearClip = nearClip;
		m_farClip = farClip;

		m_position = glm::vec3(2.0f, 2.0f, 2.0f);
		m_front = glm::vec3(-2.0f, -2.0f, -2.0f);
		m_up = glm::vec3(0.0f, 0.0f, 1.0f);

		UpdateViewMatrix();
		UpdateProjectionMatrix();
	}
	
	void PerspectiveCamera::SetPosition(const glm::vec3& position)
	{
		m_position = position;
		UpdateViewMatrix();
	}
	
	void PerspectiveCamera::SetDirection(const glm::vec3& direction)
	{
		m_front = glm::normalize(direction);
		UpdateViewMatrix();
	}
	
	void PerspectiveCamera::SetAspectRatio(float aspectRatio)
	{
		m_aspectRatio = aspectRatio;
		UpdateProjectionMatrix();
	}
	
	void PerspectiveCamera::SetFOV(float fov)
	{
		m_fov = fov;
		UpdateProjectionMatrix();
	}
	
	void PerspectiveCamera::UpdateViewMatrix()
	{
		m_viewMatrix = glm::lookAt(m_position, m_position + m_front, m_up);
	}
	
	void PerspectiveCamera::UpdateProjectionMatrix()
	{
		m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearClip, m_farClip);
		m_projectionMatrix[1][1] *= -1;
	}
}