#ifndef __ENTITY_MANAGER_H
#define __ENTITY_MANAGER_H

#include <vector>
#include <string>
#include <unordered_set>
#include <bitset>
#include "base_component.h"
#include "component_type_manager.h"
#include "entity_system.h"

namespace SES{
	class Entity;
	class IDManager;

	typedef unsigned int uint;

	class EntityManager{
		friend class Serializer;
	public:
		EntityManager(void);
		~EntityManager(void);
		
		Entity& createEntity(void);
		void destroyEntity(uint entity_id);
		
		Entity& getEntity(uint entity_id);
		
		template<class T>
		bool attachComponent(uint entity_id, BaseComponent *component);
		
		template<class T>
		bool detachComponent(uint entity_id);
		
		template<class T>
		T* getComponent(uint entity_id);
		
		// template<class T>
		// std::vector<T*> getComponentByType(void);
		
		void registerSystem(EntitySystem*);
		
		void processQueue(void);
		
	private:
		void deleteEntity(uint entity_id);
		void setComponentBit(uint entity_id, uint component_id, bool)const;
		
		uint nEntities_;
		int nTypes_;
		std::vector<Entity*> entities_;
		std::vector< std::vector<BaseComponent*> > components_;
		std::vector<EntitySystem*> systems_;
		IDManager* id_manager_;
		
		std::unordered_set<Entity*> changed_;
		std::unordered_set<Entity*> removed_;
	};

	#include "entity_manager.himpl"
}
#endif