#include "Entity.h"

Entity::Entity() {}

Entity::Entity(size_t id)
	: m_id{ id } {}

size_t Entity::getId() const {
	return m_id;
}

const std::string& Entity::getTag() const {
	return EntityMemoryPool::Instance().getTag(m_id);
}

bool Entity::isAlive() const {
	return EntityMemoryPool::Instance().isAlive(m_id);
}

void Entity::destroy() const {
	return EntityMemoryPool::Instance().destroy(m_id);
}