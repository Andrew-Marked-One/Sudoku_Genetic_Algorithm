#pragma once
#include "Scene.h"
#include "Assets.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include "implot.h"


class GameEngine {
public:
	explicit GameEngine(const std::string& configPath);
	~GameEngine();

	void changeScene(SceneType sceneType, std::unique_ptr<Scene> scene, bool endCurScene) noexcept;
	void run();
	[[nodiscard]] bool isRunning() const noexcept;
	void quit() noexcept;
	[[nodiscard]] const sf::RenderWindow& getWindow() const noexcept;
	[[nodiscard]] sf::RenderWindow& getWindow() noexcept;
	[[nodiscard]] int getFramerateLimit() const noexcept;
	[[nodiscard]] const Assets& assets() const noexcept;
	[[nodiscard]] const std::string& getConfigPath() const noexcept;

private:
	sf::RenderWindow m_window;
	Assets m_assets;
	std::unordered_map<SceneType, std::unique_ptr<Scene>> m_sceneMap;
	std::string m_configPath = "NONE";
	sf::Vector2u m_minWindowSize = { 120u, 120u };
	int m_framerateLimit = 60;
	SceneType m_currentSceneType = SceneType::None;
	bool m_isRunning = true;
	 
	void init();
	void loadFromConfig();
	[[nodiscard]] Scene* currentScene() noexcept;
	void sUserInput();
};
