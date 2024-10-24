#include "Assets.h"


void Assets::loadAssets(const std::string& assetsPath) {
	INPUT_VALIDITY(!assetsPath.empty());

	std::ifstream file(assetsPath);
	if (!file.is_open()) {
		std::cerr << "Assets::loadAssets: Cannot open file with path \"" << assetsPath << "\"\n";
	}
	std::string line;
	while (std::getline(file, line)) {
		std::stringstream iss(line);

		std::string type;
		std::string name;
		std::string path;

		iss >> type;
		if (type == "Texture") {
			iss >> name >> path;
			m_textureMap[name] = loadTextures(path);
		}
		else if (type == "Animation") {
			std::string textName;
			int frames = 0;
			int speed = 0;

			iss >> name >> textName >> frames >> speed;
			m_animationMap[name] = loadAnimation(name, m_textureMap[textName], frames, speed);
		}
		else if (type == "Font") {
			iss >> name >> path;
			m_fontMap[name] = loadFont(path);
		}
	}
}

sf::Texture Assets::loadTextures(const std::string& path) {
	INPUT_VALIDITY(!path.empty());

	sf::Texture texture;
	if (!texture.loadFromFile(path)) {
		std::cerr << "Assets::loadTextures: Cannot open file with path \"" << path << "\"\n";
	}
	return texture;
}

std::unique_ptr<Animation> Assets::loadAnimation(const std::string& name, const sf::Texture& texture, int frames, int speed) {
	INPUT_VALIDITY(!name.empty());
	INPUT_VALIDITY(texture.getSize().x != 0 && texture.getSize().y != 0);
	INPUT_VALIDITY(frames > 0);
	INPUT_VALIDITY(speed >= 0);

	return std::make_unique<Animation>(name, texture, frames, speed);
}

sf::Font Assets::loadFont(const std::string& path) {
	INPUT_VALIDITY(!path.empty());

	sf::Font font;
	if (!font.loadFromFile(path)) {
		std::cerr << "Assets::loadFont: Cannot open file with path \"" << path << "\"\n";
	}
	return font;
}

const Animation& Assets::getAnimation(const std::string& name) const noexcept {
	INPUT_VALIDITY(!name.empty());

	const auto animationIt = m_animationMap.find(name);

	INPUT_VALIDITY(animationIt != m_animationMap.end());

	return *animationIt->second;
}

const sf::Font& Assets::getFont(const std::string& name) const noexcept {
	INPUT_VALIDITY(!name.empty());

	const auto fontIt = m_fontMap.find(name);

	INPUT_VALIDITY(fontIt != m_fontMap.end());

	return fontIt->second;
}

