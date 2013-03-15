#ifndef __COMP_TYPE_MGR_H
#define __COMP_TYPE_MGR_H

#define INVALID_COMPONENT_TYPE -1

#include "base_component.h"
#include <string>
#include <vector>
#include <typeinfo>
#include <bitset>

namespace SES{
	class ComponentTypeManager{
	public:
		/* Returns a unique component type id based on the ordered encountered by this function */
		template <class T>
		static int getComponentID(void){
			static int id = count_++;
			if(count_ > nComponentTypes_){
				if(!std::is_base_of<BaseComponent, T>::value){
					count_--;
					return INVALID_COMPONENT_TYPE;
				}
				nComponentTypes_ = count_;
			}
			return id;
		}
		
		/* The following two functions are used to build a bitset of N component types using variadic templates */
		template <class T>
		static std::bitset<32> getComponentsBitset(void){
			std::bitset<32> this_bits;
			int comp_type = getComponentID<T>();
			if(comp_type != INVALID_COMPONENT_TYPE) this_bits[comp_type] = 1;
			return this_bits;
		}
		
		template <class T, class U, class... V>
		static std::bitset<32> getComponentsBitset(void){
			std::bitset<32> this_bits;
			int comp_type = getComponentID<T>();
			if(comp_type != INVALID_COMPONENT_TYPE) this_bits[comp_type] = 1;
			return this_bits | getComponentsBitset<U, V...>();
		}
		
		
		/* Gets the name of a component type using typid(T).name() */
		template <class T>
		static std::string getComponentName(void){
			int comp_type = getComponentID<T>();
			if(comp_type == INVALID_COMPONENT_TYPE) return "INVALID_COMPONENT_TYPE";
			if(component_names_.size() < (unsigned int)(comp_type + 1)){
				component_names_.resize(comp_type + 1);
			}
			std::string name = component_names_[comp_type];
			/* If the component name is asked for the first time, we need to fill the entry */
			if(name.empty()){
				name = typeid(T).name();
				component_names_[comp_type] = name;
			}
			return name;
		}
		
	private:
		static int count_;
		static int nComponentTypes_;
		static std::vector< std::string > component_names_;
	};

}
#endif