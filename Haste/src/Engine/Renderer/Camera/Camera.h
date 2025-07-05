#pragma once

#include "Core/Timestep.h"
#include "Core/Events/Event.h"
#include "Core/Events/MouseEvent.h"

#include <glm/glm.hpp>

namespace Core {

	enum CameraViewMode : uint8_t
	{
		Perspective,
		Orthographic,
	};

	class Camera
	{
	public:
		Camera() = default;
		Camera(float fov, float aspectRatio, float nearClip, float farClip);

		void OnUpdate(Timestep ts);

		inline float GetDistance() const { return m_Distance; }
		inline void SetDistance(float distance) { m_Distance = distance; }

		inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }

		const glm::mat4& GetView() const { return m_ViewMatrix; }
		const glm::mat4& GetProjection() const { return m_Projection; }
		const glm::mat4& GetViewProjection() const { return m_Projection * m_ViewMatrix; }

		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		const glm::vec3& GetPosition() const { return m_Position; }
		glm::quat GetOrientation() const;

		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }

		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

		void ResetView();

		void SetViewMode(CameraViewMode mode);
		CameraViewMode GetViewMode() const;

	private:
		void UpdateProjection();
		void UpdateView();

		glm::vec3 CalculatePosition() const;

		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;

	private:
		float m_FOV = 65.0f, m_AspectRatio = 1.778f, m_NearClip = 0.01f, m_FarClip = 1000.0f;

		CameraViewMode m_ViewMode = CameraViewMode::Perspective;

		glm::mat4 m_Projection;
		glm::mat4 m_ViewMatrix;
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

		float m_Distance = 5.0f;
		float m_Pitch = 0.0f, m_Yaw = 0.0f;

		float m_ViewportWidth = 1280, m_ViewportHeight = 720;
	};

}
