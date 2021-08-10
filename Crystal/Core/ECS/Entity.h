#pragma once
#include "../Logging/Logger.h"
#include <memory>
#include <algorithm>
#include <execution>

#include"Component.h"

namespace Crystal {
	class Entity {
	public:
		template<std::derived_from<AComponent> T>
		constexpr void AddComponent(auto&&... args) noexcept {
			if (HasComponent<T>()) {
				return;
			}

			auto component = std::make_unique<T>(std::forward<decltype(args)>(args)...);

			component->SetEntity(this);
			component->Enable();

			m_components.emplace_back(std::move(component));
		}

		template<std::derived_from<AComponent> T>
		constexpr void RemoveComponent() noexcept {
			if (const auto index = HasComponent<T>()) {
				m_components.at(index.value())->SetEntity(nullptr);
				m_components.erase(std::next(m_components.begin(), index.value()));
			}
		}

		constexpr void RemoveAllComponents() noexcept { m_components.clear(); }

		template<std::derived_from<AComponent> T>
		[[nodiscard]]
		constexpr T* GetComponent()  const noexcept {
			if (const auto index = HasComponent<T>()) {
				return static_cast<T*>(m_components.at(index.value()).get());
			}
			return nullptr;
		}

		[[nodiscard]] const auto& GetComponents() const noexcept { return m_components; }

		template<std::derived_from<AComponent> T>
		[[nodiscard]]
		constexpr std::optional<uint32_t> HasComponent() const noexcept {
			namespace rn = std::ranges;

			const auto typeId = typeid(decltype(std::declval<T>())).hash_code();
			const auto lambda = [typeId](const auto& component) -> bool { return typeid(*component).hash_code() == typeId; };

			if (const auto iter = rn::find_if(m_components, lambda); iter != m_components.cend()) {
				return std::distance(m_components.begin(), iter);
			}
			return {};
		}

		[[nodiscard]] const std::string& GetName() const noexcept { return m_name; }
		void SetName(std::string_view name)        noexcept { m_name = name; }
	private:
		std::vector<std::unique_ptr<AComponent>> m_components;
		std::string m_name{};
	};
}