#include "EntityMemoryPool.h"
#include "Entity.h"


EntityMemoryPool::EntityMemoryPool(size_t poolSize)
	: m_tags(poolSize)
	, m_alive(poolSize)
	, m_poolSize{ poolSize } {

	initTuple();
}

Entity EntityMemoryPool::addEntity(const std::string& tag) {
	if (canPushBack()) {
		m_alive[m_nextEntityId] = true;
		m_tags[m_nextEntityId] = tag;
		return Entity(m_nextEntityId++);
	}
	else {
		size_t index = findFreeIndex();
		m_alive[index] = true;
		resetNextEntityId();
		return Entity(index);
	}
}

void EntityMemoryPool::resetNextEntityId() {
	if (m_nextEntityId == 0) {
		return;
	}

	for (size_t i = m_nextEntityId; i-- != 0;) {
		if (m_alive[i]) {
			m_nextEntityId = i + 1;
			break;
		}
	}
}

size_t EntityMemoryPool::findFreeIndex() const {
	for (size_t i = 0; i < m_alive.size(); i++) {
		if (!m_alive[i]) {
			return i;
		}
	}

	std::cerr << "EntityMemoryPool::findFreeIndex: Pool is full\n";
	return std::numeric_limits<size_t>::max();
}

EntityMemoryPool& EntityMemoryPool::Instance() {
	static EntityMemoryPool pool(ENTITY_MEMORY_POOL_MAX_SIZE);
	return pool;
}

const std::string& EntityMemoryPool::getTag(size_t entityId) const {
	return m_tags[entityId];
}

bool EntityMemoryPool::isAlive(size_t entityId) const {
	return m_alive[entityId];
}

void EntityMemoryPool::destroy(size_t entityId) {
	m_alive[entityId] = false;
}

bool EntityMemoryPool::canPushBack() const {
	return m_nextEntityId < m_poolSize;
}