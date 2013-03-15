#include "event_manager.h"
#include "debug.h"
#include <cstring>

EventSignal::EventSignal(void):
	next_key_(0)
{}

int EventSignal::connect(EventHandler handler){
	int k = next_key_++;
	handlers_[k] = handler;
	return k;
}

void EventSignal::disconnect(int key){
	handlers_.erase(key);
}

void EventSignal::operator() (const Event& event)const{
	for(auto handler_pair: handlers_){
		const EventHandler& handler = handler_pair.second;
		handler(event);
	}
}

bool EventSignal::empty(void)const{
	return handlers_.empty();
}

#ifdef DEBUG
bool EventManager::valType(const EventType &type)const{
	//Check if string is valid
	if(strlen(type.getText()) == 0){
		PRINT_DEBUG("Attempted to add event with type string of null length");
		return false;
	}
	auto evt_itr = evt_assocs_.find(type);
	//Check if the type exists
	if(evt_itr == evt_assocs_.end()) return true;
	//If it already does check strings for hash collision
	else if( strcmp(evt_itr->first.getText(), type.getText()) == 0) return true;
	PRINT_DEBUG(("Event Manager: Collision found between" + std::string(evt_itr->first.getText()) + "and" + std::string(type.getText())).c_str());
	return false;
}
#else
bool EventManager::valType(const EventType &type)const{
	return true;
}
#endif

int EventManager::addHandler(EventHandler handler, const EventType& type){
	if(!valType(type)) return false;
	//Check if the event type already exists in Map
	EventSignal &signal = evt_assocs_[type];
	//Duplicates are allowed since we can't check if the handlers are equal
	//Add handler to the signal
	return signal.connect(handler);
}

std::vector<int> EventManager::addHandler(EventHandler handler, const std::vector<EventType>& types){
	std::vector<int> connections;
	for(auto itr = types.cbegin(); itr != types.cend(); itr++){
		connections.push_back(addHandler(handler, *itr));
	}
	return std::move(connections);
}

bool EventManager::rmHandler(int handler_key, const EventType& type){
	if(!valType(type)) return false;
	auto em_itr = evt_assocs_.find(type);
	//Return false if the event does not exist
	if(em_itr == evt_assocs_.end()){
		PRINT_DEBUG("Attempted to remove event/handler not previously registered");
		return false;
	}
	EventSignal &signal = em_itr->second;
	signal.disconnect(handler_key);
	//If no handlers left, delete entry
	if(signal.empty()) evt_assocs_.erase(em_itr);
	return true;
}

bool EventManager::queueEvent(Event *event){
	const EventType& type = event->getEventType();
	if(!valType(type)) return false;
	#ifdef DEBUG
	if(evt_assocs_.find(type) == evt_assocs_.end()){
		PRINT_DEBUG("Attempted to queue event with no registered handler");
		return false;
	}
	#endif
	queue_[active_queue_].push_back(event);
	return true;
}

void EventManager::processQueue(void){
	if(!queue_[active_queue_].empty()){
		for(auto event_ptr: queue_[active_queue_]){
			//Find where the type of the event is stored
			auto EventPairItr = evt_assocs_.find(event_ptr->getEventType());
			if(EventPairItr != evt_assocs_.end()){
				const EventSignal &signal = EventPairItr->second;
				//call handlers
				signal(*event_ptr);
			}
		}
		queue_[active_queue_].clear();
	}
	active_queue_ = (active_queue_ + 1)%nQueues;
}


