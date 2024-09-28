#pragma once
#include <SFML/Graphics.hpp>
#include "MyUtils.h"


class Animation {
public:
	Animation();
	explicit Animation(const std::string& name, const sf::Texture& texture, int frameCount, int speed);

	void update();
	[[nodiscard]] const std::string& getName() const noexcept;
	[[nodiscard]] const sf::Sprite& getSprite() const noexcept;
	[[nodiscard]] sf::Sprite& getSprite() noexcept;
	[[nodiscard]] bool hasEnded() const noexcept;

private:
	sf::Sprite m_sprite;
	std::string m_name = "NONE";
	sf::Vector2u m_tSize  = { 1, 1 };
	int m_frameCount   = 1;
	int m_currentFrame = 0;
	int m_speed        = 0;
};
