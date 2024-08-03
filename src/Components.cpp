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

	shape.setFillColor(fill);
	shape.setOutlineColor(outline);
	shape.setOutlineThickness(thickness);
	shape.setOrigin(size.x / 2, size.y / 2);
}

void CShape::setScale(sf::Vector2f relVal, bool maintainRatio) {
	sf::Vector2f shapeSize = shape.getLocalBounds().getSize();

	if (maintainRatio) {
		float realScale = std::min(relVal.x / shapeSize.x, relVal.y / shapeSize.y);
		shape.setScale(realScale, realScale);
	}
	else {
		sf::Vector2f realScale = { relVal.x / shapeSize.x, relVal.y / shapeSize.y };
		shape.setScale(realScale.x, realScale.y);
	}
}


CText::CText() {}

CText::CText(const std::string& textStr, sf::Vector2f containerSizePrm, const sf::Font& font, sf::Color fill, sf::Color outline, float thickness)
	: containerSize{ containerSizePrm } {

	text.setString(textStr);
	text.setFont(font);
	text.setCharacterSize(200);
	text.setFillColor(fill);
	text.setOutlineColor(outline);
	text.setOutlineThickness(thickness);

	setScale(containerSize, true);
	centerText();
}

void CText::setScale(sf::Vector2f relVal, bool maintainRatio) {
	sf::Vector2f textSize = text.getLocalBounds().getSize();

	if (maintainRatio) {
		float realScale = std::min(relVal.x / textSize.x, relVal.y / textSize.y);
		text.setScale(realScale, realScale);
	}
	else {
		sf::Vector2f realScale = { relVal.x / textSize.x, relVal.y / textSize.y };
		text.setScale(realScale.x, realScale.y);
	}
}

void CText::centerText() {
	sf::FloatRect textSizeRect = text.getLocalBounds();
	text.setOrigin(textSizeRect.left + textSizeRect.width / 2.f, textSizeRect.top + textSizeRect.height / 2.f);
}


CCell::CCell() {}
CCell::CCell(char valPrm)
	: val{ valPrm } {}