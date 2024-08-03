#include "Action.h"


Action::Action() {}

Action::Action(const std::string& name, bool isStart)
	: m_name{ name }
	, m_isStart{ isStart } {}

Action::Action(const std::string& name, bool isStart, sf::Vector2f pos)
	: m_name{ name }
	, m_isStart{ isStart }
	, m_pos{ pos } {}

const std::string& Action::name() const {
	return m_name;
}

bool Action::isStart() const {
	return m_isStart;
}

sf::Vector2f Action::pos() const {
	return m_pos;
}
