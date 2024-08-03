#pragma once
#include <SFML/Graphics.hpp>


class Animation {
public:
	Animation();
	Animation(const std::string& name, const sf::Texture& texture, int frameCount, int speed);

	void update();
	bool hasEnded() const;
	const std::string& getName() const;
	const sf::Sprite& getSprite() const;
	sf::Sprite& getSprite();
	void setScale(sf::Vector2f relVal, bool maintainRatio);
	void changeDirecHor();

private:
	sf::Sprite m_sprite;
	std::string m_name = "NONE";
	sf::Vector2u m_tSize  = { 1, 1 };
	int m_frameCount   = 1;
	int m_currentFrame = 0;
	int m_speed        = 0;
};
