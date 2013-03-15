#ifndef __OPENGL_3_H
#define __OPENGL_3_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "shader.h"
#include "camera.h"
#include "shadowmap_FBO.h"
#include "light.h"
#include "components.h"
#include "EntityMgr/entity.h"
#include "EntityMgr/component_mapper.h"

class OGLRenderer: public SES::EntitySystem{
public:
	OGLRenderer(const SES::Aspect& aspect);
	~OGLRenderer(void);
	bool createWindow(void)const; 			// Creation of our OpenGL 3.x context  
	void setupScene(void);	// All scene information can be setup here  
	void reshapeWindow(int w, int h);			// Method to get our window width and height on resize  
	void renderScene(void)const;
	void processScene(void);
	
	void toggleState(std::string state);
	bool getState(std::string state)const;
	float getZoom(void)const;
	void setZoom(float zoom);
	glm::ivec2 getScreen(void)const;
	
	void multMatrix(std::string matrix1, glm::mat4 matrix2);
	void setMatrix(std::string matrix1, glm::mat4 matrix2);
	
	Camera camera;
	
private:
	void calcLightViewMatrix(void);
	void fboPass(void)const;
	void drawPass(void)const;
	
	int windowWidth;	//Store the width of the window
	int windowHeight;	//Store the height of the window
	float fov, zoom;
	bool redisplay;
	
	glm::mat4 ProjectionMatrix;
	glm::mat4 ViewMatrix;
	glm::mat4 ModelMatrix;
	glm::mat4 lightViewMatrix;
	glm::mat4 lightProjectionMatrix;

	SES::ComponentMapper<RenderComponent> render_comps_;
	SES::ComponentMapper<TransformComponent> transform_comps_;
	std::vector<CLight> light;
	Shader *shader; // GLSL Shader
	CShadowMapFBO m_ShadowMapFBO;
};


#endif