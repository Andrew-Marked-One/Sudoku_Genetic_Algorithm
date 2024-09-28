#include "Assets.h"


void Assets::loadAssets(const std::string& assetsPath) {
	std::ifstream file(assetsPath);
	if (!file.is_open()) {
		std::cerr << "Assets::loadAssets: Cannot open file with path \"" << assetsPath << "\"\n";
	}
	std::string line;
	while (std::getline(file, line)) {
		std::stringstream iss(line);
		std::string type, name, path;
		iss >> type;
		if (type == "Texture") {
			iss >> name >> path;
			m_textureMap[name] = loadTextures(path);
		}
		else if (type == "Animation") {
			std::string textName;
			int frames, duration;
			iss >> name >> textName >> frames >> duration;
			m_animationMap[name] = loadAnimation(name, m_textureMap[textName], frames, duration);
		}
		else if (type == "Font") {
			iss >> name >> path;
			m_fontMap[name] = loadFont(path);
		}
	}
}

sf::Texture Assets::loadTextures(const std::string& path) const {
	sf::Texture texture;
	if (!texture.loadFromFile(path)) {
		std::cerr << "Assets::loadTextures: Cannot open file with path \"" << path << "\"\n";
	}
	return texture;
}

std::unique_ptr<Animation> Assets::loadAnimation(const std::string& name, const sf::Texture& texture, int frames, int duration) const {
	return std::make_unique<Animation>(name, texture, frames, duration);
}

sf::Font Assets::loadFont(const std::string& path) const {
	sf::Font font;
	if (!font.loadFromFile(path)) {
		std::cerr << "Assets::loadFont: Cannot open file with path \"" << path << "\"\n";
	}
	return font;
}

const Animation& Assets::getAnimation(const std::string& name) const noexcept {
	const auto it = m_animationMap.find(name);
	INPUT_VALIDITY(it != m_animationMap.end());
	return *it->second;
}

const sf::Font& Assets::getFont(const std::string& name) const noexcept {
	const auto it = m_fontMap.find(name);
	INPUT_VALIDITY(it != m_fontMap.end());
	return it->second;
}

