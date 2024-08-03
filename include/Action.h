#pragma once
#include <SFML/Graphics.hpp>

class Action {
public:
	Action();
	Action(const std::string& name, bool isStart);
	Action(const std::string& name, bool isStart, sf::Vector2f pos);

	const std::string& name() const;
	bool isStart() const;
	sf::Vector2f pos() const;

private:
	std::string m_name = "NONE";
	sf::Vector2f m_pos = { 0, 0 };
	bool m_isStart = false;
};
	
