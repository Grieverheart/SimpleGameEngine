#ifndef _COMPONENTS_H
#define _COMPONENTS_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "EntityMgr/base_component.h"
#include "mesh.h"
#include "texture.h"

struct RenderComponent: public BaseComponent{
	RenderComponent(GLuint tex_id, const Mesh* mesh, bool isShadow = true):
		mesh_(mesh),
		tex_id_(tex_id),
		isShadowCaster_(isShadow)
	{}
	const Mesh* mesh_;
	GLuint tex_id_;
	bool isShadowCaster_;
};

struct TransformComponent: public BaseComponent{
	explicit TransformComponent(glm::vec3 pos = glm::vec3(0.0), float angle = 0.0, glm::vec3 axis = glm::vec3(1.0, 0.0, 0.0), glm::vec3 scale = glm::vec3(1.0)):
		pos_(pos),
		rotation_(glm::angleAxis(angle, axis)),
		scale_(scale),
		changed_(true)
	{
		modelMatrix();
	}
	
	void translate(glm::vec3 trans){
		changed_ = true;
		pos_ += trans;
	}
	
	void translate(float x, float y, float z){
		changed_ = true;
		pos_ += glm::vec3(x, y, z);
	}
	
	void rotate(float angle, glm::vec3 axis){
		changed_ = true;
		rotation_ = glm::rotate(rotation_, angle, axis);
	}
	
	void scale(glm::vec3 scale){
		changed_ = true;
		scale_ *= scale;
	}
	
	void scale(float x, float y, float z){
		changed_ = true;
		scale_ *= glm::vec3(x, y, z);
	}
	
	glm::vec3 rightVec(void){
		changed_ = true;
		return rotation_ * glm::vec3(1.0, 0.0, 0.0);
	}
	
	glm::vec3 upVec(void){
		changed_ = true;
		return rotation_ * glm::vec3(0.0, 1.0, 0.0);
	}
	
	glm::vec3 forwardVec(void){
		changed_ = true;
		return rotation_ * glm::vec3(0.0, 0.0, -1.0);
	}
	
	const glm::vec3& getPosition(void)const{
		return pos_;
	}
	
	const glm::fquat& getRotation(void)const{
		return rotation_;
	}
	
	const glm::mat4& modelMatrix(void){
		if(changed_){
			model_matrix_ = glm::translate(glm::mat4(1.0), pos_) * glm::mat4_cast(rotation_) * glm::scale(glm::mat4(1.0), scale_);
			changed_ = false;
		}
		return model_matrix_;
	}
	
	glm::vec3 pos_;
	glm::fquat rotation_;
	glm::vec3 scale_;
	glm::mat4 model_matrix_;
	bool changed_;
	
};

struct MovementComponent: public BaseComponent{
	MovementComponent(void):
		isInAir_(false),
		velocity_(0.0),
		max_velocity_(5.5f),
		acceleration_(25.0f),
		ang_velocity_(0.0f),
		ground_distance_(0.0f), //Temporary
		mv_direction(0)
	{}
	bool isInAir_;
	glm::vec3 velocity_;
	float max_velocity_;
	float acceleration_;
	float ang_velocity_;
	float ground_distance_;
	glm::ivec3 mv_direction;
};

#endif