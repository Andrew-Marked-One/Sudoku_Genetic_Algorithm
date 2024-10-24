#include "Entity.h"

Entity::Entity() noexcept {}

Entity::Entity(EntityId_t entityId) noexcept
	: m_id{ entityId } {

	INPUT_VALIDITY(entityId >= 0);
}

EntityId_t Entity::getId() const noexcept {
	return m_id;
}

const std::string& Entity::getTag() const noexcept {
	return EntityMemoryPool::Instance().getTag(m_id);
}

bool Entity::isAlive() const noexcept {
	return EntityMemoryPool::Instance().isAlive(m_id);
}

void Entity::destroy() const noexcept {
	EntityMemoryPool::Instance().destroy(m_id);
}