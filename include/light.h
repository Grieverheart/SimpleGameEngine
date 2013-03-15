#ifndef __LIGHT_H
#define __LIGHT_H

#include <GL/glew.h>
#include <glm/glm.hpp>

class CLight{
public:
	CLight(void);
	CLight(glm::vec3 position, glm::vec3 direction);
	~CLight(void);
	
	bool Init(GLuint shade_id);
	void uploadDirection(glm::mat4 ViewMatrix)const;
	void uploadMVP(glm::mat4 lightMVP)const;
	
	glm::vec3 getPosition(void)const;
	glm::vec3 getDirection(void)const;
	
private:
	glm::vec3 m_position;
	glm::vec3 m_direction;
	int m_DirectionLocation;
	int m_MVPLocation;
};

#endif