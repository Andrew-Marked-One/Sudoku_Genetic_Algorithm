#include "GameEngine.h"
#include "Scene_Algorithm.h"


GameEngine::GameEngine(const std::string& configPath)
	: m_configPath{ configPath } {

	init();
}

void GameEngine::init() {
	loadFromConfig();
	ImGui::SFML::Init(m_window);
	changeScene("ALGORITHM", std::make_unique<Scene_Algorithm>(this), true);
}

void GameEngine::loadFromConfig() {
	std::ifstream file(m_configPath);
	if (!file.is_open()) {
		std::cerr << "GameEngine::loadFromConfig: Cannot open file with path \"" << m_configPath << "\"\n";
	}

	std::string line;
	while (std::getline(file, line)) {
		std::stringstream iss(line);
		std::string type;
		iss >> type;

		if (type == "Window") {
			int winWidth;
			int winHeight;
			iss >> winWidth >> winHeight >> m_framerateLimit;
			m_window.create(sf::VideoMode(winWidth, winHeight), "Sudoku", sf::Style::Close);
			m_window.setFramerateLimit(m_framerateLimit);
		}
		else if (type == "Assets") {
			std::string assetsPath;
			iss >> assetsPath;
			m_assets.loadAssets(assetsPath);
		}
	}
}

void GameEngine::changeScene(const std::string& sceneName, std::unique_ptr<Scene>&& scene, bool endCurScene) {
	if (scene) {
		m_sceneMap[sceneName] = std::move(scene);
	}
	if (endCurScene) {
		m_sceneMap.erase(m_curSceneName);
	}
	m_curSceneName = sceneName;
}

void GameEngine::run() {
	sf::Clock deltaClock;

	while (isRunning()) {
		try {
			sUserInput();

			currentScene()->update();
			ImGui::SFML::Update(m_window, deltaClock.restart());


			m_window.clear(sf::Color(255, 255, 255));

			currentScene()->sRender();
			ImGui::SFML::Render(m_window);

			m_window.display();
		}
		catch (const std::exception& e) {
			std::cerr << "GameEngine::run, exception caught: " << e.what() << '\n';
			quit();
		}
	}

	ImGui::SFML::Shutdown();
}

bool GameEngine::isRunning() const {
	return m_isRunning && m_window.isOpen();
}

Scene* GameEngine::currentScene() {
	return m_sceneMap[m_curSceneName].get();
}

void GameEngine::quit() {
	m_isRunning = false;
}

void GameEngine::sUserInput() {
	sf::Event event;
	while (m_window.pollEvent(event)) {
		ImGui::SFML::ProcessEvent(event);

		if (event.type == sf::Event::Closed) {
			quit();
		}
		else if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
			if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end()) {
				continue;
			}

			bool isStart = event.type == sf::Event::KeyPressed;
			currentScene()->sDoAction(Action(currentScene()->getActionMap().at(event.key.code), isStart));
		}
		else if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
			sf::Vector2f mPos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(m_window));
			bool isStart = event.type == sf::Event::MouseButtonPressed;

			switch (event.mouseButton.button) {
			case sf::Mouse::Left:
				currentScene()->sDoAction(Action("LEFT_CLICK", isStart, mPos));
				break;
			case sf::Mouse::Right:
				currentScene()->sDoAction(Action("RIGHT_CLICK", isStart, mPos));
				break;
			case sf::Mouse::Middle:
				currentScene()->sDoAction(Action("MIDDLE_CLICK", isStart, mPos));
				break;
			}
		}
		else if (event.type == sf::Event::MouseMoved) {
			sf::Vector2f mPos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(m_window));
			currentScene()->sDoAction(Action("MOUSE_MOVE", true, mPos));
		}
		else if (event.type == sf::Event::Resized) {
			sf::Vector2u newWinSize = { std::max(event.size.width, 120u), std::max(event.size.height, 120u) };
			m_window.setSize(newWinSize);

			sf::FloatRect visibleArea = { 0, 0, static_cast<float>(newWinSize.x), static_cast<float>(newWinSize.y) };
			m_window.setView(sf::View(visibleArea));

			currentScene()->sDoAction(Action("RESIZED", true));
		}
	}
}

sf::RenderWindow& GameEngine::window() {
	return m_window;
}

int GameEngine::getFramerateLimit() const {
	return m_framerateLimit;
}

const Assets& GameEngine::assets() const {
	return m_assets;
}

const std::string& GameEngine::getConfigPath() const {
	return m_configPath;
}
