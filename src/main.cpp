#include "include/world.h"

int main(int argc,char *argv[] ){
	
	if(!World::init()) return 1;
	
	World::run();
	
	World::clean();
	
	return 0;
}
