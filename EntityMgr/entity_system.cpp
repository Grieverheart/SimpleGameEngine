#include "entity.h"
#include <iostream>
namespace SES{

	EntitySystem::EntitySystem(void)
	{}
	
	EntitySystem::EntitySystem(const Aspect& aspect):
		aspect_(aspect)
	{}
	
	void EntitySystem::setAspect(const Aspect& aspect){
		aspect_ = aspect;
	}
	
	void EntitySystem::removeEntity(Entity& entity){
		for(auto ent = active_entities_.begin(); ent != active_entities_.end(); ent++){
			if((*ent)->getInstanceID() == entity.getInstanceID()){
				active_entities_.erase(ent);
				break;
			}
		}
		entity.removeSystemBit(system_bit_);
		removed(entity);
	}
	
	void EntitySystem::entityChanged(Entity& entity){
		bool isInterested = true;
		bool isContained = ((entity.getSystemBits() & system_bit_) == system_bit_);
		std::bitset<32> entity_bits = entity.getComponentBits();
		/* Check if entity has all required components in the allSet */
		if(aspect_.getAllSet().any()) isInterested = ((entity_bits & aspect_.getAllSet()) == aspect_.getAllSet());
		/* Check if entity has any of the components in the exclusionSet */
		if(aspect_.getExclusionSet().any() && isInterested) isInterested = (entity_bits & aspect_.getExclusionSet()).none();
		/* Check if entity has any of the components in the oneSet */
		if(aspect_.getOneSet().any()) isInterested = (entity_bits & aspect_.getOneSet()).any();
		
		/* If the system is interested in the entity and it did not contain it, add it */
		if(isInterested && !isContained){
			entity.addSystemBit(system_bit_);
			active_entities_.push_back(&entity);
			added(entity);
		}
		/* If the entity is contained in the system and the system is no longer interested, remove it */
		else if(!isInterested && isContained){
			removeEntity(entity);
			removed(entity);
		}
	}
	
	void EntitySystem::setSystemBit(const std::bitset<32>& sys_bit){
		system_bit_ = sys_bit;
	}
}
