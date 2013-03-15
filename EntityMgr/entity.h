#ifndef __ENTITY_H
#define __ENTITY_H

#include "entity_manager.h"

namespace SES{
	typedef unsigned int uint;
	
	class Entity{
	friend class EntityManager;
	public:
		//No copy and assignment operators
		Entity(const Entity&);
		Entity& operator =(const Entity&);
		
		~Entity(void);
		
		template<class T>
		void attachComponent(T *component);
		
		template<class T>
		void detachComponent(void);
		
		template<class T>
		T* getComponent(void)const;
		
		unsigned int getInstanceID(void)const;
		
		void destroy(void)const;
		
		const std::bitset<32>& getComponentBits(void)const;
		
		const std::bitset<32>& getSystemBits(void)const;
		
		void addSystemBit(const std::bitset<32>&);
		
		void removeSystemBit(const std::bitset<32>&);
		
	private:
		Entity(const uint id);
		
		const uint id_;
		static EntityManager *owner_;
		std::bitset<32> component_bits_;
		std::bitset<32> system_bits_;
	};
	#include "entity.himpl"
}

#endif