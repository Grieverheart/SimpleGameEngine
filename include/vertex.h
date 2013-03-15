#ifndef _VERTEX_H
#define _VERTEX_H

#include <glm/glm.hpp>

struct Vertex{
	glm::vec3 coord_;
	glm::vec3 normal_;
	glm::vec2 texCoord_;
	
	Vertex(void){}
	~Vertex(void){}
	Vertex(glm::vec3 coord, glm::vec3 normal, glm::vec2 texCoord = glm::vec2(0.0, 0.0)):
		coord_(coord),
		normal_(normal),
		texCoord_(texCoord)
	{}
};

#endif