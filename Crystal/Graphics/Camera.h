#pragma once
#include "../Core/Math/Transform.h"
#include <type_traits>

namespace Crystal {
	struct Frustum {
		float Near{ 0.1f };
		float Far{ 1000.0f };
	};

	class Camera {
	public:
		Camera(float fov, float aspectRatio, Frustum viewFrustum) noexcept;
		void SetProjection(float fov, float aspectRatio, Frustum viewFrustum) noexcept;

		void SetLookAt(const Math::Vector4& eye, const Math::Vector4& target, const Math::Vector4& up) noexcept;
		[[nodiscard]] Math::Matrix GetView() const noexcept;

		[[nodiscard]] Math::Matrix GetProjection() const noexcept;

		void SetFov(float vFov) noexcept;
		[[nodiscard]] const float GetFov() const noexcept;
	private:
		void UpdateViewMatrix() const noexcept;

		Transform m_transform;
		Frustum m_viewFrustum;
		float m_fov{ 45 };
		float m_aspectRatio{ 1.0f };

		mutable Math::Matrix m_view;
		mutable Math::Matrix m_projection;
	};
}