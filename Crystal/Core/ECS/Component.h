#pragma once

namespace Crystal {
	class Entity;
	class AComponent {
	public:
		virtual void Update() noexcept = 0;

		void Enable()  noexcept { m_enabled = true; }
		void Disable() noexcept { m_enabled = false; }
		[[nodiscard]] constexpr bool IsEnabled() const noexcept { return m_enabled; }

		[[nodiscard]] constexpr Entity* GetEntity() const noexcept { return m_entity; }
		void SetEntity(Entity* const entity)        noexcept { m_entity = entity; }
	protected:
		Entity* m_entity{ nullptr };
		bool m_enabled{ false };
	};
}