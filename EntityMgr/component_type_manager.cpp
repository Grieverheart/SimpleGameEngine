#include "component_type_manager.h"

namespace SES{
	int ComponentTypeManager::count_ = 0;
	int ComponentTypeManager::nComponentTypes_ = 0;
	std::vector< std::string > ComponentTypeManager::component_names_;
}