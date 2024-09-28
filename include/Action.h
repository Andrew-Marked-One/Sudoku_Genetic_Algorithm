#pragma once
#include <SFML/Graphics.hpp>


enum class ActionType : uint8_t {
	None = 0,
	LeftClick,
	RightClick,
	MiddleClick,
	MouseMove,
	Resized,
	Quit
};


enum class ActionStage : uint8_t {
	Start = 0,
	End   = 1
};


class Action {
public:
	Action();
	explicit Action(ActionType type, ActionStage stage);
	explicit Action(ActionType type, ActionStage stage, sf::Vector2f pos);

	[[nodiscard]] sf::Vector2f getPos() const noexcept;
	[[nodiscard]] ActionType getType() const noexcept;
	[[nodiscard]] ActionStage getStage() const noexcept;

private:
	sf::Vector2f m_pos = { 0, 0 };
	ActionType m_type = ActionType::None;
	ActionStage m_stage = ActionStage::Start;
};
	
