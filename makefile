components.o: src/components.cpp src/components.h src/gameLoopConstants.h src/ODLGameLoop_private.h
	mkdir -p bin && g++ -std=c++11 src/main.cpp -lglut -lGLU -lGL -o bin/BallBouncer
