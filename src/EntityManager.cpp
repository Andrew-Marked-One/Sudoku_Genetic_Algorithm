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
	auto needErase = [this](Entity entity) {
		if (!entity.isAlive()) {
			EntityMemoryPool::Instance().resetComponents(entity.getId());
			removeFromEntityMap(entity);
			return true;
		}
		else {
			return false;
		}
	};

	auto eraseVec = std::ranges::remove_if(m_entities, needErase);

	if (!eraseVec.empty()) {
		EntityMemoryPool::Instance().resetNextEntityId();
	}

	m_entities.erase(eraseVec.begin(), eraseVec.end());
}

void EntityManager::removeFromEntityMap(Entity entity) {
	auto& vec = m_entityMap[entity.getTag()];
	auto it = std::ranges::find_if(vec, [entity](Entity target) {
		return target.getId() == entity.getId();
	});

	if (it != vec.end()) {
		vec.erase(it);
	}
}

const std::vector<Entity>& EntityManager::getEntities() const {
	return m_entities;
}

std::vector<Entity>& EntityManager::getEntities() {
	return m_entities;
}

const std::vector<Entity>& EntityManager::getEntities(const std::string& tag) const {
	return m_entityMap.at(tag);
}

std::vector<Entity>& EntityManager::getEntities(const std::string& tag) {
	return m_entityMap[tag];
}
