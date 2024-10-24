#include "EntityMemoryPool.h"
#include "Entity.h"


EntityMemoryPool::EntityMemoryPool(EntityId_t poolSize)
	: m_tags(poolSize)
	, m_alive(poolSize)
	, m_poolSize{ poolSize } {

	INPUT_VALIDITY(poolSize >= 0);

	initTuple();
}

Entity EntityMemoryPool::addEntity(const std::string& tag) {
	INPUT_VALIDITY(!tag.empty());

	EntityId_t index = isPoolFull() ? getFreeIndexFromFront() : m_nextEntityId++;

	OUTPUT_VALIDITY(index < m_poolSize);

	m_alive[index] = true;
	m_tags[index] = tag;
	return Entity(index);
}

EntityId_t EntityMemoryPool::getFreeIndexFromBack() const noexcept {
	for (EntityId_t i = m_poolSize; i-- > 0; ) {
		if (m_alive[i]) {
			return i + 1;
		}
	}
	return 0;
}

EntityId_t EntityMemoryPool::getFreeIndexFromFront() const noexcept {
	for (EntityId_t i = 0; i < m_poolSize; i++) {
		if (!m_alive[i]) {
			return i;
		}
	}
	return m_poolSize;
}

EntityMemoryPool& EntityMemoryPool::Instance() {
	static EntityMemoryPool pool(ENTITY_MEMORY_POOL_MAX_SIZE);
	return pool;
}

const std::string& EntityMemoryPool::getTag(EntityId_t entityId) const noexcept {
	INPUT_VALIDITY(entityId < m_poolSize);

	return m_tags[entityId];
}

bool EntityMemoryPool::isAlive(EntityId_t entityId) const noexcept {
	INPUT_VALIDITY(entityId < m_poolSize);

	return m_alive[entityId];
}

void EntityMemoryPool::destroy(EntityId_t entityId) noexcept {
	INPUT_VALIDITY(entityId < m_poolSize);

	m_alive[entityId] = false;
}

bool EntityMemoryPool::isPoolFull() const noexcept {
	return m_nextEntityId >= m_poolSize;
}