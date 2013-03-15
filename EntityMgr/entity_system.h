#ifndef __ENTITY_SYSTEM_H
#define __ENTITY_SYSTEM_H

#include <bitset>
#include "aspect.h"

namespace SES{
	class Entity;
	
	class EntitySystem{
	friend class EntityManager;
	public:
		EntitySystem(void);
		
		EntitySystem(const Aspect& aspect);
		
		virtual ~EntitySystem(void){}
		
		void setAspect(const Aspect& aspect);
		
	protected:
		virtual void removed(Entity &e){}
		virtual void added(Entity &e){}
	
		std::vector<Entity*> active_entities_;
		
	private:
		void removeEntity(Entity& entity);
		
		void entityChanged(Entity& entity);
		
		void setSystemBit(const std::bitset<32>& sys_bit);
		
		Aspect aspect_;
		std::bitset<32> system_bit_;
	};
}

#endif