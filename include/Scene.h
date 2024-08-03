#pragma once
#include "EntityManager.h"
#include "Action.h"


class GameEngine;

class Scene {
public:
	Scene(GameEngine* gameEngine);

	const std::unordered_map<int, std::string>& getActionMap() const;

	virtual void update() = 0;
	virtual void sDoAction(const Action& action) = 0;
	virtual void sRender() = 0;

protected:
	std::unordered_map<int, std::string> m_actionMap;
	EntityManager m_entityManager;
	GameEngine* m_game = nullptr;
	sf::Vector2f m_windowSize;
	sf::Vector2f m_windowHalfSize;
	int m_currentFrame = 0;
	bool m_isPaused = false;

	void registerAction(int inputKey, const std::string& actionName);
	void drawEntities();
	void drawEntities(const std::string& tag);
	void drawEntitiesExcept(const std::string& tag);

private:
	void drawEntitiesHelper(Entity entity);
};
