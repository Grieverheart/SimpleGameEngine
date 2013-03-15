#include "include/movement_system.h"

template<typename T> 
static inline T sqr(T a){
	return a * a;
}

MovementSystem::MovementSystem(const SES::Aspect& aspect):
	SES::EntitySystem(aspect),
	gravity_(9.89f)
{}

MovementSystem::~MovementSystem(void){
}

void MovementSystem::update(float dt){
	for(auto entity: active_entities_){
		if(move_comps_[*entity]->mv_direction != glm::ivec3(0)){
			glm::vec3 mv_dir(0.0);
			mv_dir += (float)move_comps_[*entity]->mv_direction.z * transform_comps_[*entity]->forwardVec();
			mv_dir += (float)move_comps_[*entity]->mv_direction.x * transform_comps_[*entity]->rightVec();
			mv_dir = glm::normalize(mv_dir);
			
			glm::vec3& velocity = move_comps_[*entity]->velocity_;
			transform_comps_[*entity]->pos_ += dt * glm::vec3(velocity.x, 0.0, velocity.z) + 0.5f * move_comps_[*entity]->acceleration_ * sqr(dt) * mv_dir;
			
			velocity += dt * move_comps_[*entity]->acceleration_ * mv_dir;
			if(sqr(velocity.x) +  sqr(velocity.z) > sqr(move_comps_[*entity]->max_velocity_)){
				velocity.x = move_comps_[*entity]->max_velocity_ * mv_dir.x;
				velocity.z = move_comps_[*entity]->max_velocity_ * mv_dir.z;
			}
			
		}
		else if((sqr(move_comps_[*entity]->velocity_.x) +  sqr(move_comps_[*entity]->velocity_.z) > 0.0)){
			glm::vec3& velocity = move_comps_[*entity]->velocity_;
			glm::vec3 mv_dir = glm::normalize(glm::vec3(velocity.x, 0.0, velocity.z));
			transform_comps_[*entity]->pos_ += dt * glm::vec3(velocity.x, 0.0, velocity.z) + 0.5f * move_comps_[*entity]->acceleration_ * sqr(dt) * mv_dir;
			if(!move_comps_[*entity]->isInAir_){
				velocity -= dt * move_comps_[*entity]->acceleration_ * mv_dir;
				if(sqr(velocity.x) +  sqr(velocity.z) < 0.01f){
					velocity.x = velocity.z = 0.0f;
				}
			}
		}
		
		if(move_comps_[*entity]->isInAir_){
			transform_comps_[*entity]->pos_.y += dt * move_comps_[*entity]->velocity_.y - 0.5f * gravity_ * sqr(dt);
			if(transform_comps_[*entity]->pos_.y < move_comps_[*entity]->ground_distance_){
				transform_comps_[*entity]->pos_.y = move_comps_[*entity]->ground_distance_;
				move_comps_[*entity]->isInAir_ = false;
			}
			move_comps_[*entity]->velocity_.y -= dt * gravity_;
		}
			
		float dtheta = move_comps_[*entity]->ang_velocity_ * dt;
		transform_comps_[*entity]->rotate(dtheta, glm::vec3(0.0, 1.0, 0.0));
	}
}

void MovementSystem::hEntityMovementStart(const Event& event){
	SES::Entity* entity = static_cast<const EntityEvent&>(event).getEntity();
	const EventType& type = event.getEventType();
	
	if(type == ENTITY_MOVE_FW_START){
		move_comps_[*entity]->mv_direction += glm::ivec3(0, 0, 1);
	}
	else if(type == ENTITY_MOVE_BW_START){
		move_comps_[*entity]->mv_direction += glm::ivec3(0, 0, -1);
	}
	else if(type == ENTITY_STRAFE_LEFT_START){
		move_comps_[*entity]->mv_direction += glm::ivec3(-1, 0, 0);
	}
	else if(type == ENTITY_STRAFE_RIGHT_START){
		move_comps_[*entity]->mv_direction += glm::ivec3(1, 0, 0);
	}
	else if(type == ENTITY_TURN_LEFT_START){
		move_comps_[*entity]->ang_velocity_ = 45.0f;
	}
	else if(type == ENTITY_TURN_RIGHT_START){
		move_comps_[*entity]->ang_velocity_ = -45.0f;
	}
	else if(type == ENTITY_JUMP_START){
		move_comps_[*entity]->isInAir_  = true;
		move_comps_[*entity]->velocity_.y  = 5.5f;
	}
}

void MovementSystem::hEntityMovementEnd(const Event& event){
	SES::Entity* entity = static_cast<const EntityEvent&>(event).getEntity();
	const EventType& type = event.getEventType();
	
	if(type == ENTITY_TURN_LEFT_END || type == ENTITY_TURN_RIGHT_END){
		move_comps_[*entity]->ang_velocity_ = 0.0f;
	}
	else if(type == ENTITY_MOVE_FW_END){
		move_comps_[*entity]->mv_direction -= glm::ivec3(0, 0, 1);
	}
	else if(type == ENTITY_MOVE_BW_END){
		move_comps_[*entity]->mv_direction -= glm::ivec3(0, 0, -1);
	}
	else if(type == ENTITY_STRAFE_LEFT_END){
		move_comps_[*entity]->mv_direction -= glm::ivec3(-1, 0, 0);
	}
	else if(type == ENTITY_STRAFE_RIGHT_END){
		move_comps_[*entity]->mv_direction -= glm::ivec3(1, 0, 0);
	}
	
}

void MovementSystem::hEntityRotateBy(const Event& event){
	SES::Entity* entity = static_cast<const EntityXYEvent&>(event).getEntity();
	float dx = static_cast<const EntityXYEvent&>(event).dx_;
	transform_comps_[*entity]->rotate(-180.0f * dx, glm::vec3(0.0, 1.0, 0.0));
}

