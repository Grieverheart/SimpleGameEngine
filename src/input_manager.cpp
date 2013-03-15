#include "include/input_manager.h"
#include "include/camera.h"
#include "glws/glws.h"

InputManager::InputManager(EventManager *manager):
	windowWidth(600),
	windowHeight(600)
{
	keyRepeat = false;
	this->evt_manager_ = manager;
}

InputManager::~InputManager(void){
}

void InputManager::keyOps(unsigned char key, bool pressed){
	switch(key){
	case 'W':
		if(pressed){
			auto pair = event_assocs_.find(ENTITY_MOVE_FW_START);
			if(pair != event_assocs_.end()){
				for(auto entity: pair->second) evt_manager_->queueEvent(new EntityEvent(ENTITY_MOVE_FW_START, entity));
			}
		}
		else{
			auto pair = event_assocs_.find(ENTITY_MOVE_FW_END);
			if(pair != event_assocs_.end()){
				for(auto entity: pair->second) evt_manager_->queueEvent(new EntityEvent(ENTITY_MOVE_FW_END, entity));
			}
		}
		break;
		
	case 'S':
		if(pressed){
			auto pair = event_assocs_.find(ENTITY_MOVE_BW_START);
			if(pair != event_assocs_.end()){
				for(auto entity: pair->second) evt_manager_->queueEvent(new EntityEvent(ENTITY_MOVE_BW_START, entity));
			}
		}
		else{
			auto pair = event_assocs_.find(ENTITY_MOVE_BW_END);
			if(pair != event_assocs_.end()){
				for(auto entity: pair->second) evt_manager_->queueEvent(new EntityEvent(ENTITY_MOVE_BW_END, entity));
			}
		}
		break;
	
	case 'Q':
		if(pressed){
			auto pair = event_assocs_.find(ENTITY_STRAFE_LEFT_START);
			if(pair != event_assocs_.end()){
				for(auto entity: pair->second) evt_manager_->queueEvent(new EntityEvent(ENTITY_STRAFE_LEFT_START, entity));
			}
		}
		else{
			auto pair = event_assocs_.find(ENTITY_STRAFE_LEFT_END);
			if(pair != event_assocs_.end()){
				for(auto entity: pair->second) evt_manager_->queueEvent(new EntityEvent(ENTITY_STRAFE_LEFT_END, entity));
			}
		}
		break;
	
	case 'E':
		if(pressed){
			auto pair = event_assocs_.find(ENTITY_STRAFE_RIGHT_START);
			if(pair != event_assocs_.end()){
				for(auto entity: pair->second) evt_manager_->queueEvent(new EntityEvent(ENTITY_STRAFE_RIGHT_START, entity));
			}
		}
		else{
			auto pair = event_assocs_.find(ENTITY_STRAFE_RIGHT_END);
			if(pair != event_assocs_.end()){
				for(auto entity: pair->second) evt_manager_->queueEvent(new EntityEvent(ENTITY_STRAFE_RIGHT_END, entity));
			}
		}
		break;
	
	case 'A':
		if(pressed){
			auto pair = event_assocs_.find(ENTITY_TURN_LEFT_START);
			if(pair != event_assocs_.end()){
				for(auto entity: pair->second) evt_manager_->queueEvent(new EntityEvent(ENTITY_TURN_LEFT_START, entity));
			}
		}
		else{
			auto pair = event_assocs_.find(ENTITY_TURN_LEFT_END);
			if(pair != event_assocs_.end()){
				for(auto entity: pair->second) evt_manager_->queueEvent(new EntityEvent(ENTITY_TURN_LEFT_END, entity));
			}
		}
		break;
	
	case 'D':
		if(pressed){
			auto pair = event_assocs_.find(ENTITY_TURN_RIGHT_START);
			if(pair != event_assocs_.end()){
				for(auto entity: pair->second) evt_manager_->queueEvent(new EntityEvent(ENTITY_TURN_RIGHT_START, entity));
			}
		}
		else{
			auto pair = event_assocs_.find(ENTITY_TURN_RIGHT_END);
			if(pair != event_assocs_.end()){
				for(auto entity: pair->second) evt_manager_->queueEvent(new EntityEvent(ENTITY_TURN_RIGHT_END, entity));
			}
		}
		break;
	
	case 0x70: //F1
		if(pressed){
			glwsToggleKeyRepeat();
			keyRepeat=!keyRepeat;
			if(keyRepeat) printf("Key Repeat Enabled\n");
			else printf("Key Repeat Disabled\n");
		}
		break;
	case 0x20:
		if(pressed){
			auto pair = event_assocs_.find(ENTITY_JUMP_START);
			if(pair != event_assocs_.end()){
				for(auto entity: pair->second) evt_manager_->queueEvent(new EntityEvent(ENTITY_JUMP_START, entity));
			}
		}
		else{
			auto pair = event_assocs_.find(ENTITY_JUMP_END);
			if(pair != event_assocs_.end()){
				for(auto entity: pair->second) evt_manager_->queueEvent(new EntityEvent(ENTITY_JUMP_END, entity));
			}
		}
		break;
	}
	
}

void InputManager::centerMouse(void){
	last_my = windowHeight/2, last_mx = windowWidth/2;
	glwsSetMousePos(last_mx, last_my);
}

void InputManager::onMouseWheel(float position){
	// float zoom = renderer->getZoom() - 1.5f * position;
	// if(zoom < -58.0f) zoom =- 58.0f;
	// else if(zoom > 90.0f) zoom = 90.0f;
	// renderer->setZoom(zoom);
}

void InputManager::onPassiveMotion(int x, int y){
	if(x != last_mx || y != last_my){
		float dx = (float)(x - last_mx)/windowWidth;
		float dy = (float)(last_my - y)/windowHeight;
		last_my = y, last_mx = x;
		
		auto pair = event_assocs_.find(ENTITY_ROTATE_BY);
		if(pair != event_assocs_.end()){
			for(auto entity: pair->second) evt_manager_->queueEvent(new EntityXYEvent(ENTITY_ROTATE_BY, entity, dx, dy));
		}
		
		centerMouse();
	}
}

void InputManager::windowSizeChanged(int width, int height){
	windowWidth = width;
	windowHeight = height;
	centerMouse();
}

void InputManager::linkEntityWithEvent(const EventType& type, SES::Entity* entity){
	event_assocs_[type].push_back(entity);
}

void InputManager::linkEntityWithEvent(const std::vector<EventType>& types, SES::Entity* entity){
	for(auto type: types) event_assocs_[type].push_back(entity);
}
	