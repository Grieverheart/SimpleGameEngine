#include "include/world.h"
#include "include/ogl_renderer.h"
#include "include/movement_system.h"
#include "include/input_manager.h"
#include "EntityMgr/entity.h"
#include "include/components.h"
#include "glws/glws.h"
#include "EntityMgr/aspect.h"
#include "EventMgr/event_manager.h"

namespace{
	OGLRenderer* 		renderer; // Our OpenGL Context Object
	MovementSystem* 	movement;
	InputManager* 		input;
	SES::EntityManager* ent_manager;
	EventManager* 		evt_manager;
	MeshManager* 		mesh_manager_;
	TextureManager* 	tex_manager_;
}
	
namespace World{

	bool init(void){
		glwsInit();
		
		renderer = new OGLRenderer( SES::Aspect().all<RenderComponent>() );
		
		if(!renderer->createWindow()){
			glwsClean();
			delete renderer;
			return false;
		}
		
		movement		= new MovementSystem(SES::Aspect().all<MovementComponent>());
		ent_manager 	= new SES::EntityManager();
		evt_manager 	= new EventManager("Simple Event Manager");
		input 			= new InputManager(evt_manager);
		mesh_manager_ 	= new MeshManager();
		tex_manager_ 	= new TextureManager();
		
		ent_manager->registerSystem(renderer);
		ent_manager->registerSystem(movement);
		
		renderer->setupScene();
		input->centerMouse();
		
		// {
			SES::Entity& player = ent_manager->createEntity();
			SES::Entity& mesh = ent_manager->createEntity();
			SES::Entity& plane = ent_manager->createEntity();
			SES::Entity& skybox = ent_manager->createEntity();
			
			player.attachComponent<TransformComponent>( new TransformComponent(glm::vec3(10.0, 0.95, 0.0), 0.0f, glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.5, 0.95, 0.5)) );
			player.attachComponent<MovementComponent>( new MovementComponent() );
			player.attachComponent<RenderComponent>( new RenderComponent(tex_manager_->load("crate.bmp"), mesh_manager_->loadMesh("cube-tex.obj", "flat")) );
			player.getComponent<MovementComponent>()->ground_distance_ = 0.95f;
			
			mesh.attachComponent<RenderComponent>( new RenderComponent(tex_manager_->load("crate2.bmp"), mesh_manager_->loadMesh("cube-tex.obj", "flat")) );
			mesh.attachComponent<TransformComponent>( new TransformComponent(glm::vec3(0.0, 0.5, 0.0), 0.0f, glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.5))  );
			plane.attachComponent<RenderComponent>( new RenderComponent(tex_manager_->load("floor-tile.bmp"), mesh_manager_->loadMesh("plane.obj", "flat"), false) );
			plane.attachComponent<TransformComponent>( new TransformComponent(glm::vec3(0.0), 0.0f, glm::vec3(0.0, 1.0, 0.0), glm::vec3(25.0)) );
			skybox.attachComponent<RenderComponent>( new RenderComponent(tex_manager_->load("wall2.bmp"), mesh_manager_->loadMesh("cube-tex_inv.obj", "flat"), false) );
			skybox.attachComponent<TransformComponent>( new TransformComponent(glm::vec3(0.0), 0.0f, glm::vec3(0.0, 1.0, 0.0), glm::vec3(25.0)) );
		// }
		
		renderer->camera.followEntity(&player);
		ent_manager->processQueue();
		
		glwsSetWindowSizeCB([=](int width, int height){
			renderer->reshapeWindow(width, height);
			input->windowSizeChanged(width, height);
		});
		
		glwsSetMouseWheelCB([=](float x){
			input->onMouseWheel(x);
		});
		
		glwsSetMouseMoveCB([=](int x, int y){
			input->onPassiveMotion(x, y);
		});
		
		glwsSetKeyCB([=](unsigned char key, bool pressed){
			input->keyOps(key, pressed);
		});
		
		input->linkEntityWithEvent(ENTITY_MOVE_START, &player);
		input->linkEntityWithEvent(ENTITY_MOVE_END, &player);
		input->linkEntityWithEvent(ENTITY_ROTATE_BY, &player);
		
		evt_manager->addHandler([=](const Event& event){
			movement->hEntityMovementStart(event);
		}, ENTITY_MOVE_START);
		
		evt_manager->addHandler([&](const Event& event){
			movement->hEntityMovementEnd(event);
		}, ENTITY_MOVE_END);
		
		evt_manager->addHandler([=](const Event& event){
			const EntityXYEvent& xy = static_cast<const EntityXYEvent&>(event);
			renderer->camera.rotate(180.0f * xy.dy_, glm::vec3(1.0, 0.0, 0.0));
			movement->hEntityRotateBy(event);
		}, ENTITY_ROTATE_BY);
		
		return true;
	}
	
	void run(void){
		double last_time = glwsGetElapsedTime();
		while(glwsIsWindowOpen()){
			double this_time = glwsGetElapsedTime();
			double dt = this_time - last_time;
			last_time = this_time;
			movement->update(dt);
			renderer->processScene();
			if(renderer->getState("redisplay")){
				evt_manager->processQueue();
				renderer->toggleState("redisplay");
				renderer->renderScene();
			}
		}
	}
	
	void clean(void){
		delete movement;
		delete renderer;
		delete input;
		delete ent_manager;
		delete evt_manager;
		delete mesh_manager_;
		delete tex_manager_;
		glwsClean();
	}
}