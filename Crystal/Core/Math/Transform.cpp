#include "Transform.h"
#include "Vector4.cpp"

using namespace Crystal;
using namespace Crystal::Math;

void Transform::SetRotation(const Quaternion& rotation) noexcept {
	m_rotation = rotation;
}

const Quaternion& Transform::GetRotation() const noexcept {
	return m_rotation;
}

void Transform::SetPosition(const Vector4& position) noexcept {
	m_position = position;
}

const Vector4& Transform::GetPosition() const noexcept {
	return m_position;
}

void Transform::SetScale(const Vector4& scale) noexcept {
	m_scale = scale;
}

const Vector4& Transform::GetScale() const noexcept {
	return m_scale;
}

void Transform::Rotate(const Quaternion& quaternion) noexcept {
	m_rotation = quaternion * m_rotation;
}

void Transform::TranslationImpl(LocalTranslationTag translationTag, TranslationSpecification specification) noexcept {
	m_position += Vector4::Rotate(specification.Translation, specification.Rotation);
}

void Transform::TranslationImpl(WorldTranslationTag translationTag, TranslationSpecification specification) noexcept {
	m_position += specification.Translation;
}
