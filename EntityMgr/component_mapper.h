#ifndef __COMP_MAPPER_H
#define __COMP_MAPPER_H

namespace SES{

	template<typename T>
	struct ComponentMapper{
		T* operator[](const Entity& entity)const{
			return entity.getComponent<T>();
		}
	};

}
#endif