#include "Scene_Algorithm.h"
#include "GameEngine.h"


Scene_Algorithm::Scene_Algorithm(GameEngine* gameEngine)
	: Scene{ gameEngine } {

	INPUT_VALIDITY(gameEngine != nullptr);
	INPUT_VALIDITY(m_fitnessFunctions.size() == m_colorFunctions.size() && m_fitnessFunctions.size() == m_colorWeights.size());
	for (const auto& weight : m_colorWeights) {
		INPUT_VALIDITY(weight > 0);
	}
	INPUT_VALIDITY(m_relativeGridSize > 0);
	INPUT_VALIDITY(m_curFitnessFuncIndex >= 0 && m_curFitnessFuncIndex < m_fitnessFunctions.size());
	INPUT_VALIDITY(m_generationsPerUpdate > 0);
	INPUT_VALIDITY(m_updateLimit > 0);
	INPUT_VALIDITY(m_curUpdate == 0);
	INPUT_VALIDITY(m_populationSize > 0);
	INPUT_VALIDITY(m_mutationRate >= 0 && m_mutationRate <= 100);
	INPUT_VALIDITY(m_randomGenesRate >= 0 && m_randomGenesRate <= 100);
	INPUT_VALIDITY(m_eliteSurvivalRate >= 0 && m_eliteSurvivalRate <= 100);
	INPUT_VALIDITY(m_gridSize > 0);
	int boxSize = static_cast<int>(std::sqrt(m_gridSize));
	INPUT_VALIDITY(m_gridSize == boxSize * boxSize);
	INPUT_VALIDITY(!m_isPopulationUpdating);
	INPUT_VALIDITY(!m_shouldDiscardFuturePopulation);

	init();

	OUTPUT_VALIDITY(m_population.size() == m_populationSize);
	OUTPUT_VALIDITY(m_population[0].gene.size() == m_gridSize * m_gridSize);
	OUTPUT_VALIDITY(m_population[0].gene.size() == m_colorGene.size());
	OUTPUT_VALIDITY(m_maxFitnessValues.size() == m_minFitnessValues.size() && m_maxFitnessValues.size() == m_avgFitnessValues.size());
	OUTPUT_VALIDITY(!m_maxFitnessValues.empty());
	OUTPUT_VALIDITY(m_gridPhysicalSize > 0);
}

void Scene_Algorithm::init() {
	registerAction(sf::Keyboard::Escape, ActionType::Quit);

	m_maxFitnessValues = std::vector<int>(m_updateLimit);
	m_minFitnessValues = std::vector<int>(m_updateLimit);
	m_avgFitnessValues = std::vector<int>(m_updateLimit);

	reset();

	m_gridPhysicalSize = computeGridPhysicalSize(m_game->getWindow().getSize(), m_relativeGridSize);
	float cellSize = m_gridPhysicalSize / m_gridSize;
	sf::Vector2f cellSizeVec = { cellSize, cellSize };

	m_cell = m_entityManager.addEntity("cell");
	m_cell.addComponent<CTransform>(cellSizeVec / 2.F);
	m_cell.addComponent<CShape>(cellSizeVec, sf::Color::White, sf::Color::Black, 0.F);
	m_cell.addComponent<CText>("0", cellSizeVec / 2.F, m_game->assets().getFont("Roboto"), sf::Color::Black, sf::Color::Black, 0.F);

	m_entityManager.update();
}

void Scene_Algorithm::reset() {
	m_population = initPopulation(m_populationSize, m_gridSize);
	evaluatePopulation(m_population, m_fitnessFunctions[m_curFitnessFuncIndex]);
	sortMostFitInividual(m_population);
	m_colorGene = m_colorFunctions[m_curFitnessFuncIndex](m_population[0]);

	m_curUpdate = 0;
	addGenerationToGraph();

	if (m_isPopulationUpdating) {
		m_shouldDiscardFuturePopulation = true;
	}
}

Population_t Scene_Algorithm::initPopulation(int populationSize, int gridSize) {
	INPUT_VALIDITY(populationSize > 0);
	INPUT_VALIDITY(gridSize > 0);

	Population_t population(populationSize);

	for (auto& individual : population) {
		individual = getRandomGenesIndividual(gridSize);
	}

	OUTPUT_VALIDITY(population.size() == populationSize);
	OUTPUT_VALIDITY(population[0].gene.size() == gridSize * gridSize);

	return population;
}

Individual Scene_Algorithm::getRandomGenesIndividual(int gridSize) {
	INPUT_VALIDITY(gridSize > 0);

	Gene_t gene(gridSize * gridSize);

	for (auto& trait : gene) {
		trait = getRandomNum(0, gridSize - 1);
	}

	OUTPUT_VALIDITY(gene.size() == gridSize * gridSize);

	return Individual(std::move(gene));
}

int Scene_Algorithm::getRandomNum(int rngStart, int rngEnd) {
	INPUT_VALIDITY(rngStart <= rngEnd);

	static std::random_device randDev;
	static std::mt19937 rng(randDev());
	std::uniform_int_distribution<std::mt19937::result_type> range(rngStart, rngEnd);

	int result = static_cast<int>(range(rng));

	OUTPUT_VALIDITY(result >= rngStart && result <= rngEnd);

	return result;
}

void Scene_Algorithm::evaluatePopulation(Population_t& population, const FitnessFunc_t& fitnessFunc) noexcept {
	INPUT_VALIDITY(!population.empty());

	for (auto& individual : population) {
		individual.fitness = fitnessFunc(individual);
	}
}

void Scene_Algorithm::sortMostFitInividual(Population_t& population) {
	INPUT_VALIDITY(!population.empty());

	std::ranges::partial_sort(population, population.begin() + 1, [](const auto& individual1, const auto& individual2) {
		return individual1.fitness > individual2.fitness;
	});
}

void Scene_Algorithm::addGenerationToGraph() {
	int totalPopulationFitness = getTotalPopulationFitness(m_population);
	auto [minFitness, maxFitness] = getMinMaxPopulationFitness(m_population);
	int avgFitness = totalPopulationFitness / m_populationSize;

	m_maxFitnessValues[m_curUpdate] = maxFitness;
	m_minFitnessValues[m_curUpdate] = minFitness;
	m_avgFitnessValues[m_curUpdate] = avgFitness;

	m_curUpdate++;
}

int Scene_Algorithm::getTotalPopulationFitness(const Population_t& population) {
	INPUT_VALIDITY(!population.empty());

	int sum = 0;

	for (const auto& individual : population) {
		sum += individual.fitness;
	}

	return sum;
}

std::pair<int, int> Scene_Algorithm::getMinMaxPopulationFitness(const Population_t& population) {
	INPUT_VALIDITY(!population.empty());

	auto result = std::ranges::minmax_element(population, [](const Individual& individual1, const Individual& individual2) {
		return individual1.fitness < individual2.fitness;
	});

	OUTPUT_VALIDITY(result.min->fitness <= result.max->fitness);

	return { result.min->fitness, result.max->fitness };
}

float Scene_Algorithm::computeGridPhysicalSize(sf::Vector2u windowSize, float relativeGridSize) noexcept {
	INPUT_VALIDITY(windowSize.x > 0 && windowSize.y > 0);
	INPUT_VALIDITY(relativeGridSize > 0);

	float result = std::min(windowSize.x, windowSize.y) * relativeGridSize;

	OUTPUT_VALIDITY(result > 0);

	return result;
}

void Scene_Algorithm::resizeCell(float relativeDif) {
	INPUT_VALIDITY(relativeDif > 0);

	m_cell.getComponent<CTransform>().pos *= relativeDif;

	auto& cellSh = m_cell.getComponent<CShape>().shape;
	cellSh.setSize(cellSh.getSize() * relativeDif);
	cellSh.setOrigin(cellSh.getSize() / 2.F);

	auto& cellText = m_cell.getComponent<CText>();
	cellText.text.setScale(cellText.text.getScale() * relativeDif);
	cellText.containerSize *= relativeDif;
}

Population_t Scene_Algorithm::updatePopulation(Population_t population, int gridSize, int mutationRate, int randomGenesRate, int eliteSurvivalRate, int generationsPerUpdate, int framerateLimit, const FitnessFunc_t& fitnessFunc) {
	INPUT_VALIDITY(!population.empty());
	INPUT_VALIDITY(gridSize > 0);
	INPUT_VALIDITY(population[0].gene.size() == gridSize * gridSize);
	INPUT_VALIDITY(mutationRate >= 0 && mutationRate <= 100);
	INPUT_VALIDITY(randomGenesRate >= 0 && randomGenesRate <= 100);
	INPUT_VALIDITY(eliteSurvivalRate >= 0 && eliteSurvivalRate <= 100);
	INPUT_VALIDITY(generationsPerUpdate > 0);
	INPUT_VALIDITY(framerateLimit > 0);
	
	for (int i = 0; i < generationsPerUpdate; i++) {
		population = geneticAlgorithmOperations(population, gridSize, mutationRate, randomGenesRate, eliteSurvivalRate);
		evaluatePopulation(population, fitnessFunc);
		shufflePopulation(population);
	}

	return population;
}

Population_t Scene_Algorithm::geneticAlgorithmOperations(const Population_t& population, int gridSize, int mutationRate, int randomGenesRate, int eliteSurvivalRate) {
	INPUT_VALIDITY(!population.empty());
	INPUT_VALIDITY(gridSize > 0);
	INPUT_VALIDITY(population[0].gene.size() == gridSize * gridSize);
	INPUT_VALIDITY(mutationRate >= 0 && mutationRate <= 100);
	INPUT_VALIDITY(randomGenesRate >= 0 && randomGenesRate <= 100);
	INPUT_VALIDITY(eliteSurvivalRate >= 0 && eliteSurvivalRate <= 100);

	Population_t newPopulation;
	newPopulation.reserve(population.size());

	int recombinationRate = std::max(0, 100 - mutationRate - randomGenesRate);
	int totalPopulationFitness = getTotalPopulationFitness(population);

	std::vector<int> eliteIndividualsIndices = getEliteIndividualsIndices(population, eliteSurvivalRate);
	for (auto& index : eliteIndividualsIndices) {
		newPopulation.emplace_back(population[index]);
	}

	while (newPopulation.size() < population.size()) {
		GeneticOperation operation = selectGeneticOperation(mutationRate, randomGenesRate, recombinationRate);

		if (operation == GeneticOperation::Mutation) {
			int pickedIndex = rouletteIndexSelect(population, getRandomNum(0, totalPopulationFitness));
			newPopulation.emplace_back(population[pickedIndex]);
			int randomTraitIndex = getRandomNum(0, static_cast<int>(newPopulation.back().gene.size()) - 1);
			newPopulation.back().gene[randomTraitIndex] = getRandomNum(0, gridSize - 1);
		}
		else if (operation == GeneticOperation::RandomGenes) {
			newPopulation.emplace_back(getRandomGenesIndividual(gridSize));
		}
		else if (operation == GeneticOperation::Recombination) {
			int pickedIndex1 = rouletteIndexSelect(population, getRandomNum(0, totalPopulationFitness - 1));
			int pickedIndex2 = rouletteIndexSelect(population, getRandomNum(0, totalPopulationFitness - 1));
			std::array<Individual, 2> recombinedIndividuals = recombineIndividuals(population[pickedIndex1], population[pickedIndex2]);
			newPopulation.emplace_back(std::move(recombinedIndividuals[0]));

			if (newPopulation.size() < population.size()) {
				newPopulation.emplace_back(std::move(recombinedIndividuals[1]));
			}
		}
	}

	OUTPUT_VALIDITY(newPopulation.size() == population.size());
	OUTPUT_VALIDITY(newPopulation[0].gene.size() == population[0].gene.size());

	return newPopulation;
}

std::vector<int> Scene_Algorithm::getEliteIndividualsIndices(const Population_t& population, int eliteSurvivalRate) {
	INPUT_VALIDITY(!population.empty());
	INPUT_VALIDITY(eliteSurvivalRate >= 0);

	int populationSize = static_cast<int>(population.size());
	int eliteAmount = getPercentageValue(populationSize, eliteSurvivalRate);
	std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<>> eliteIndividuals;

	if (eliteAmount == 0) {
		return {};
	}

	for (int i = 0; i < populationSize; i++) {
		if (eliteIndividuals.size() < eliteAmount) {
			eliteIndividuals.emplace(population[i].fitness, i);
		}
		else if (eliteIndividuals.top().first < population[i].fitness) {
			eliteIndividuals.pop();
			eliteIndividuals.emplace(population[i].fitness, i);
		}
	}

	std::vector<int> result(eliteAmount);

	for (int& index : result) {
		index = eliteIndividuals.top().second;
		eliteIndividuals.pop();
	}

	OUTPUT_VALIDITY(result.size() == eliteAmount);
	if (!result.empty()) {
		OUTPUT_VALIDITY(population[result[0]].fitness <= population[result[result.size() - 1]].fitness);
	}

	return result;
}

int Scene_Algorithm::getPercentageValue(int totalSize, int percentage) noexcept {
	INPUT_VALIDITY(totalSize > 0);
	INPUT_VALIDITY(percentage >= 0 && percentage <= 100);

	int result = static_cast<int>(std::ceil(totalSize * (percentage / 100.F)));

	OUTPUT_VALIDITY(result >= 0);

	return result;
}

GeneticOperation Scene_Algorithm::selectGeneticOperation(int mutationRate, int randomGenesRate, int recombinationRate) {
	INPUT_VALIDITY(mutationRate >= 0 && mutationRate <= 100);
	INPUT_VALIDITY(randomGenesRate >= 0 && randomGenesRate <= 100);
	INPUT_VALIDITY(recombinationRate >= 0 && recombinationRate <= 100);

	int sum = mutationRate + randomGenesRate + recombinationRate;
	int randomNum = getRandomNum(0, sum - 1);

	if (randomNum < mutationRate) {
		return GeneticOperation::Mutation;
	}
	else if (randomNum < mutationRate + randomGenesRate) {
		return GeneticOperation::RandomGenes;
	}
	else {
		return GeneticOperation::Recombination;
	}
}

int Scene_Algorithm::rouletteIndexSelect(const Population_t& population, int pickValue) noexcept {
	INPUT_VALIDITY(!population.empty());
	INPUT_VALIDITY(pickValue >= 0);

	int sum = 0;
	for (int i = 0; i < population.size(); i++) {
		sum += population[i].fitness;
		if (sum > pickValue) {
			return i;
		}
	}

	return population.size() - 1;
}

std::array<Individual, 2> Scene_Algorithm::recombineIndividuals(const Individual& individual1, const Individual& individual2) {
	INPUT_VALIDITY(!individual1.gene.empty());
	INPUT_VALIDITY(!individual2.gene.empty());
	INPUT_VALIDITY(individual1.gene.size() == individual2.gene.size());

	std::array<Individual, 2> result = { individual1, individual2 };
	int slicePoint = getRandomNum(0, static_cast<int>(individual1.gene.size()) / 2);

	std::swap_ranges(result[0].gene.begin() + slicePoint, result[0].gene.end(), result[1].gene.begin() + slicePoint);

	OUTPUT_VALIDITY(!result[0].gene.empty());
	OUTPUT_VALIDITY(!result[1].gene.empty());
	OUTPUT_VALIDITY(result[0].gene.size() == result[1].gene.size());

	return result;
}

void Scene_Algorithm::shufflePopulation(Population_t& population) {
	INPUT_VALIDITY(!population.empty());

	static std::random_device randDev;
	static std::mt19937 rng(randDev());
	std::ranges::shuffle(population, rng);
}



int Scene_Algorithm::evaluationSudoku(const Individual& individual) {
	int fitness = 0;
	int gridSize = static_cast<int>(std::sqrt(individual.gene.size()));
	int boxSize = static_cast<int>(std::sqrt(gridSize));

	INPUT_VALIDITY(!individual.gene.empty());
	INPUT_VALIDITY(individual.gene.size() == gridSize * gridSize);
	INPUT_VALIDITY(gridSize == boxSize * boxSize);

	std::vector<int> rowTraits(gridSize);
	std::vector<int> colTraits(gridSize);
	std::vector<int> boxTraits(gridSize);

	for (int i = 0; i < gridSize; i++) {
		std::ranges::fill(rowTraits, 0);
		std::ranges::fill(colTraits, 0);

		for (int j = 0; j < gridSize; j++) {
			rowTraits[individual.gene[i * gridSize + j]]++;
			colTraits[individual.gene[j * gridSize + i]]++;
		}

		for (const auto& trait : rowTraits) {
			fitness += trait == 1;
		}
		for (const auto& trait : colTraits) {
			fitness += trait == 1;
		}
	}

	for (int boxRow = 0; boxRow < boxSize; boxRow++) {
		for (int boxCol = 0; boxCol < boxSize; boxCol++) {
			std::ranges::fill(boxTraits, 0);

			int boxPos = boxRow * boxSize * gridSize + boxCol * boxSize;

			for (int row = 0; row < boxSize; row++) {
				for (int col = 0; col < boxSize; col++) {
					boxTraits[individual.gene[boxPos + row * gridSize + col]]++;
				}
			}

			for (const auto& trait : boxTraits) {
				fitness += trait == 1;
			}
		}
	}

	return fitness;
}

int Scene_Algorithm::evaluationCheckers(const Individual& individual) {
	int fitness = 0;
	int gridSize = static_cast<int>(std::sqrt(individual.gene.size()));

	INPUT_VALIDITY(!individual.gene.empty());
	INPUT_VALIDITY(individual.gene.size() == gridSize * gridSize);

	bool isHigh = true;
	for (int i = 0; i < individual.gene.size(); i++) {
		if (isHigh) {
			fitness += individual.gene[i];
		}
		else {
			fitness += gridSize - 1 - individual.gene[i];
		}

		isHigh = !isHigh;
		if (gridSize % 2 == 0 && i != 0 && (i + 1) % gridSize == 0) {
			isHigh = !isHigh;
		}
	}

	return fitness;
}

int Scene_Algorithm::evaluationMax(const Individual& individual) {
	INPUT_VALIDITY(!individual.gene.empty());

	int fitness = 0;
	for (const auto& trait : individual.gene) {
		fitness += trait;
	}
	return fitness;
}

int Scene_Algorithm::evaluationMin(const Individual& individual) {
	INPUT_VALIDITY(!individual.gene.empty());

	int fitness = 0;
	int gridSize = static_cast<int>(std::sqrt(individual.gene.size()));
	for (const auto& trait : individual.gene) {
		fitness += gridSize - 1 - trait ;
	}
	return fitness;
}

int Scene_Algorithm::evaluationDistToCenter(const Individual& individual) {
	int fitness = 0;
	int gridSize = static_cast<int>(std::sqrt(individual.gene.size()));
	int middle = gridSize / 2;
	bool isSizeEven = !(gridSize & 1);

	INPUT_VALIDITY(!individual.gene.empty());
	INPUT_VALIDITY(individual.gene.size() == gridSize * gridSize);

	for (int row = 0; row < gridSize; row++) {
		for (int col = 0; col < gridSize; col++) {
			int side1 = std::abs(middle - row - (isSizeEven && row < middle));
			int side2 = std::abs(middle - col - (isSizeEven && col < middle));
			int dist = static_cast<int>(std::round(side1 + side2 - std::min(side1, side2) / 2.F));
			fitness += gridSize - std::abs(dist - individual.gene[static_cast<size_t>(row) * gridSize + col]);
		}
	}

	return fitness;
}



Gene_t Scene_Algorithm::colorSudoku(const Individual& individual) {
	Gene_t colorGene(individual.gene.size(), 0);
	int gridSize = static_cast<int>(std::sqrt(individual.gene.size()));
	int boxSize = static_cast<int>(std::sqrt(gridSize));

	INPUT_VALIDITY(!individual.gene.empty());
	INPUT_VALIDITY(individual.gene.size() == gridSize * gridSize);
	INPUT_VALIDITY(gridSize == boxSize * boxSize);

	std::vector<int> rowTraits(gridSize);
	std::vector<int> colTraits(gridSize);
	std::vector<int> boxTraits(gridSize);

	for (size_t i = 0; i < gridSize; i++) {
		std::ranges::fill(rowTraits, 0);
		std::ranges::fill(colTraits, 0);

		for (size_t j = 0; j < gridSize; j++) {
			rowTraits[individual.gene[i * gridSize + j]]++;
			colTraits[individual.gene[j * gridSize + i]]++;
		}

		for (size_t j = 0; j < gridSize; j++) {
			colorGene[i * gridSize + j] += rowTraits[individual.gene[i * gridSize + j]];
			colorGene[j * gridSize + i] += colTraits[individual.gene[j * gridSize + i]];
		}
	}

	for (size_t boxRow = 0; boxRow < boxSize; boxRow++) {
		for (size_t boxCol = 0; boxCol < boxSize; boxCol++) {
			std::ranges::fill(boxTraits, 0);

			size_t boxPos = boxRow * boxSize * gridSize + boxCol * boxSize;

			for (size_t row = 0; row < boxSize; row++) {
				for (size_t col = 0; col < boxSize; col++) {
					boxTraits[individual.gene[boxPos + row * gridSize + col]]++;
				}
			}

			for (size_t row = 0; row < boxSize; row++) {
				for (size_t col = 0; col < boxSize; col++) {
					colorGene[boxPos + row * gridSize + col] += boxTraits[individual.gene[boxPos + row * gridSize + col]];
				}
			}
		}
	}

	for (auto& colorTrait : colorGene) {
		colorTrait = std::min(colorTrait - 3, gridSize);
	}

	OUTPUT_VALIDITY(colorGene.size() == individual.gene.size());

	return colorGene;
}

Gene_t Scene_Algorithm::colorMax(const Individual& individual) {
	INPUT_VALIDITY(!individual.gene.empty());

	return individual.gene;
}



#pragma region GUI
void Scene_Algorithm::gui() {
	controls();
	graph();
	//ImGui::ShowDemoWindow();
	//ImPlot::ShowDemoWindow();
}

void Scene_Algorithm::controls() {
	ImVec2 controlsPos = { 0, m_gridPhysicalSize };
	ImVec2 controlsSize = { static_cast<float>(m_game->getWindow().getSize().x), m_game->getWindow().getSize().y - m_gridPhysicalSize};

	ImGui::SetNextWindowPos(controlsPos);
	ImGui::SetNextWindowSize(controlsSize);

	if (ImGui::Begin("Controls window", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, controlsSize.x / 2.F);

		sliderGridSize();
		sliderPopulationSize(); 
		sliderMutationRate();
		sliderRandomGenesRate();
		sliderEliteSurvivalRate();

		ImGui::NextColumn();

		comboWindowSize();
		comboFitnessFunc();
		buttonPause();
	}
	ImGui::End();
}

void Scene_Algorithm::sliderGridSize() {
	static int boxSize = static_cast<int>(std::sqrt(m_gridSize));
	ImGui::SliderInt("Grid size", &boxSize, 1, 6, nullptr, ImGuiSliderFlags_AlwaysClamp);
	if (ImGui::IsItemDeactivatedAfterEdit()) {
		m_gridSize = boxSize * boxSize;
		m_gridPhysicalSize = computeGridPhysicalSize(m_game->getWindow().getSize(), m_relativeGridSize);
		float cellSize = m_gridPhysicalSize / m_gridSize;
		float relativeDif = cellSize / m_cell.getComponent<CShape>().shape.getSize().x;
		resizeCell(relativeDif);

		reset();

		if (m_isPopulationUpdating) {
			m_shouldDiscardFuturePopulation = true;
		}
	}
}

void Scene_Algorithm::sliderPopulationSize() {
	static int tempPopulationSize = m_populationSize;
	ImGui::SliderInt("Population size", &tempPopulationSize, 1, 1000, nullptr, ImGuiSliderFlags_AlwaysClamp);
	if (ImGui::IsItemDeactivatedAfterEdit()) {
		int prevSize = m_populationSize;
		m_populationSize = tempPopulationSize;
		m_population.resize(m_populationSize);

		for (int i = prevSize; i < m_populationSize; i++) {
			m_population[i] = getRandomGenesIndividual(m_gridSize);
		}

		if (m_isPopulationUpdating) {
			m_shouldDiscardFuturePopulation = true;
		}
	}
}

void Scene_Algorithm::sliderMutationRate() {
	static int sliderMutationRate = m_mutationRate;
	ImGui::SliderInt("Mutation rate", &sliderMutationRate, 0, 100, nullptr, ImGuiSliderFlags_AlwaysClamp);
	if (ImGui::IsItemDeactivatedAfterEdit()) {
		m_mutationRate = sliderMutationRate;
	}
}

void Scene_Algorithm::sliderRandomGenesRate() {
	static int sliderRandomGenesRate = m_randomGenesRate;
	ImGui::SliderInt("Random genomes rate", &sliderRandomGenesRate, 0, 100, nullptr, ImGuiSliderFlags_AlwaysClamp);
	if (ImGui::IsItemDeactivatedAfterEdit()) {
		m_randomGenesRate = sliderRandomGenesRate;
	}
}

void Scene_Algorithm::sliderEliteSurvivalRate() {
	static int sliderEliteSurvivalRate = m_eliteSurvivalRate;
	ImGui::SliderInt("Elite survival rate", &sliderEliteSurvivalRate, 0, 100, nullptr, ImGuiSliderFlags_AlwaysClamp);
	if (ImGui::IsItemDeactivatedAfterEdit()) {
		m_eliteSurvivalRate = sliderEliteSurvivalRate;
	}
}

void Scene_Algorithm::comboWindowSize() {
	static const std::vector<const char*> comboItems = { "Full screen", "3840x2160", "2560x1440" , "1920x1080" , "1600x900", "1280x720" , "1024x576" };
	static const std::vector<int> windowSizes = { 1, 240, 160, 120, 100, 80, 64 };
	static int currentItemIndex = 6;
	static int prevItemIndex = currentItemIndex;
	static int itemAmount = static_cast<int>(comboItems.size());

	if (ImGui::Combo("Window Size", &currentItemIndex, comboItems.data(), itemAmount, itemAmount)) {
		sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();

		currentItemIndex = getOptimalScreenSizeIndex(sf::Vector2i(desktopMode.width, desktopMode.height), windowSizes, currentItemIndex);

		if (currentItemIndex == 0) {
			m_game->getWindow().create(desktopMode, "Sudoku", sf::Style::Fullscreen);
		}
		else {
			if (prevItemIndex == 0) {
				m_game->getWindow().create(sf::VideoMode(16 * windowSizes[currentItemIndex], 9 * windowSizes[currentItemIndex]), "Sudoku", sf::Style::Close);
			}
			else {
				m_game->getWindow().setSize(sf::Vector2u(16 * windowSizes[currentItemIndex], 9 * windowSizes[currentItemIndex]));
			}
		}
		float prevGridPhysicalSize = m_gridPhysicalSize;
		m_gridPhysicalSize = computeGridPhysicalSize(m_game->getWindow().getSize(), m_relativeGridSize);
		resizeCell(m_gridPhysicalSize / prevGridPhysicalSize);
		prevItemIndex = currentItemIndex;
	}
}

int Scene_Algorithm::getOptimalScreenSizeIndex(sf::Vector2i maxSceenSize, const std::vector<int>& screenSizes, int selectedIndex) noexcept {
	INPUT_VALIDITY(maxSceenSize.x > 0 && maxSceenSize.y > 0);
	INPUT_VALIDITY(!screenSizes.empty());
	INPUT_VALIDITY(selectedIndex >= 0);

	if (selectedIndex == 0) {
		return 0;
	}

	sf::Vector2i screenRatio = { 16, 9 };

	for (int i = selectedIndex; i < screenSizes.size(); i++) {
		if (maxSceenSize.x / screenRatio.x > screenSizes[i] && maxSceenSize.y / screenRatio.y > screenSizes[i]) {
			return i;
		}
	}

	return 0;
}

void Scene_Algorithm::comboFitnessFunc() {
	static const std::vector<const char*> comboItems = { "Sudoku", "Checkers", "Max", "Min", "DistToCenter" };

	static int currentItemIndex = m_curFitnessFuncIndex;
	static int itemAmount = static_cast<int>(comboItems.size());

	if (ImGui::Combo("Fitness function", &currentItemIndex, comboItems.data(), itemAmount, itemAmount)) {
		m_curFitnessFuncIndex = currentItemIndex;
		reset();
	}
}

void Scene_Algorithm::buttonPause() {
	if (ImGui::Button("Pause genetic algorithm")) {
		m_isPaused = !m_isPaused;
	}
}

void Scene_Algorithm::graph() {
	ImVec2 graphPos = { m_gridPhysicalSize, 0 };
	ImVec2 graphSize = { m_game->getWindow().getSize().x - m_gridPhysicalSize, m_gridPhysicalSize };

	ImGui::SetNextWindowPos(graphPos);
	ImGui::SetNextWindowSize(graphSize);

	if (!m_isPaused) {
		ImPlot::SetNextAxesToFit();
	}

	if (ImGui::Begin("Graph window", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
		if (ImPlot::BeginPlot("Plots", ImGui::GetContentRegionAvail(), ImPlotFlags_NoTitle | ImPlotFlags_NoMenus)) {
			ImPlot::PlotLine("Max fitness", m_maxFitnessValues.data(), m_curUpdate, m_generationsPerUpdate);
			ImPlot::PlotLine("Min fitness", m_minFitnessValues.data(), m_curUpdate, m_generationsPerUpdate);
			ImPlot::PlotLine("Avg fitness", m_avgFitnessValues.data(), m_curUpdate, m_generationsPerUpdate);
		}
		ImPlot::EndPlot();
	}
	ImGui::End();
}
#pragma endregion




void Scene_Algorithm::update() {
	if (!m_isPaused) {
		if (!m_isPopulationUpdating && m_curUpdate < m_updateLimit) {
			m_isPopulationUpdating = true;
			m_futurePopulation = std::async(std::launch::async, &Scene_Algorithm::updatePopulation, m_population, m_gridSize, m_mutationRate, m_randomGenesRate, m_eliteSurvivalRate, 
											m_generationsPerUpdate, m_game->getFramerateLimit(), std::ref(m_fitnessFunctions[m_curFitnessFuncIndex]));
		}

		if (m_futurePopulation.valid() && m_futurePopulation.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			if (!m_shouldDiscardFuturePopulation) {
				m_population = m_futurePopulation.get();
				sortMostFitInividual(m_population);
				m_colorGene = m_colorFunctions[m_curFitnessFuncIndex](m_population[0]);
				addGenerationToGraph();
			}
			else {
				m_futurePopulation.get();
			}
			m_isPopulationUpdating = false;
			m_shouldDiscardFuturePopulation = false;
		}
	}
	m_currentFrame++;
}

void Scene_Algorithm::sDoAction(const Action& action) {
	switch (action.getStage()) {
	case ActionStage::Start:
		switch (action.getType()) {
		case ActionType::Quit:
			m_game->quit();
			break;
		//case ActionType::LeftClick: break;
		//case ActionType::RightClick: break;
		//case ActionType::MiddleClick: break;
		//case ActionType::MouseMove: break;
		//case ActionType::Resized: break;
		}
	//case ActionStage::End:
		//switch (action.getType()) {
		//case ActionType::LeftClick: break;
		//case ActionType::RightClick: break;
		//case ActionType::MiddleClick: break;
		//}
	}
}

void Scene_Algorithm::sRender() {
	drawCells();
	drawBoxLines();
	gui();
}

void Scene_Algorithm::drawCells() {
	auto& cellSh = m_cell.getComponent<CShape>().shape;
	auto& cellText = m_cell.getComponent<CText>().text;

	sf::Vector2f cellPos = m_cell.getComponent<CTransform>().pos;
	sf::Vector2f cellSize = cellSh.getSize();
	float colorShare = 255 / (m_gridSize * m_colorWeights[m_curFitnessFuncIndex]);

	auto& mostFitIndividual = m_population[0];

	for (int i = 0; i < mostFitIndividual.gene.size(); i++) {
		int row = i / m_gridSize;
		int col = i % m_gridSize;

		sf::Vector2f newPos = cellPos + sf::Vector2f(col * cellSize.x, row * cellSize.y);
		int gbColor = 255 - std::min(static_cast<int>(std::round(colorShare * m_colorGene[i])), 255);
		cellSh.setPosition(newPos);
		cellSh.setFillColor(sf::Color(255, gbColor, gbColor));

		cellText.setString(std::to_string(mostFitIndividual.gene[i] + 1));
		cellText.setPosition(newPos);
		MyUtils::centerText(cellText);

		m_game->getWindow().draw(cellSh);
		m_game->getWindow().draw(cellText);
	}
}

void Scene_Algorithm::drawBoxLines() {
	float cellSize = m_cell.getComponent<CShape>().shape.getSize().x;
	int boxSize = static_cast<int>(std::sqrt(m_gridSize));
	sf::VertexArray lines(sf::Lines, (boxSize - 1) * 2 * 2);

	size_t curIndex = 0;
	for (int i = 1; i < boxSize; i++) {
		lines[curIndex++] = sf::Vertex(sf::Vector2f(cellSize * boxSize * i, 0), sf::Color::Black);
		lines[curIndex++] = sf::Vertex(sf::Vector2f(cellSize * boxSize * i, m_gridPhysicalSize), sf::Color::Black);

		lines[curIndex++] = sf::Vertex(sf::Vector2f(0, cellSize * boxSize * i), sf::Color::Black);
		lines[curIndex++] = sf::Vertex(sf::Vector2f(m_gridPhysicalSize, i * cellSize * boxSize ), sf::Color::Black);
	}
	m_game->getWindow().draw(lines);
}