#include "entity_manager.h"
#include "entity.h"
#include "id_manager.h"

namespace SES{

	EntityManager::EntityManager(void):
		nEntities_(0),
		nTypes_(0)
	{
		Entity::owner_ = this;
		id_manager_ = new IDManager();
	}

	Entity& EntityManager::createEntity(void){
		uint id = id_manager_->drawID();
		entities_.push_back(new Entity(id));
		nEntities_++;
		return *entities_.back();
	}

	Entity& EntityManager::getEntity(uint entity_id){
		return *entities_[entity_id];
	}

	void EntityManager::destroyEntity(uint entity_id){
		removed_.insert(entities_[entity_id]);
	}

	void EntityManager::deleteEntity(uint entity_id){
		Entity* entity = entities_[entity_id];
		if(entity != nullptr){
			//First find and delete all components this entity pocesses by looping through its set bits
			std::bitset<32> ebits = entity->getComponentBits();
			for(int i = 0; i < 32; i++){
				if(ebits[i]) delete components_[i][entity_id];
			}
			delete entity;
			entities_[entity_id] = nullptr;
			nEntities_--;
		}
	}

	EntityManager::~EntityManager(void){
		for(auto entity: entities_){
			if(entity != nullptr){
				//First find and delete all components this entity pocesses by looping through its set bits
				uint eid = entity->getInstanceID();
				std::bitset<32> ebits = entity->getComponentBits();
				for(int i = 0; i < 32; i++){
					if(ebits[i]) delete components_[i][eid];
				}
				delete entity;
			}
		}
		delete id_manager_;
	}
	
	bool EntityManager::attachComponent(uint entity_id, BaseComponent *component, uint comp_id){
		int comp_type = comp_id;
		//Check if the type T is encountered for the first time
		if(comp_type == INVALID_COMPONENT_TYPE) return false;
		if(comp_type + 1 > nTypes_){
			nTypes_ = comp_type + 1;
			components_.resize(nTypes_);
		}
		//Check if the vector for this component type has space for this entity, if not resize to accomondate
		if(entity_id + 1 > components_[comp_type].size()) components_[comp_type].resize(entity_id + 1, nullptr);
		components_[comp_type][entity_id] = component;
		setComponentBit(entity_id, comp_type, true);
		changed_.insert(entities_[entity_id]);
		return true;
	}

	void EntityManager::registerSystem(EntitySystem* ent_system){
		/* Check if either the system already exists (pointer equality) or the system type already exists */
		for(auto system: systems_){
			if(system == ent_system) return;
			if(typeid(*system) == typeid(*ent_system)) return;
		}
		ent_system->setSystemBit(std::bitset<32>(1 << systems_.size()));
		systems_.push_back(ent_system);
	}

	void EntityManager::processQueue(void){
		/* Notify registered systems about changed and/or added entities */
		for(auto changed_entity: changed_){
			for(auto system: systems_) system->entityChanged(*changed_entity);
		}
		/* Notify systems about removed entities. Subsequently delete them */
		for(auto removed_entity: removed_){
			for(auto system: systems_) system->removeEntity(*removed_entity);
			deleteEntity(removed_entity->getInstanceID());
		}
		/* Clear the queues after being done */
		changed_.clear();
		removed_.clear();
	}

	void EntityManager::setComponentBit(uint entity_id, uint component_id, bool val)const{
		entities_[entity_id]->component_bits_[component_id] = val;
	}

}
