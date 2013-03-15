#include "include/mesh.h"

#define MESH_ROOT_DIR "resources/obj/"

Mesh::Mesh(std::vector<Vertex> vertices, GLuint vaoID, GLuint vboID){
	vertices_ = vertices;
	vaoID_ = vaoID;
	vboID_ = vboID;
}

Mesh::~Mesh(void){
	if(vaoID_) glDeleteBuffers(1, &vaoID_);
	if(vboID_) glDeleteBuffers(1, &vboID_);
}

void Mesh::draw(void)const{
	glBindVertexArray(vaoID_); 
	glDrawArrays(GL_TRIANGLES, 0, vertices_.size());
	glBindVertexArray(0);
}

MeshManager::~MeshManager(void){
	for(auto pair: meshes_){
		delete pair.second;
	}
}

const Mesh* MeshManager::loadMesh(const std::string& filename, const std::string& shading_model){
	
	auto mesh_itr = meshes_.find(filename);
	if(mesh_itr != meshes_.end()) return mesh_itr->second;
	
	std::vector<Vertex> vertices = parseObj((MESH_ROOT_DIR + filename).c_str(), shading_model);
	
	GLuint vaoID, vboID;
	
	glGenVertexArrays(1,&vaoID);
	glBindVertexArray(vaoID);
	
	glGenBuffers(1,&vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	
	glEnableVertexAttribArray((GLuint)0);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray((GLuint)1);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)sizeof(glm::vec3));
	glEnableVertexAttribArray((GLuint)2);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(2 * sizeof(glm::vec3)));
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	Mesh* mesh = new Mesh(vertices, vaoID, vboID);
	meshes_[filename] = mesh;
	return mesh;
}