#pragma once
#include <random>
#include <numeric>
#include <queue>
#include <array>
#include <future>
#include "Scene.h"
#include "Timer.h"

using Gene_t = std::vector<int>;

class Individual {
public:
	Gene_t gene;
	int fitness = 0;

	Individual() {}

	Individual(Gene_t&& genePrm)
		: gene(std::move(genePrm)) {

		INPUT_VALIDITY(!this->gene.empty());
	}
};


using Population_t = std::vector<Individual>;


class Scene_Algorithm : public Scene {

public:
	Scene_Algorithm(GameEngine* gameEngine);
private:
	Population_t m_population;

	std::vector<int> m_maxFitnessValues;
	std::vector<int> m_minFitnessValues;
	std::vector<int> m_avgFitnessValues;

	std::vector<std::function<int(const Individual& individual)>> m_fitnessFunctions = {
		[this](const Individual& individual) -> int { return evaluationSudoku(individual); },
		[this](const Individual& individual) -> int { return evaluationCheckers(individual); },
		[this](const Individual& individual) -> int { return evaluationMax(individual); },
		[this](const Individual& individual) -> int { return evaluationMin(individual); },
		[this](const Individual& individual) -> int { return evaluationDistToCenter(individual); }
	};
	std::vector<std::function<Gene_t(const Individual& individual)>> m_colorFunctions = {
		[this](const Individual& individual) -> Gene_t { return colorSudoku(individual); },
		[this](const Individual& individual) -> Gene_t { return colorMax(individual); },
		[this](const Individual& individual) -> Gene_t { return colorMax(individual); },
		[this](const Individual& individual) -> Gene_t { return colorMax(individual); },
		[this](const Individual& individual) -> Gene_t { return colorMax(individual); }
	};
	std::vector<float> m_colorWeights = { 0.3f, 1.f, 1.f, 1.f, 1.f };

	std::future<Population_t> m_futurePopulation;
	Gene_t m_colorGene;

	float m_relativeGridSize = 3.f / 4.f;
	float m_gridPhysicalSize;

	int m_curFitnessFuncIndex  = 0;
	int m_generationsPerUpdate = 10;
	int m_updateLimit          = 5'000;
	int m_curUpdate            = 0;
	int m_populationSize       = 100;
	int m_mutationRate         = 20;
	int m_randomGenesRate      = 5;
	int m_eliteSurvivalRate    = 10;
	int m_gridSize = 9;

	Entity m_cell;
	bool m_isPopulationUpdating = false;
	bool m_shouldDiscardFuturePopulation = false;


	void init();
	void reset();
	[[nodiscard]] Population_t initPopulation(int populationSize, int gridSize) const;
	[[nodiscard]] Individual getRandomGenesIndividual(int gridSize) const;
	[[nodiscard]] int getRandomNum(int rngStart, int rngEnd) const;
	void evaluatePopulation(Population_t& population) const noexcept;
	void sortMostFitInividual(Population_t& population) const;
	void addGenerationToGraph();
	[[nodiscard]] int getTotalPopulationFitness(const Population_t& population) const;
	[[nodiscard]] std::pair<int, int> getMinMaxPopulationFitness(const Population_t& population) const;
	[[nodiscard]] float computeGridPhysicalSize(sf::Vector2u windowSize, float relativeGridSize) const noexcept;
	void resizeCell(float relativeDif);
	[[nodiscard]] Population_t updatePopulation(const Population_t& population, int gridSize, int mutationRate, int randomGenesRate, int eliteSurvivalRate, int generationsPerUpdate, int framerateLimit) const;
	[[nodiscard]] Population_t geneticAlgorithmOperations(const Population_t& population, int gridSize, int mutationRate, int randomGenesRate, int eliteSurvivalRate) const;
	[[nodiscard]] std::vector<int> getEliteIndividualsIndices(const Population_t& population, int eliteSurvivalRate) const;
	[[nodiscard]] int getPercentageValue(int totalSize, int percentage) const noexcept;
	[[nodiscard]] int selectGeneticAlgorithmOperation(int mutationRate, int randomGenesRate, int recombinationRate) const;
	[[nodiscard]] int rouletteIndexSelect(const Population_t& population, int pickValue) const noexcept;
	[[nodiscard]] std::array<Individual, 2> recombineIndividuals(const Individual& individual1, const Individual& individual2) const;
	void shufflePopulation(Population_t& population) const;

	[[nodiscard]] int evaluationSudoku(const Individual& individual) const;
	[[nodiscard]] int evaluationCheckers(const Individual& individual) const;
	[[nodiscard]] int evaluationMax(const Individual& individual) const;
	[[nodiscard]] int evaluationMin(const Individual& individual) const;
	[[nodiscard]] int evaluationDistToCenter(const Individual& individual) const;

	[[nodiscard]] Gene_t colorSudoku(const Individual& individual) const;
	[[nodiscard]] Gene_t colorMax(const Individual& individual) const;

#pragma region GUI
	void gui();
	void controls();
	void sliderGridSize();
	void sliderPopulationSize();
	void sliderMutationRate();
	void sliderRandomGenesRate();
	void sliderEliteSurvivalRate();
	void comboWindowSize();
	[[nodiscard]] int getOptimalScreenSizeIndex(sf::Vector2i maxSceenSize, const std::vector<int>& screenSizes, int selectedIndex) const noexcept;
	void comboFitnessFunc();
	void buttonPause();
	void graph();
#pragma endregion 

	void update() override;
	void sDoAction(const Action& action) override;
	void sRender() override;
	void drawCells();
	void drawBoxLines();
};