#ifndef _ENTITY_EVENT_H
#define _ENTITY_EVENT_H

#include <vector>
#include "EventMgr/event.h"

namespace SES{
	class Entity;
}

class EntityEvent: public Event{
public:
	explicit EntityEvent(const EventType& type, SES::Entity* entity):
		type_(type),
		entity_(entity)
	{}
	virtual ~EntityEvent(void){}
	virtual const EventType& getEventType(void)const{
		return type_;
	}
	virtual SES::Entity* getEntity(void)const{
		return entity_;
	}
private:
	const EventType type_;
	SES::Entity* entity_;
};

class EntityXYEvent: public EntityEvent{
public:
	explicit EntityXYEvent(const EventType& type, SES::Entity* entity, float dx, float dy):
		EntityEvent(type, entity),
		dx_(dx),
		dy_(dy)
	{}
	~EntityXYEvent(void){}
	
	float dx_, dy_;
};

extern const EventType ENTITY_MOVE_FW_START;
extern const EventType ENTITY_MOVE_FW_END;
extern const EventType ENTITY_MOVE_BW_START;
extern const EventType ENTITY_MOVE_BW_END;
extern const EventType ENTITY_STRAFE_LEFT_START;
extern const EventType ENTITY_STRAFE_LEFT_END;
extern const EventType ENTITY_STRAFE_RIGHT_START;
extern const EventType ENTITY_STRAFE_RIGHT_END;
extern const EventType ENTITY_TURN_LEFT_START;
extern const EventType ENTITY_TURN_LEFT_END;
extern const EventType ENTITY_TURN_RIGHT_START;
extern const EventType ENTITY_TURN_RIGHT_END;
extern const EventType ENTITY_JUMP_START;
extern const EventType ENTITY_JUMP_END;
extern const EventType ENTITY_ROTATE_BY;

extern const std::vector<EventType> ENTITY_MOVE_START;
extern const std::vector<EventType> ENTITY_MOVE_END;



#endif