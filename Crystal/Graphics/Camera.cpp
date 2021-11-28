#include "Camera.h"
#include "../Core/Math/Vector3.cpp"

using namespace Crystal;
using namespace Crystal::Math;

Camera::Camera(float fov, float aspectRatio, Frustum viewFrustum) noexcept 
	:
	m_fov(fov),
	m_aspectRatio(aspectRatio),
	m_viewFrustum(viewFrustum)
{}

void Camera::SetProjection(float fov, float aspectRatio, Frustum viewFrustum) noexcept {
	m_fov         = fov;
	m_aspectRatio = aspectRatio;
	m_viewFrustum = viewFrustum;
}

void Camera::SetLookAt(const Vector4& eye, const Vector4& target, const Vector4& up) noexcept {
	m_view = Matrix::CreateLookAtLH(eye, target, up);

	m_transform.SetPosition(eye);
	m_transform.SetRotation(Matrix::Transpose(m_view).GetRotation());
}

Matrix Camera::GetView() const noexcept {
	return m_view;
}

Matrix Camera::GetProjection() const noexcept {
	return m_projection;
}

void Camera::SetFov(float vFov) noexcept {
	if (m_fov != vFov) {
		m_fov = vFov;
	}
}

const float Camera::GetFov() const noexcept {
	return m_fov;
}

void Camera::UpdateViewMatrix() const noexcept {
	const auto& translation = Matrix::CreateTranslation(Vector4::Negate(m_transform.GetPosition()));
	const auto& rotation    = Matrix::Transpose(Matrix::CreateRotation(m_transform.GetRotation()));

	m_view = translation * rotation;
}
