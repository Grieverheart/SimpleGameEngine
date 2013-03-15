#include "entity.h"

namespace SES{

	Entity::~Entity(void){
	}

	unsigned int Entity::getInstanceID(void)const{
		return id_;
	}

	void Entity::destroy(void)const{
		owner_->destroyEntity(id_);
	}

	Entity::Entity(const uint id):
		id_(id)
	{}

	const std::bitset<32>& Entity::getComponentBits(void)const{
		return component_bits_;
	}

	const std::bitset<32>& Entity::getSystemBits(void)const{
		return system_bits_;
	}

	void Entity::addSystemBit(const std::bitset<32>& bit){
		system_bits_ |= bit;
	}

	void Entity::removeSystemBit(const std::bitset<32>& bit){
		system_bits_ ^= bit;
	}

	EntityManager *Entity::owner_ = 0;
}
