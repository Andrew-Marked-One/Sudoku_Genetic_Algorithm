#pragma once
#include <unordered_map>
#include <ranges>
#include "Entity.h"


class EntityManager {
public:
	void update();
	Entity addEntity(const std::string& tag);
	[[nodiscard]] const std::vector<Entity>& getEntities() const noexcept;
	[[nodiscard]] std::vector<Entity>& getEntities() noexcept;
	[[nodiscard]] const std::vector<Entity>& getEntities(const std::string& tag) const;
	[[nodiscard]] std::vector<Entity>& getEntities(const std::string& tag);

private:
	std::vector<Entity> m_entities;
	std::vector<Entity> m_entitiesToAdd;
	std::unordered_map<std::string, std::vector<Entity>> m_entityMap;

	void addEntities();
	void removeEntities();
	void removeFromEntityMap(Entity entity);
};
