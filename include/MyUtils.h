#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <type_traits>

#define CHECK_INPUT_OUTPUT_VALIDITY false
#ifdef CHECK_INPUT_OUTPUT_VALIDITY
	#define INPUT_VALIDITY(condition) \
		do{ \
			if (!(condition)) { \
				std::cerr << __FUNCTION__ << ": Invalid input at line: " << __LINE__ << '\n'; \
				std::abort(); \
			} \
		} while(0)

	#define OUTPUT_VALIDITY(condition) \
		do{ \
			if (!(condition)) { \
				std::cerr << __FUNCTION__ << ": Invalid output at line: " << __LINE__ << '\n'; \
				 \
				std::abort(); \
			} \
		} while(0)
#else
	#define INPUT_VALIDITY(condition)
	#define OUTPUT_VALIDITY(condition)
#endif


namespace MyUtils {
	void centerText(sf::Text& text) noexcept;

	template <typename T>
	requires std::is_same_v<T, sf::Sprite> || std::is_same_v<T, sf::Shape> || std::is_same_v<T, sf::Text>
	void setScale(T& element, sf::Vector2f relVal, bool maintainRatio) noexcept {
		INPUT_VALIDITY(relVal.x > 0 && relVal.y > 0);

		const sf::Vector2f elementSize = element.getLocalBounds().getSize();
		sf::Vector2f realScale;
		if (maintainRatio) {
			float minScale = std::min(relVal.x / elementSize.x, relVal.y / elementSize.y);
			realScale = { minScale , minScale };
		}
		else {
			realScale = { relVal.x / elementSize.x, relVal.y / elementSize.y };
		}
		element.setScale(realScale.x, realScale.y);
	}
}
