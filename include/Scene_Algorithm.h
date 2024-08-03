#pragma once
#include <random>
#include "Scene.h"


class Scene_Algorithm : public Scene {
public:
	Scene_Algorithm(GameEngine* gameEngine);
private:
	std::vector<std::vector<Entity>> m_grid;
	std::vector<std::pair<int, std::vector<char>>> m_population;

	int m_populationSize = 100;
	int m_mutationRate = 20;
	int m_randomGenesRate = 5;
	int m_eliteSurvivalRate = 10;
	char m_gridSize = 9;

	void init();
	void resetGridAndPopulation();
	void resizeGrid(float relativeDif);
	void resizePopulation();
	void initGrid();
	Entity createCell(sf::Vector2f pos, sf::Vector2f size, char val);
	void initPopulation(int startIndex);
	char getRandCellVal() const;
	void sortElitePopulation();
	void updateCells();
	void gui();
	void onEnd();
	void pause();
	void update() override;
	void sDoAction(const Action& action) override;
	void sRender() override;
};
