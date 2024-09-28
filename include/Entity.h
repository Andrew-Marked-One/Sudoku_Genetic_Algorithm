#pragma once
#include "EntityMemoryPool.h"


class Entity {
public:
	Entity() noexcept;
	explicit Entity(EntityId_t id) noexcept;

	[[nodiscard]] EntityId_t getId() const noexcept;
	[[nodiscard]] const std::string& getTag() const noexcept;
	[[nodiscard]] bool isAlive() const noexcept;
	void destroy() const noexcept;


	template <typename T, typename... Args>
	T& addComponent(Args&&... args) {
		return EntityMemoryPool::Instance().addComponent<T>(m_id, std::forward<Args>(args)...);
	}


	template <typename T>
	[[nodiscard]] bool hasComponent() const {
		return EntityMemoryPool::Instance().hasComponent<T>(m_id);
	}


	template <typename T>
	[[nodiscard]] const T& getComponent() const {
		return EntityMemoryPool::Instance().getComponent<T>(m_id);
	}


	template <typename T>
	[[nodiscard]] T& getComponent() {
		return EntityMemoryPool::Instance().getComponent<T>(m_id);
	}


	template <typename T>
	void removeComponent() {
		EntityMemoryPool::Instance().removeComponent<T>(m_id);
	}


private:
	EntityId_t m_id = std::numeric_limits<EntityId_t>::max();
};
