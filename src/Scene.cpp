#include "Scene.h"
#include "GameEngine.h"


Scene::Scene(GameEngine* gameEngine)
	: m_game{ gameEngine } {

	INPUT_VALIDITY(gameEngine);
}

const Scene::ActionMap_t& Scene::getActionMap() const noexcept {
	return m_actionMap;
}

void Scene::registerAction(sf::Keyboard::Key inputKey, ActionType actionType) {
	INPUT_VALIDITY(inputKey >= 0);

	m_actionMap[inputKey] = actionType;
}

void Scene::drawEntities() const {
	for (auto&  entity : m_entityManager.getEntities()) {
		drawEntity(entity);
	}
}

void Scene::drawEntities(const std::string& tag) const {
	for (auto& entity : m_entityManager.getEntities(tag)) {
		drawEntity(entity);
	}
}

void Scene::drawEntitiesExcept(const std::string& tag) const {
	for (auto& entity : m_entityManager.getEntities()) {
		if (entity.getTag() == tag) {
			continue;
		}
		drawEntity(entity);
	}
}

void Scene::drawEntity(Entity entity) const {
	if (entity.hasComponent<CAnimation>()) {
		auto& entitySp = entity.getComponent<CAnimation>().animation.getSprite();
		m_game->getWindow().draw(entitySp);
	}
	else if (entity.hasComponent<CShape>()) {
		auto& entitySh = entity.getComponent<CShape>().shape;
		m_game->getWindow().draw(entitySh);
	}

	if (entity.hasComponent<CText>()) {
		auto& entityText = entity.getComponent<CText>().text;
		m_game->getWindow().draw(entityText);
	}
}