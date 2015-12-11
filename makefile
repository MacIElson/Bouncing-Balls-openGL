components.o: src/components.cpp src/components.h src/gameLoopConstants.h src/ODLGameLoop_private.h
	g++ -std=c++11 src/components.cpp -lglut -lGLU -lGL -o bin/components
