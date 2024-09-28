#pragma once
#include <tuple>
#include <iostream>
#include "Components.h"


using EntityId_t = uint8_t;

inline EntityId_t ENTITY_MEMORY_POOL_MAX_SIZE = 1;

class EntityManager;
class Entity;


class EntityMemoryPool {
	using ComponentsTuple_t = std::tuple<
		std::vector<CTransform>,
		std::vector<CAnimation>,
		std::vector<CShape>,
		std::vector<CText>
	>;

	friend EntityManager;

public:
	[[nodiscard]] static EntityMemoryPool& Instance();

	[[nodiscard]] const std::string& getTag(EntityId_t entityId) const noexcept;
	[[nodiscard]] bool isAlive(EntityId_t entityId) const noexcept;
	void destroy(EntityId_t entityId) noexcept;


	template <typename T>
	[[nodiscard]] const T& getComponent(EntityId_t entityId) const {
		return std::get<std::vector<T>>(m_data)[entityId];
	}


	template <typename T>
	[[nodiscard]] T& getComponent(EntityId_t entityId) {
		return std::get<std::vector<T>>(m_data)[entityId];
	}


	template <typename T>
	[[nodiscard]] bool hasComponent(EntityId_t entityId) const {
		return getComponent<T>(entityId).has;
	}


	template <typename T, typename... Args>
	T& addComponent(EntityId_t entityId, Args&&... args) {
		auto& component = getComponent<T>(entityId);
		component = T(std::forward<Args>(args)...);
		component.has = true;
		return component;
	}


	template <typename T>
	void removeComponent(EntityId_t entityId) {
		getComponent<T>(entityId).has = false;
	}


private:
	ComponentsTuple_t m_data;
	std::vector<std::string> m_tags;
	std::vector<bool> m_alive;
	EntityId_t m_nextEntityId = 0;
	EntityId_t m_poolSize;

	explicit EntityMemoryPool(EntityId_t poolSize);

	[[nodiscard]] Entity addEntity(const std::string& tag);
	[[nodiscard]] EntityId_t getFreeIndexFromBack() const noexcept;
	[[nodiscard]] EntityId_t getFreeIndexFromFront() const noexcept;
	[[nodiscard]] bool isPoolFull() const noexcept;


	template<uint8_t I = 0>
	void initTuple() {
		if constexpr (I < std::tuple_size<ComponentsTuple_t>::value) {
			std::get<I>(m_data).resize(m_poolSize);
			initTuple<I + 1>();
		}
	}


	template<uint8_t I = 0>
	void resetComponents(EntityId_t entityId) {
		if constexpr (I < std::tuple_size<ComponentsTuple_t>::value) {
			std::get<I>(m_data)[entityId] = {};
			resetComponents<I + 1>(entityId);
		}
	}
};
