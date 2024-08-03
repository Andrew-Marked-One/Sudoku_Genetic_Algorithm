#pragma once
#include "EntityMemoryPool.h"


class Entity {
public:
	Entity();
	Entity(size_t id);

	size_t getId() const;
	const std::string& getTag() const;
	bool isAlive() const;
	void destroy() const;

	template <typename T, typename... Args>
	T& addComponent(Args&&... args) const {
		return EntityMemoryPool::Instance().addComponent<T>(m_id, std::forward<Args>(args)...);
	}

	template <typename T>
	bool hasComponent() const {
		return EntityMemoryPool::Instance().hasComponent<T>(m_id);
	}

	template <typename T>
	T& getComponent() const {
		return EntityMemoryPool::Instance().getComponent<T>(m_id);
	}

	template <typename T>
	void removeComponent() const {
		EntityMemoryPool::Instance().removeComponent<T>(m_id);
	}

private:
	size_t m_id = std::numeric_limits<size_t>::max();
};
