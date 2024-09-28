#include "Action.h"


Action::Action() {}

Action::Action(ActionType type, ActionStage stage)
	: m_type{ type }
	, m_stage{ stage } {}

Action::Action(ActionType type, ActionStage stage, sf::Vector2f pos)
	: m_type{ type }
	, m_stage{ stage }
	, m_pos{ pos } {}

sf::Vector2f Action::getPos() const noexcept {
	return m_pos;
}

ActionType Action::getType() const noexcept {
	return m_type;
}

ActionStage Action::getStage() const noexcept {
	return m_stage;
}
