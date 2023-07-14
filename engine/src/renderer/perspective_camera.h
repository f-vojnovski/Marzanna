#pragma once

#include "engine/src/mzpch.h"

namespace mz {
	class PerspectiveCamera {
	public:
        PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip);
        void SetPosition(const glm::vec3& position);
        void SetDirection(const glm::vec3& direction);
        void SetAspectRatio(float aspectRatio);
        void SetFOV(float fov);

        inline const glm::mat4& GetViewMatrix() const { return m_viewMatrix; }
        inline const glm::mat4& GetProjectionMatrix() const { return m_projectionMatrix; }
	private:
        void UpdateViewMatrix();
        void UpdateProjectionMatrix();

        glm::vec3 m_position;
        glm::vec3 m_front;
        glm::vec3 m_up;

        float m_fov;
        float m_aspectRatio;
        float m_nearClip;
        float m_farClip;

        glm::mat4 m_viewMatrix;
        glm::mat4 m_projectionMatrix;
	};
}