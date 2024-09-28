#pragma once
#include "Animation.h"
#include "MyUtils.h"


class ComponentBase {
public:
	bool has = false;
};


class CTransform : public ComponentBase {
public:
	sf::Vector2f pos = { 0, 0 };

	CTransform();
	explicit CTransform(sf::Vector2f posPrm);
};


class CAnimation : public ComponentBase {
public:
	bool isRepeating = false;
	Animation animation;

	CAnimation();
	explicit CAnimation(const Animation& animationPrm, bool isRepeatingPrm);
};


class CShape : public ComponentBase {
public:
	sf::RectangleShape shape;

	CShape();
	explicit CShape(sf::Vector2f size, sf::Color fill, sf::Color outline, float thickness);
};


class CText : public ComponentBase {
public:
	sf::Text text;
	sf::Vector2f containerSize = { 0, 0 };

	CText();
	explicit CText(const std::string& textStr, sf::Vector2f containerSizePrm, const sf::Font& font, sf::Color fill, sf::Color outline, float thickness);
};