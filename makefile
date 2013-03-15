SRC=$(wildcard src/*.cpp)
OBJ=$(patsubst src/%.cpp, bin/%.o, $(SRC))
OBJ+= bin/glws.o bin/component_type_manager.o bin/entity_manager.o bin/entity.o bin/entity_system.o bin/event_manager.o
EXE=main.exe

CC=gcc
CFLAGS=-Wall -O3 -std=c99
CXX=g++
CXXFLAGS=-Wall -O3 -std=c++0x
LDFLAGS= -lglew32 -lopengl32 -lgdi32 -lfreeImage
RM=del /q

vpath %.o bin/

bin/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -I. -c $< -o $@
	
bin/glws.o: glws/glws.c glws/glws.h
	$(CC) $(CFLAGS) -c $< -o $@
	
bin/component_type_manager.o: EntityMgr/component_type_manager.cpp EntityMgr/component_type_manager.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
bin/entity_manager.o: EntityMgr/entity_manager.cpp EntityMgr/entity_manager.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
bin/entity.o: EntityMgr/entity.cpp EntityMgr/entity.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
bin/entity_system.o: EntityMgr/entity_system.cpp EntityMgr/entity_system.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
bin/event_manager.o: EventMgr/event_manager.cpp EventMgr/event_manager.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: all
all: $(EXE)
	@echo Done

$(EXE): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $@
	
.PHONY: clean
clean:
	-$(RM) bin\*
	@echo Clean Done!
