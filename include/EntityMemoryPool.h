#pragma once

#include <tuple>
#include <iostream>
#include "Components.h"


inline size_t ENTITY_MEMORY_POOL_MAX_SIZE = 1344;

class EntityManager;
class Entity;


class EntityMemoryPool {
	using ComponentTuple = std::tuple<
		std::vector<CTransform>,
		std::vector<CAnimation>,
		std::vector<CShape>,
		std::vector<CText>,
		std::vector<CCell>
	>;

	friend EntityManager;

public:
	static EntityMemoryPool& Instance();
	const std::string& getTag(size_t entityId) const;
	bool isAlive(size_t entityId) const;
	void destroy(size_t entityId);


	template <typename T>
	const T& getComponent(size_t entityId) const {
		return std::get<std::vector<T>>(m_data)[entityId];
	}

	template <typename T>
	T& getComponent(size_t entityId) {
		return std::get<std::vector<T>>(m_data)[entityId];
	}

	template <typename T>
	bool hasComponent(size_t entityId) const {
		return getComponent<T>(entityId).has;
	}

	template <typename T, typename... Args>
	T& addComponent(size_t entityId, Args&&... args) {
		auto& component = getComponent<T>(entityId);
		component = T(std::forward<Args>(args)...);
		component.has = true;
		return component;
	}

	template <typename T>
	void removeComponent(size_t entityId) {
		getComponent<T>(entityId).has = false;
	}

private:
	ComponentTuple m_data;
	std::vector<std::string> m_tags;
	std::vector<bool> m_alive;
	size_t m_nextEntityId = 0;
	size_t m_poolSize;

	EntityMemoryPool(size_t poolSize);

	Entity addEntity(const std::string& tag);
	size_t findFreeIndex() const;
	void resetNextEntityId();
	bool canPushBack() const;

	template<size_t I = 0>
	void initTuple() {
		if constexpr (I < std::tuple_size<ComponentTuple>::value) {
			std::get<I>(m_data).resize(m_poolSize);
			initTuple<I + 1>();
		}
	}

	template<size_t I = 0>
	void resetComponents(size_t entityId) {
		if constexpr (I < std::tuple_size<ComponentTuple>::value) {
			std::get<I>(m_data)[entityId] = {};
			resetComponents<I + 1>(entityId);
		}
	}
};
