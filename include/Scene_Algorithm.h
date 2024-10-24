#pragma once
#include <random>
#include <numeric>
#include <queue>
#include <array>
#include <future>
#include <ranges>
#include <algorithm>
#include <utility>
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

using Population_t  = std::vector<Individual>;
using FitnessFunc_t = std::function<int(const Individual& individual)>;
using ColorFunc_t   = std::function<Gene_t(const Individual& individual)>;

enum class GeneticOperation : uint8_t {
	Mutation = 0,
	RandomGenes,
	Recombination
};

class Scene_Algorithm : public Scene {

public:
	Scene_Algorithm(GameEngine* gameEngine);
private:
	Population_t m_population;

	std::vector<int> m_maxFitnessValues;
	std::vector<int> m_minFitnessValues;
	std::vector<int> m_avgFitnessValues;

	std::vector<FitnessFunc_t> m_fitnessFunctions = {
		[this](const Individual& individual) -> int { return evaluationSudoku(individual); },
		[this](const Individual& individual) -> int { return evaluationCheckers(individual); },
		[this](const Individual& individual) -> int { return evaluationMax(individual); },
		[this](const Individual& individual) -> int { return evaluationMin(individual); },
		[this](const Individual& individual) -> int { return evaluationDistToCenter(individual); }
	};
	std::vector<ColorFunc_t> m_colorFunctions = {
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
	float m_gridPhysicalSize = 0;

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
	bool m_isPopulationUpdating          = false;
	bool m_shouldDiscardFuturePopulation = false;


	void init();
	void reset();
	[[nodiscard]] static Population_t initPopulation(int populationSize, int gridSize);
	[[nodiscard]] static Individual getRandomGenesIndividual(int gridSize);
	[[nodiscard]] static int getRandomNum(int rngStart, int rngEnd);
	static void evaluatePopulation(Population_t& population, const FitnessFunc_t& fitnessFunc) noexcept;
	static void sortMostFitInividual(Population_t& population);
	void addGenerationToGraph();
	[[nodiscard]] static int getTotalPopulationFitness(const Population_t& population);
	[[nodiscard]] static std::pair<int, int> getMinMaxPopulationFitness(const Population_t& population);
	[[nodiscard]] static float computeGridPhysicalSize(sf::Vector2u windowSize, float relativeGridSize) noexcept;
	void resizeCell(float relativeDif);
	[[nodiscard]] static Population_t updatePopulation(Population_t population, int gridSize, int mutationRate, int randomGenesRate, int eliteSurvivalRate, int generationsPerUpdate, int framerateLimit, const FitnessFunc_t& fitnessFunc);
	[[nodiscard]] static Population_t geneticAlgorithmOperations(const Population_t& population, int gridSize, int mutationRate, int randomGenesRate, int eliteSurvivalRate);
	[[nodiscard]] static std::vector<int> getEliteIndividualsIndices(const Population_t& population, int eliteSurvivalRate);
	[[nodiscard]] static int getPercentageValue(int totalSize, int percentage) noexcept;
	[[nodiscard]] static GeneticOperation selectGeneticOperation(int mutationRate, int randomGenesRate, int recombinationRate);
	[[nodiscard]] static int rouletteIndexSelect(const Population_t& population, int pickValue) noexcept;
	[[nodiscard]] static std::array<Individual, 2> recombineIndividuals(const Individual& individual1, const Individual& individual2);
	static void shufflePopulation(Population_t& population);

	[[nodiscard]] static int evaluationSudoku(const Individual& individual);
	[[nodiscard]] static int evaluationCheckers(const Individual& individual);
	[[nodiscard]] static int evaluationMax(const Individual& individual);
	[[nodiscard]] static int evaluationMin(const Individual& individual);
	[[nodiscard]] static int evaluationDistToCenter(const Individual& individual);

	[[nodiscard]] static Gene_t colorSudoku(const Individual& individual);
	[[nodiscard]] static Gene_t colorMax(const Individual& individual);

#pragma region GUI
	void gui();
	void controls();
	void sliderGridSize();
	void sliderPopulationSize();
	void sliderMutationRate();
	void sliderRandomGenesRate();
	void sliderEliteSurvivalRate();
	void comboWindowSize();
	[[nodiscard]] static int getOptimalScreenSizeIndex(sf::Vector2i maxSceenSize, const std::vector<int>& screenSizes, int selectedIndex) noexcept;
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