#pragma once

#include "Common.h"
#include <vector>
#include <memory>
#include "../ECS/Component.h"

namespace Crystal {

	namespace details {
		struct LocalTranslationTag {};
		struct WorldTranslationTag {};

		struct TranslationSpecification {
			Math::Vector4 Translation{};
			Math::Quaternion Rotation{};
		};
	}

	using details::LocalTranslationTag;
	using details::WorldTranslationTag;
	using details::TranslationSpecification;

	class Transform final : public AComponent{
	public:
		virtual void Update() noexcept override {}

		void SetRotation(const Math::Quaternion& rotation) noexcept;
		[[nodiscard]] const Math::Quaternion& GetRotation() const noexcept;

		void SetPosition(const Math::Vector4& position) noexcept;
		[[nodiscard]] const Math::Vector4& GetPosition() const noexcept;

		void SetScale(const Math::Vector4& scale) noexcept;
		[[nodiscard]] const Math::Vector4& GetScale() const noexcept;

		void Rotate(const Math::Quaternion& quaternion) noexcept;

		template<class TranslationTag = LocalTranslationTag>
		constexpr void Translate(const Math::Vector4& Translation) noexcept {
			TranslationImpl(TranslationTag(), { Translation, m_rotation });
		}
	private:
		void TranslationImpl(LocalTranslationTag translationTag, TranslationSpecification specification) noexcept;
		void TranslationImpl(WorldTranslationTag translationTag, TranslationSpecification specification) noexcept;

		Math::Vector4 m_scale;
		Math::Quaternion m_rotation;
		Math::Vector4 m_position;

		Transform* m_parent;
		std::vector<std::shared_ptr<Transform>> m_children;
	};
}
