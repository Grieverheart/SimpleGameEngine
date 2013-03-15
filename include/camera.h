#ifndef __CAMERA_H
#define __CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "EntityMgr/entity.h"


class Camera{
public:
	enum eProjection{Orthographic, Perspective};
	
	Camera(float aspect, const glm::vec3& pos = glm::vec3(0.0), float angle = 0.0f, const glm::vec3& axis = glm::vec3(1.0, 0.0, 0.0), eProjection mode = Perspective);
	~Camera(void);
	
	void updateView(void);
	void rotate(float angle, const glm::vec3& axis);
	void translate(const glm::vec3&);
	void setPosition(const glm::vec3&);
	void setRotation(float angle, const glm::vec3& axis);
	
	void followEntity(SES::Entity*);
	void unfollow(void);
	void mimicEntity(bool);
	
	const glm::mat4& getViewMatrix(void)const;
	void setProjectionParams(float aspect, float fov_ortho_h, float zn, float zf);
	void setProjectionMode(eProjection);
	const glm::mat4& getProjectionMatrix(void);
	
private:
	eProjection projection_mode_;
	bool isMimicking_;
	bool isFollowing_;
	bool isProjectionChanged_;
	glm::vec3 pos_;
	glm::fquat rotation_;
	float zn_, zf_;
	float fov_, aspect_, ortho_height_;
	glm::mat4 view_matrix_;
	glm::mat4 projection_matrix_;
	SES::Entity* followee_;
};

#endif