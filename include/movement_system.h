#ifndef _MOVEMENT_SYSTEM_H
#define _MOVEMENT_SYSTEM_H

#include <glm/glm.hpp>
#include "components.h"
#include "EntityMgr/entity.h"
#include "EntityMgr/component_mapper.h"
#include "entity_event.h"

class MovementSystem: public SES::EntitySystem{
public:
	MovementSystem(const SES::Aspect& aspect);
	~MovementSystem(void);
	
	void update(float dt);
	
	void hEntityMovementStart(const Event&);
	void hEntityMovementEnd(const Event&);
	void hEntityRotateBy(const Event&);
	
private:
	SES::ComponentMapper<MovementComponent> move_comps_;
	SES::ComponentMapper<TransformComponent> transform_comps_;
	
	float gravity_;
};


#endif