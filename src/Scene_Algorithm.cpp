#include "Scene_Algorithm.h"
#include "GameEngine.h"


#pragma region Initialization
Scene_Algorithm::Scene_Algorithm(GameEngine* gameEngine)
	: Scene{ gameEngine } {

	init();
}

void Scene_Algorithm::init() {
	registerAction(sf::Keyboard::Escape, "QUIT");

	resetGridAndPopulation();
}

void Scene_Algorithm::resetGridAndPopulation() {
	for (auto& entity : m_entityManager.getEntities("cell")) {
		entity.destroy();
	}

	m_entityManager.update();

	m_grid.clear();
	m_population.clear();

	m_grid.resize(m_gridSize, std::vector<Entity>(m_gridSize));
	m_grid.shrink_to_fit();
	resizePopulation();

	initGrid();
	initPopulation(0);

	m_entityManager.update();
}

void Scene_Algorithm::resizeGrid(float relativeDif) {
	for (auto& entity : m_entityManager.getEntities("cell")) {
		entity.getComponent<CTransform>().pos *= relativeDif;

		auto& entitySh = entity.getComponent<CShape>().shape;
		entitySh.setSize(entitySh.getSize() * relativeDif);
		entitySh.setOrigin(entitySh.getSize() / 2.f);

		auto& entityText = entity.getComponent<CText>();
		entityText.text.setScale(entityText.text.getScale() * relativeDif);
		entityText.containerSize *= relativeDif;
	}
}

void Scene_Algorithm::resizePopulation() {
	m_population.resize(m_populationSize, std::pair<int, std::vector<char>>(0, std::vector<char>(m_gridSize * m_gridSize)));
	m_population.shrink_to_fit();
}

void Scene_Algorithm::initGrid() {
	float cellSize = (m_game->window().getSize().y * (3.f / 4.f)) / m_gridSize;
	float cellHalfSize = cellSize / 2.f;

	for (char i = 0; i < m_gridSize; i++) {
		for (char j = 0; j < m_gridSize; j++) {
			m_grid[i][j] = createCell(sf::Vector2f(j * cellSize + cellHalfSize, i * cellSize + cellHalfSize), sf::Vector2f(cellSize, cellSize), 0);
		}
	}
}

Entity Scene_Algorithm::createCell(sf::Vector2f pos, sf::Vector2f size, char val) {
	Entity cell = m_entityManager.addEntity("cell");
	cell.addComponent<CTransform>(pos);
	cell.addComponent<CShape>(size, sf::Color(255, 255, 255), sf::Color(0, 0, 0), -1);
	cell.addComponent<CCell>(val);
	cell.addComponent<CText>("0", size * 0.5f, m_game->assets().getFont("Roboto"), sf::Color(0, 0, 0), sf::Color(0, 0, 0), 0);

	return cell;
}

void Scene_Algorithm::initPopulation(int startIndex) {
	for (int i = startIndex; i < m_populationSize; i++) {
		auto& charVec = m_population[i].second;
		for (auto& ch : charVec) {
			ch = getRandCellVal();
		}
	}
}

char Scene_Algorithm::getRandCellVal() const {
	static std::random_device rd;
	static std::mt19937 rng(rd());
	std::uniform_int_distribution<std::mt19937::result_type> range(0, m_gridSize - 1);

	return range(rng);
}

void Scene_Algorithm::sortElitePopulation() {
	int eliteAmount = std::max(static_cast<int>(std::round(m_populationSize * m_eliteSurvivalRate / 100.f)), 1);
	std::partial_sort(m_population.begin(), m_population.begin() + eliteAmount, m_population.end(), [](const auto& pair1, const auto& pair2) {
		return pair1.first > pair2.first;
	});
}

void Scene_Algorithm::updateCells() {
	std::vector<std::vector<char>> rowVec(m_gridSize, std::vector<char>(m_gridSize, 0));
	std::vector<std::vector<char>> colVec(m_gridSize, std::vector<char>(m_gridSize, 0));

	for (char i = 0; i < m_gridSize; i++) {
		for (char j = 0; j < m_gridSize; j++) {
			const auto& entityCCell = m_grid[i][j].getComponent<CCell>();
			rowVec[i][entityCCell.val]++;
			colVec[j][entityCCell.val]++;
		}
	}

	int colorUnit = 255 / (m_gridSize - 1);

	for (char i = 0; i < m_gridSize; i++) {
		for (char j = 0; j < m_gridSize; j++) {
			Entity& entity = m_grid[i][j];
			auto& entityCCell = entity.getComponent<CCell>();
			char collisions = std::min(rowVec[i][entityCCell.val] + colVec[j][entityCCell.val] - 2, m_gridSize - 1);

			char gbCellColor = std::max(255 - colorUnit * collisions, 0);
			entity.getComponent<CShape>().shape.setFillColor(sf::Color(255, gbCellColor, gbCellColor));

			auto& entityCText = entity.getComponent<CText>();
			entityCText.text.setString(std::to_string(entityCCell.val + 1));
			entityCText.centerText();
		}
	}
}

void Scene_Algorithm::gui() {
	sf::Vector2f controlsPos = { 0, m_game->window().getSize().y * (3.f / 4.f) };
	ImGui::SetNextWindowPos(controlsPos);
	if (ImGui::Begin("Genetic algorithm controls", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
		static char gridSizes[] = { 1, 4, 9, 16, 25, 36 };
		static int gridSizeIndex = std::sqrt(m_gridSize);

		static const char* comboItems[] = { "Full screen", "3840x2160", "2560x1440" , "1920x1080" , "1600x900", "1280x720" , "1024x576" };
		static unsigned char windowSizes[] = {255, 240, 160, 120, 100, 80, 64};
		static int currentItemIndex = 6;

		ImGui::SliderInt("Grid size", &gridSizeIndex, 1, 6, nullptr, ImGuiSliderFlags_AlwaysClamp);
		if (ImGui::IsItemDeactivatedAfterEdit()) {
			m_gridSize = gridSizes[gridSizeIndex - 1];
			resetGridAndPopulation();
		}

		ImGui::SliderInt("Population size", &m_populationSize, 1, 1000, nullptr, ImGuiSliderFlags_AlwaysClamp);
		if (ImGui::IsItemDeactivatedAfterEdit()) {
			int prevPopulationSize = m_population.size();
			sortElitePopulation();
			resizePopulation();
			initPopulation(prevPopulationSize - 1);
		}

		ImGui::SliderInt("Mutation rate, %", &m_mutationRate, 1, 100, nullptr, ImGuiSliderFlags_AlwaysClamp);
		ImGui::SliderInt("Random genes rate, %", &m_randomGenesRate, 1, 100, nullptr, ImGuiSliderFlags_AlwaysClamp);
		ImGui::SliderInt("Elite survival rate, %", &m_eliteSurvivalRate, 1, 100, nullptr, ImGuiSliderFlags_AlwaysClamp);
		if (ImGui::Combo("Window Size", &currentItemIndex, comboItems, 7, 7)) {
			float prevWindowHeight = static_cast<float>(m_game->window().getSize().y);
			if (currentItemIndex == 0) {
				m_game->window().create(sf::VideoMode::getDesktopMode(), "Sudoku", sf::Style::Fullscreen);
			}
			else {
				m_game->window().create(sf::VideoMode(16 * windowSizes[currentItemIndex], 9 * windowSizes[currentItemIndex]), "Sudoku", sf::Style::Close);
			}
			float newWindowHeight = static_cast<float>(m_game->window().getSize().y);
			resizeGrid(newWindowHeight / prevWindowHeight);
		}
		
	}
	//ImGui::ShowDemoWindow();
	ImGui::End();
}

void Scene_Algorithm::update() {
	m_currentFrame++;
}

void Scene_Algorithm::onEnd() {
	m_game->quit();
}

void Scene_Algorithm::pause() {
	m_isPaused = !m_isPaused;
}

void Scene_Algorithm::sDoAction(const Action& action) {
	if (action.isStart()) {
		if (action.name() == "QUIT") {
			onEnd();
		}
		//else if (action.name() == "MOUSE_MOVE") {}
		//else if (action.name() == "LEFT_CLICK") {}
		//else if (action.name() == "RIGHT_CLICK") {}
		//else if (action.name() == "MIDDLE_CLICK") {}
		//else if (action.name() == "RESIZED") {}
	}
	//else if (action.type() == "END") {
		//if (action.name() == "LEFT_CLICK") {}
		//else if (action.name() == "RIGHT_CLICK") {}
		//else if (action.name() == "MIDDLE_CLICK") {}
	//}
}

void Scene_Algorithm::sRender() {
	drawEntities();
	gui();
}
