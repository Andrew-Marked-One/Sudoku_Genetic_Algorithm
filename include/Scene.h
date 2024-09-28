#pragma once
#include "EntityManager.h"
#include "Action.h"


enum class SceneType : uint8_t {
	None = 0,
	Algorithm
};


class GameEngine;

class Scene {
	using ActionMap_t = std::unordered_map<sf::Keyboard::Key, ActionType>;

public:
	explicit Scene(GameEngine* gameEngine);

	[[nodiscard]] const ActionMap_t& getActionMap() const noexcept;

	virtual void update() = 0;
	virtual void sDoAction(const Action& action) = 0;
	virtual void sRender() = 0;

protected:
	ActionMap_t m_actionMap;
	EntityManager m_entityManager;
	GameEngine* m_game = nullptr;
	int m_currentFrame = 0;
	bool m_isPaused = false;
	bool m_isOnEnd  = false;

	void registerAction(sf::Keyboard::Key inputKey, ActionType actionType);
	void drawEntities() const;
	void drawEntities(const std::string& tag) const;
	void drawEntitiesExcept(const std::string& tag) const;
	void drawEntity(Entity entity) const;
};
