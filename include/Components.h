#pragma once
#include "Animation.h"


class ComponentBase {
public:
	bool has = false;
};


class CTransform : public ComponentBase {
public:
	sf::Vector2f pos = { 0, 0 };

	CTransform();
	CTransform(sf::Vector2f posPrm);
};


class CAnimation : public ComponentBase {
public:
	Animation animation;
	bool isRepeating = false;

	CAnimation();
	CAnimation(const Animation& animationPrm, bool isRepeatingPrm);
};


class CShape : public ComponentBase {
public:
	sf::RectangleShape shape;

	CShape();
	CShape(sf::Vector2f size, sf::Color fill, sf::Color outline, float thickness);

	void setScale(sf::Vector2f relVal, bool maintainRatio);
};


class CText : public ComponentBase {
public:
	sf::Text text;
	sf::Vector2f containerSize = { 0, 0 };

	CText();
	CText(const std::string& textStr, sf::Vector2f containerSizePrm, const sf::Font& font, sf::Color fill, sf::Color outline, float thickness);

	void setScale(sf::Vector2f relVal, bool maintainRatio);
	void centerText();
};


class CCell : public ComponentBase {
public:
	char val  = 0;

	CCell();
	CCell(char valPrm);
};