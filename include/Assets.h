#pragma once
#include <unordered_map>
#include <memory>
#include <fstream>
#include <sstream>
#include "Animation.h"


class Assets {
public:
	void loadAssets(const std::string& assetsPath);
	[[nodiscard]] const Animation& getAnimation(const std::string& name) const noexcept;
	[[nodiscard]] const sf::Font& getFont(const std::string& name) const noexcept;

private:
	std::unordered_map<std::string, sf::Font>                   m_fontMap;
	std::unordered_map<std::string, sf::Texture>                m_textureMap;
	std::unordered_map<std::string, std::unique_ptr<Animation>> m_animationMap;

	[[nodiscard]] static sf::Texture loadTextures(const std::string& path);
	[[nodiscard]] static std::unique_ptr<Animation> loadAnimation(const std::string& name, const sf::Texture& texture, int frames, int speed);
	[[nodiscard]] static sf::Font loadFont(const std::string& path);
};
