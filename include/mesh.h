#ifndef __MESH_H
#define __MESH_H

#include <vector>
#include <map>
#include <GL/glew.h>
#include "vertex.h"
#include "obj_parser.h"

class Mesh{
public:
	Mesh(std::vector<Vertex> vertices, GLuint vaoID = 0, GLuint vboID = 0);
	~Mesh(void);
	void draw(void)const;
private:
	std::vector<Vertex> vertices_;
	GLuint vaoID_;
	GLuint vboID_;
};


class MeshManager{
public:
	MeshManager(void){};
	~MeshManager(void);
	
	const Mesh* loadMesh(const std::string& filename, const std::string& shading_model);
	// bool freeMesh(const char*);
	
private:
	std::map<std::string, Mesh*> meshes_;
};

#endif