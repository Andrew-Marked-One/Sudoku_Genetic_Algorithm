#include "EntityManager.h"


void EntityManager::update() {
	addEntities();
	removeEntities();
}

Entity EntityManager::addEntity(const std::string& tag) {
	Entity entity = EntityMemoryPool::Instance().addEntity(tag);
	m_entitiesToAdd.emplace_back(entity);
	return entity;
}

void EntityManager::addEntities() {
	for (const auto& entity : m_entitiesToAdd) {
		m_entities.emplace_back(entity);
		m_entityMap[entity.getTag()].emplace_back(entity);
	}
	m_entitiesToAdd.clear();
}

void EntityManager::removeEntities() {
	auto needErase = [this](const auto& entity) {
		if (!entity.isAlive()) {
			EntityMemoryPool::Instance().resetComponents(entity.getId());
			removeFromEntityMap(entity);
			return true;
		}

		return false;
	};

	auto eraseVec = std::ranges::remove_if(m_entities, needErase);

	if (!eraseVec.empty()) {
		EntityMemoryPool::Instance().m_nextEntityId = EntityMemoryPool::Instance().getFreeIndexFromBack();
	}

	m_entities.erase(eraseVec.begin(), eraseVec.end());
}

void EntityManager::removeFromEntityMap(Entity entity) {
	auto& vec = m_entityMap[entity.getTag()];
	const auto entityIt = std::ranges::find_if(vec, [entity](Entity target) {
		return target.getId() == entity.getId();
	});

	if (entityIt != vec.end()) {
		vec.erase(entityIt);
	}
}

const std::vector<Entity>& EntityManager::getEntities() const noexcept {
	return m_entities;
}

std::vector<Entity>& EntityManager::getEntities() noexcept {
	return m_entities;
}

const std::vector<Entity>& EntityManager::getEntities(const std::string& tag) const {
	const auto entityIt = m_entityMap.find(tag);

	INPUT_VALIDITY(entityIt != m_entityMap.end());

	return entityIt->second;
}

std::vector<Entity>& EntityManager::getEntities(const std::string& tag) {
	const auto entityIt = m_entityMap.find(tag);

	INPUT_VALIDITY(entityIt != m_entityMap.end());

	return entityIt->second;
}
