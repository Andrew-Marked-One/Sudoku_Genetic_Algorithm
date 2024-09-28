#include "MyUtils.h"


void MyUtils::centerText(sf::Text& text) noexcept {
	const sf::FloatRect textSizeRect = text.getLocalBounds();
	text.setOrigin(textSizeRect.left + textSizeRect.width / 2.f, textSizeRect.top + textSizeRect.height / 2.f);
}