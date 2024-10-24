#include "GameEngine.h"
#include "Scene_Algorithm.h"


GameEngine::GameEngine(const std::string& configPath)
	: m_configPath{ configPath } {

	INPUT_VALIDITY(!configPath.empty());

	init();
}

GameEngine::~GameEngine() {
	ImPlot::DestroyContext();
	ImGui::SFML::Shutdown();
}

void GameEngine::init() {
	loadFromConfig();
	if (!ImGui::SFML::Init(m_window)) {
		std::cerr << "GameEngine::init: Cannot init ImGui\n";
	}
	ImPlot::CreateContext();
	changeScene(SceneType::Algorithm, std::make_unique<Scene_Algorithm>(this), true);
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
			int winWidth = 0;
			int winHeight = 0;
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

void GameEngine::changeScene(SceneType sceneType, std::unique_ptr<Scene> scene, bool endCurScene) noexcept {
	INPUT_VALIDITY(sceneType != SceneType::None);
	INPUT_VALIDITY(scene != nullptr);
	INPUT_VALIDITY(sceneType != m_currentSceneType || !endCurScene);

	m_sceneMap[sceneType] = std::move(scene);

	if (endCurScene) {
		m_sceneMap.erase(m_currentSceneType);
	}
	m_currentSceneType = sceneType;
}

void GameEngine::run() {
	sf::Clock deltaClock;

	while (isRunning()) {
		try {
			sUserInput();

			currentScene()->update();
			ImGui::SFML::Update(m_window, deltaClock.restart());

			m_window.clear(sf::Color::White);

			currentScene()->sRender();
			ImGui::SFML::Render(m_window);

			m_window.display();
		}
		catch (const std::exception& e) {
			std::cerr << "GameEngine::run: exception caught: " << e.what() << '\n';
			quit();
		}
	}
}

bool GameEngine::isRunning() const noexcept {
	return m_isRunning && m_window.isOpen();
}

Scene* GameEngine::currentScene() noexcept {
	return m_sceneMap[m_currentSceneType].get();
}

void GameEngine::quit() noexcept {
	m_isRunning = false;
}

void GameEngine::sUserInput() {
	sf::Event event;
	while (m_window.pollEvent(event)) {
		ImGui::SFML::ProcessEvent(m_window, event);
		const auto& actionMap = currentScene()->getActionMap();
		ActionStage actionStage = ActionStage::Start;
		sf::Vector2f mPos;

		switch (event.type) {
		case sf::Event::Closed:
			quit();
			break;
		case sf::Event::KeyPressed:
		case sf::Event::KeyReleased:
			if (actionMap.find(event.key.code) == actionMap.end()) {
				continue;
			}
			actionStage = event.type == sf::Event::KeyPressed ? ActionStage::Start : ActionStage::End;
			currentScene()->sDoAction(Action(actionMap.at(event.key.code), actionStage));
			break;
		case sf::Event::MouseButtonPressed:
		case sf::Event::MouseButtonReleased:
			actionStage = event.type == sf::Event::MouseButtonPressed ? ActionStage::Start : ActionStage::End;
			mPos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(m_window));

			switch (event.mouseButton.button) {
			case sf::Mouse::Left:
				currentScene()->sDoAction(Action(ActionType::LeftClick, actionStage, mPos));
				break;
			case sf::Mouse::Right:
				currentScene()->sDoAction(Action(ActionType::RightClick, actionStage, mPos));
				break;
			case sf::Mouse::Middle:
				currentScene()->sDoAction(Action(ActionType::MiddleClick, actionStage, mPos));
				break;
			}

			break;
		case sf::Event::MouseMoved:
			mPos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(m_window));
			currentScene()->sDoAction(Action(ActionType::MouseMove, actionStage, mPos));
			break;
		case sf::Event::Resized:
			sf::Vector2u newWinSize = { std::max(event.size.width, m_minWindowSize.x), std::max(event.size.height, m_minWindowSize.y) };
			m_window.setSize(newWinSize);

			sf::FloatRect visibleArea = { 0, 0, static_cast<float>(newWinSize.x), static_cast<float>(newWinSize.y) };
			m_window.setView(sf::View(visibleArea));

			currentScene()->sDoAction(Action(ActionType::Resized, actionStage));
			break;
		}
	}
}

const sf::RenderWindow& GameEngine::getWindow() const noexcept {
	return m_window;
}

sf::RenderWindow& GameEngine::getWindow() noexcept {
	return m_window;
}

int GameEngine::getFramerateLimit() const noexcept {
	return m_framerateLimit;
}

const Assets& GameEngine::assets() const noexcept {
	return m_assets;
}

const std::string& GameEngine::getConfigPath() const noexcept {
	return m_configPath;
}
