#ifndef __ASPECT_H
#define __ASPECT_H

#include <bitset>
#include "component_type_manager.h"

namespace SES{
	class Aspect{
	public:
		Aspect(void){}
		
		template<class... T>
		Aspect& all(void){
			allSet_ = ComponentTypeManager::getComponentsBitset<T...>();
			return *this;
		}
		
		template<class... T>
		Aspect& one(void){
			oneSet_ = ComponentTypeManager::getComponentsBitset<T...>();
			return *this;
		}
		
		template<class... T>
		Aspect& exclude(void){
			noSet_ = ComponentTypeManager::getComponentsBitset<T...>();
			return *this;
		}
		
		const std::bitset<32>& getAllSet(void){
			return allSet_;
		}
		
		const std::bitset<32>& getOneSet(void){
			return oneSet_;
		}
		
		const std::bitset<32>& getExclusionSet(void){
			return noSet_;
		}
		
	private:
		std::bitset<32> allSet_;
		std::bitset<32> oneSet_;
		std::bitset<32> noSet_;
	};
}
#endif