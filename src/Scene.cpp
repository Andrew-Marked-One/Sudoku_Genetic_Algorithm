#include "Scene.h"
#include "GameEngine.h"


Scene::Scene(GameEngine* gameEngine) {
	m_game = gameEngine;
}

const std::unordered_map<int, std::string>& Scene::getActionMap() const {
	return m_actionMap;
}

void Scene::registerAction(int inputKey, const std::string& actionName) {
	m_actionMap[inputKey] = actionName;
}

void Scene::drawEntities() {
	for (auto&  entity : m_entityManager.getEntities()) {
		drawEntitiesHelper(entity);
	}
}

void Scene::drawEntities(const std::string& tag) {
	for (auto& entity : m_entityManager.getEntities(tag)) {
		drawEntitiesHelper(entity);
	}
}

void Scene::drawEntitiesExcept(const std::string& tag) {
	for (auto& entity : m_entityManager.getEntities()) {
		if (entity.getTag() == tag) {
			continue;
		}

		drawEntitiesHelper(entity);
	}
}

void Scene::drawEntitiesHelper(Entity entity) {
	sf::Vector2f entityPos = entity.getComponent<CTransform>().pos;
	if (entity.hasComponent<CAnimation>()) {
		auto& entitySp = entity.getComponent<CAnimation>().animation.getSprite();
		entitySp.setPosition(entityPos.x, entityPos.y);
		m_game->window().draw(entitySp);
	}
	else if (entity.hasComponent<CShape>()) {
		auto& entitySh = entity.getComponent<CShape>().shape;
		entitySh.setPosition(entityPos.x, entityPos.y);
		m_game->window().draw(entitySh);
	}

	if (entity.hasComponent<CText>()) {
		auto& entityText = entity.getComponent<CText>().text;
		entityText.setPosition(entityPos.x, entityPos.y);
		m_game->window().draw(entityText);
	}
}