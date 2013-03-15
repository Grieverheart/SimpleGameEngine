#ifndef INPUT_MGR_H
#define INPUT_MGR_H

#include "EventMgr/event_manager.h"
#include "entity_event.h"
#include <vector>

namespace SES{
	class Entity;
}

class InputManager{
public:
	InputManager(EventManager *evt_manager_);
	~InputManager(void);
	
	void keyOps(unsigned char, bool);
	
	void onMouseWheel(float position);
	void onPassiveMotion(int x, int y);
	void centerMouse(void);
	
	void linkEntityWithEvent(const EventType&, SES::Entity*);
	void linkEntityWithEvent(const std::vector<EventType>&, SES::Entity*);
	
	void windowSizeChanged(int width, int height);
	
private:
	bool keyRepeat;
	int windowWidth;
	int windowHeight;
	int last_mx,last_my;
	EventManager *evt_manager_;
	std::map<EventType, std::vector<SES::Entity*>> event_assocs_;
};
#endif