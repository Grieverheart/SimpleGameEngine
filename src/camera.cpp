#include "include/camera.h"
#include "include/components.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

Camera::Camera(float aspect, const glm::vec3& pos, float angle, const glm::vec3& axis, eProjection mode):
	projection_mode_(mode),
	isMimicking_(false),
	isFollowing_(false),
	isProjectionChanged_(false),
	pos_(pos),
	rotation_(glm::angleAxis(angle, axis)),
	zn_(1.0f),
	zf_(100.0f),
	fov_(60.0f),
	aspect_(aspect),
	ortho_height_(10.0f),
	followee_(nullptr)
{
	view_matrix_ = glm::translate(glm::mat4(1.0), pos_) * glm::mat4_cast(rotation_);
	view_matrix_ = glm::inverse(view_matrix_);
	if(mode == Perspective){
		projection_matrix_ = glm::perspective(fov_, aspect_, zn_, zf_);
	}
	else{
		projection_matrix_ = glm::ortho(-aspect_ * ortho_height_ * 0.5f, aspect_ * ortho_height_ * 0.5f, -ortho_height_ * 0.5f, ortho_height_ * 0.5f, zn_, zf_);
	}
}

Camera::~Camera(void){
}

void Camera::updateView(void){
	if(isFollowing_){
		TransformComponent* transformation = followee_->getComponent<TransformComponent>();
		glm::fquat rotation = transformation->getRotation() * rotation_;
		// glm::vec3 pos = pos_ + transformation->getPosition();
		view_matrix_ = glm::translate(glm::mat4(1.0), transformation->getPosition()) * glm::mat4_cast(rotation) * glm::translate(glm::mat4(1.0), pos_);
	}
	else if(isMimicking_){
		glm::vec3 pos = pos_ + followee_->getComponent<TransformComponent>()->getPosition();
		view_matrix_ = glm::translate(glm::mat4(1.0), pos) * glm::mat4_cast(rotation_);
	}
	else{
		view_matrix_ = glm::translate(glm::mat4(1.0), pos_) * glm::mat4_cast(rotation_);
	}
	
	view_matrix_ = glm::inverse(view_matrix_);
}
	
void Camera::rotate(float angle, const glm::vec3& axis){
	rotation_ = glm::rotate(rotation_, angle, axis);
}

void Camera::setRotation(float angle, const glm::vec3& axis){
	rotation_ = glm::rotate(rotation_, angle, axis);
}

void Camera::setPosition(const glm::vec3& pos){
	this->pos_ = pos;
}

void Camera::translate(const glm::vec3& trans){
	pos_ += trans;
}

const glm::mat4& Camera::getViewMatrix(void)const{
	return view_matrix_;
}

void Camera::setProjectionParams(float aspect, float fov_ortho_h, float zn, float zf){
	aspect_ = aspect;
	if(zn > 0.0f) zn_ = zn;
	if(zf > 0.0f) zf_ = zf;
	if(fov_ortho_h > 0.0f){
		if(projection_mode_ == Perspective) fov_ = fov_ortho_h;
		else ortho_height_ = fov_ortho_h;
	}
	isProjectionChanged_ = true;
}

void Camera::setProjectionMode(eProjection mode){
	projection_mode_ = mode;
	isProjectionChanged_ = true;
}

const glm::mat4& Camera::getProjectionMatrix(void){
	if(isProjectionChanged_){
		if(projection_mode_ == Perspective){
			projection_matrix_ = glm::perspective(fov_, aspect_, zn_, zf_);
		}
		else{
			projection_matrix_ = glm::ortho(-aspect_ * ortho_height_ * 0.5f, aspect_ * ortho_height_ * 0.5f, -ortho_height_ * 0.5f, ortho_height_ * 0.5f, zn_, zf_);
		}
		isProjectionChanged_ = false;
	}
	return projection_matrix_;
}
	
void Camera::followEntity(SES::Entity* entity){
	isFollowing_ = true;
	followee_ = entity;
}

void Camera::unfollow(void){
	isFollowing_ = false;
}

void Camera::mimicEntity(bool val){
	isMimicking_ = val;
}

