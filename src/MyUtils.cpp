#include "MyUtils.h"


void MyUtils::centerText(sf::Text& text) noexcept {
	const sf::FloatRect textSizeRect = text.getLocalBounds();
	const sf::Vector2f textHalfSize = textSizeRect.getSize() / 2.f;
	text.setOrigin(textSizeRect.left + textHalfSize.x, textSizeRect.top + textHalfSize.y);
}