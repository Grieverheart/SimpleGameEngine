#include "include/ogl_renderer.h"
#include "glws/glws.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

OGLRenderer::OGLRenderer(const SES::Aspect& aspect):
	SES::EntitySystem(aspect),
	camera(1.0f, glm::vec3(0.0, 1.5, 5.5)),
	windowWidth(600),
	windowHeight(600)
{
	/////////////////////////////////////////////////
	// Default Constructor for OGLRenderer class //
	/////////////////////////////////////////////////
	zoom = 0.0f;
	fov = 60.0f;
	redisplay = false;
	light.push_back(CLight(glm::vec3(-7.0, 7.0, 0.0), glm::vec3(1.0, -1.0, 0.0)));
}

OGLRenderer::~OGLRenderer(void) {
	delete shader;
}  

void OGLRenderer::toggleState(std::string state){
	if(state == "redisplay") redisplay = !redisplay;
	else std::cout << "Cannot toggle state " << state << std::endl;
}

bool OGLRenderer::getState(std::string state)const{
	if(state == "redisplay") return redisplay;
	else std::cout << "Cannot return state " << state << std::endl;
	return false;
}

void OGLRenderer::multMatrix(std::string matrix1, glm::mat4 matrix2){
	if(matrix1 == "ModelMatrix") ModelMatrix *= matrix2;
	else if(matrix1 == "ViewMatrix") ViewMatrix *= matrix2;
}

void OGLRenderer::setMatrix(std::string matrix1, glm::mat4 matrix2){
	if(matrix1 == "ModelMatrix") ModelMatrix = matrix2;
	else if(matrix1 == "ViewMatrix") ViewMatrix = matrix2;
}

bool OGLRenderer::createWindow(void)const{
	////////////////////////////////////////////////////
	// Create an OpenGL 3.2 context. Fall back to a   //
	// OpenGL 2.1 Context if it fails.				  //
	////////////////////////////////////////////////////
	
	glwsSetContextVersion(3, 3);
	if(!glwsCreateWindow("Project", windowWidth, windowHeight)) return false;
	
	int glVersion[2] = {-1,1};
	glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);
	
	std::cout << "Using OpenGL: " << glVersion[0] << "." << glVersion[1] << std::endl;
	
	glewExperimental = GL_TRUE;
	GLenum error = glewInit(); //Enable GLEW
	if(error != GLEW_OK) return false; //Failure!
	
	return true; // Success, return true
}

void OGLRenderer::setupScene(void){
	glClearColor(0.4f,0.6f,0.9f,1.0f);
	
	glEnable(GL_BLEND);
	glEnable(GL_MULTISAMPLE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, windowWidth, windowHeight);
	
	shader = new Shader("shaders/shader.vert", "shaders/shader.frag");
	
	if(!light[0].Init(shader->id())) std::cout << "Could not bind light uniforms" << std::endl;
	
	camera.setProjectionParams((float)windowWidth / windowHeight, fov, 0.1f, 100.0f);
	ProjectionMatrix = camera.getProjectionMatrix();
	ViewMatrix = camera.getViewMatrix();
	ModelMatrix = glm::mat4(1.0);
	calcLightViewMatrix();
	
	if(!m_ShadowMapFBO.Init(windowWidth, windowHeight)) std::cout << "Couldn't initialize FBO!" << std::endl;
	
	
	//Change if you want full scene culling. It now only culls for fbo pass
	glCullFace(GL_FRONT);
	
	shader->bind();
	{
		shader->setUniform("inShadowMap", 0);
		shader->setUniform("inSampler", 1);
	}
	shader->unbind();
}

void OGLRenderer::reshapeWindow(int w, int h){
	windowWidth = w;
	windowHeight = h;
	glViewport(0, 0, windowWidth, windowHeight);
	camera.setProjectionParams((float)windowWidth / windowHeight, 0.0f, 0.0f, 0.0f);
	ProjectionMatrix = camera.getProjectionMatrix();
	m_ShadowMapFBO.Resize(windowWidth, windowHeight);
}

void OGLRenderer::processScene(void){
	static double last_time = 0.0;
	double this_time = glwsGetElapsedTime();
	// if(this_time - last_time >= 1.0/61.0){
		camera.updateView();
		redisplay = true;
		last_time = this_time;
		ProjectionMatrix = camera.getProjectionMatrix();
		ViewMatrix = camera.getViewMatrix();
		calcLightViewMatrix();
	// }
}

void OGLRenderer::calcLightViewMatrix(void){
	//Calculate ViewMatrix
	glm::vec3 light_position = light[0].getPosition();
	glm::vec3 light_direction = light[0].getDirection();
	glm::vec3 upVector(0.0,1.0,0.0);
	glm::vec3 sVector;
	sVector = glm::normalize(glm::cross(light_direction, upVector));
	upVector = glm::normalize(glm::cross(sVector, light_direction));
	
	lightViewMatrix = glm::mat4(
		glm::vec4(sVector,glm::dot(-light_position,sVector)),
		glm::vec4(upVector,glm::dot(-light_position,upVector)),
		glm::vec4(-light_direction, glm::dot(light_direction,light_position)),
		glm::vec4(0.0,0.0,0.0,1.0)
	);
	lightViewMatrix = glm::transpose(lightViewMatrix);
	lightProjectionMatrix = glm::perspective(fov, (float)windowWidth/(float)windowHeight, 13.0f, 18.0f);
	// lightProjectionMatrix = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 13.0f, 18.0f);
}

void OGLRenderer::fboPass(void)const{
	glEnable(GL_CULL_FACE);

	m_ShadowMapFBO.BindForWriting();

	glClear(GL_DEPTH_BUFFER_BIT);
	
	glm::mat4 lightMVPMatrix = lightProjectionMatrix * lightViewMatrix * ModelMatrix;;
	
	{
		for(auto entity: active_entities_){
			if(!render_comps_[*entity]->isShadowCaster_) continue;
			glm::mat4 lLMVPMatrix = lightMVPMatrix * transform_comps_[*entity]->modelMatrix();
			shader->setUniform("MVPMatrix", 1, lLMVPMatrix);
			
			render_comps_[*entity]->mesh_->draw();
		}
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_CULL_FACE);
}

void OGLRenderer::drawPass(void)const{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	m_ShadowMapFBO.BindForReading(GL_TEXTURE0 + 0);
	
	glActiveTexture(GL_TEXTURE0 + 1);
	
	glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
	glm::mat4 lightMVPMatrix = lightProjectionMatrix * lightViewMatrix * ModelMatrix;
	
	light[0].uploadDirection(ViewMatrix);
	
	for(auto entity: active_entities_){
	
		glm::mat4 lMVMatrix = ModelViewMatrix * transform_comps_[*entity]->modelMatrix();
		glm::mat4 lMVPMatrix = ProjectionMatrix * lMVMatrix;
		glm::mat3 lNormalMatrix = glm::mat3(glm::transpose(glm::inverse(lMVMatrix)));
		glm::mat4 lLMVPMatrix = lightMVPMatrix * transform_comps_[*entity]->modelMatrix();
		
		light[0].uploadMVP(lLMVPMatrix);
		shader->setUniform("MVPMatrix", 1, lMVPMatrix);
		shader->setUniform("ModelViewMatrix", 1, lMVMatrix);
		shader->setUniform("NormalMatrix", 1, lNormalMatrix);
		
		glBindTexture(GL_TEXTURE_2D, render_comps_[*entity]->tex_id_);
		render_comps_[*entity]->mesh_->draw();
	}
}

void OGLRenderer::renderScene(void)const{
	
	shader->bind();
	{
		fboPass();
		drawPass();
	}
	shader->unbind();
	
	glwsSwapBuffers();
}

float OGLRenderer::getZoom(void)const{
	return zoom;
}

void OGLRenderer::setZoom(float zoom){
	this->zoom = zoom;
}


glm::ivec2 OGLRenderer::getScreen(void)const{
	glm::ivec2 screen = glm::ivec2(windowWidth, windowHeight);
	return screen;
}