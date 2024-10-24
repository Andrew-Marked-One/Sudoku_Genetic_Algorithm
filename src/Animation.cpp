#include "Animation.h"


Animation::Animation() {}

Animation::Animation(const std::string& name, const sf::Texture& texture, int frameCount, int speed)
	: m_name{ name }
	, m_sprite{ texture }
	, m_frameCount{ frameCount }
	, m_speed{ speed } {

	INPUT_VALIDITY(!name.empty());
	INPUT_VALIDITY(texture.getSize().x != 0 && texture.getSize().y != 0);
	INPUT_VALIDITY(frameCount > 0);
	INPUT_VALIDITY(speed >= 0);

	m_tSize = texture.getSize();
	m_sprite.setOrigin(m_tSize.x / 2.F, m_tSize.y / 2.F);
	m_sprite.setTextureRect(sf::IntRect(m_currentFrame * m_tSize.x, 0, m_tSize.x, m_tSize.y));
}

void Animation::update() {
	if (m_speed > 0) {
		m_sprite.setTextureRect(sf::IntRect((m_currentFrame / m_speed) % m_frameCount * m_tSize.x, 0, m_tSize.x, m_tSize.y));
		m_currentFrame++;
	}
}

const std::string& Animation::getName() const noexcept {
	return m_name;
}

const sf::Sprite& Animation::getSprite() const noexcept {
	return m_sprite;
}

sf::Sprite& Animation::getSprite() noexcept {
	return m_sprite;
}

bool Animation::hasEnded() const noexcept {
	return m_currentFrame / m_speed > m_frameCount;
}



