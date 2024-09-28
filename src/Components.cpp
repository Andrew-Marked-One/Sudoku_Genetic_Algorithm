#include "Components.h"


CTransform::CTransform() {}

CTransform::CTransform(sf::Vector2f posPrm)
	: pos{ posPrm } {}


CAnimation::CAnimation() {}

CAnimation::CAnimation(const Animation& animationPrm, bool isRepeatingPrm)
	: animation{ animationPrm }
	, isRepeating{ isRepeatingPrm } {}


CShape::CShape() {}

CShape::CShape(sf::Vector2f size, sf::Color fill, sf::Color outline, float thickness)
	: shape{ size } {

	INPUT_VALIDITY(size.x >= 0 && size.y >= 0);

	shape.setFillColor(fill);
	shape.setOutlineColor(outline);
	shape.setOutlineThickness(thickness);
	shape.setOrigin(size.x / 2, size.y / 2);
}


CText::CText() {}

CText::CText(const std::string& textStr, sf::Vector2f containerSizePrm, const sf::Font& font, sf::Color fill, sf::Color outline, float thickness)
	: containerSize{ containerSizePrm } {

	INPUT_VALIDITY(containerSizePrm.x >= 0 && containerSizePrm.y >= 0);

	text.setString(textStr);
	text.setFont(font);
	text.setCharacterSize(200);
	text.setFillColor(fill);
	text.setOutlineColor(outline);
	text.setOutlineThickness(thickness);

	MyUtils::setScale(text, containerSize, true);
	MyUtils::centerText(text);
}