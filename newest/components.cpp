#include "components.h"
#include "gameLoopConstants.h"
#include "ODLGameLoop_private.h"

#include <stdio.h>
#include <time.h>
#include <math.h> 
#include <string>
#include <GL/gl.h>     // The GL Header File
#include <GL/glut.h>   // The GL Utility Toolkit (Glut) Header
#include <unistd.h>

#define PI 3.14159265f

using namespace std;

ODLGameLoopState odlGameLoopState;

// GameObject Implementation
GameObject::GameObject( double x, double y ) : x( x ), y( y ) {}

list<Component*> GameObject::getComponent( string type ) {
  list<Component*> res;
  for(Component* elem: componentList) {
    if ((elem->type).compare( type ) == 0) res.push_back( elem );
  }
  return res;
}

/**
 * Adds a component to a particular GameObject
 */ 
void GameObject::addComponent( Component* c) {
  componentList.push_back( c );
}

// Component Implementation
list<Component*> Component::components; // Initialize a list of components within the Component namespace

Component::Component( GameObject* parent, string type ) : parent ( parent ), type ( type ) {
  Component::components.push_back( this ); //this is essentially C++ for self (python)
  parent->addComponent( this ); //need the arrow because parent is a pointer, adds the component to the specific game object 
}; // Similar as an init method: In this case, initilizes the type and parent fields, and adds self to the static components list.

void Component::update( float dt ) {} //will be overrided in a subclass, so we don't need to specify it 
void Component::fixedUpdate( float dt ) {usleep(17000);}

void Component::updateAll( float dt ){
  for(Component* elem: Component::components){
    elem->update( dt );
  }
};  // Run all variable updates (eg renderAll) 

void Component::fixedUpdateAll( float dt ){
 for(Component* elem: Component::components){
    elem->fixedUpdate( dt );
  }
}; // Updates on fixed interval (eg physicsUpdateAll)

/**
 * Begin code to handle game loop
 */ 

void ODLGameLoop_updateMeasurements() {
  double now = glutGet(GLUT_ELAPSED_TIME);
  //printf("now:%d last:%d \n", (int) now, (int) odlGameLoopState.lastMeasurementTime);
  double timeElapsedMs = ((now-odlGameLoopState.lastMeasurementTime)*1000000)/(CLOCKS_PER_SEC);
  //printf("timeElapsed:%d \n", (int) timeElapsedMs);
  if(timeElapsedMs>=500) {

    double ups = (odlGameLoopState.upsCount*1000)/timeElapsedMs;
    double fps = (odlGameLoopState.fpsCount*1000)/timeElapsedMs;
    char title[100];
    printf("C Game Loop Study - On Demand Game Loop. FPS:%d UPS:%d \n", (int) fps, (int)ups);
    odlGameLoopState.upsCount = 0;
    odlGameLoopState.fpsCount = 0;
    odlGameLoopState.lastMeasurementTime = now;
  }
}

void ODLGameLoop_initGameLoopState() {

  odlGameLoopState.lastLoopTime = glutGet(GLUT_ELAPSED_TIME);
  odlGameLoopState.lastMeasurementTime = glutGet(GLUT_ELAPSED_TIME);

  odlGameLoopState.desiredStateUpdatesPerSecond = DESIRED_STATE_UPDATES_PER_SECOND;
  odlGameLoopState.desiredStateUpdateDurationMs = DESIRED_STATE_UPDATE_DURATION_MS;

  odlGameLoopState.upsCount = 0;
  odlGameLoopState.fpsCount = 0;

  odlGameLoopState.timeAccumulatedMs = glutGet(GLUT_ELAPSED_TIME);

  printf("lastLoop:%d lastMeasure:%d time timeAccumulated:%d \n", (int) odlGameLoopState.lastLoopTime, (int) odlGameLoopState.lastMeasurementTime, (int) odlGameLoopState.timeAccumulatedMs);
}

void ODLGameLoop_onOpenGLDisplay() {

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen
  Component::updateAll(0);
  odlGameLoopState.fpsCount++;
  glutSwapBuffers();
}

void ODLGameLoop_onOpenGLIdle() {

  double now = glutGet(GLUT_ELAPSED_TIME);
  double timeElapsedMs = ((now-odlGameLoopState.lastLoopTime)*1000000)/(CLOCKS_PER_SEC);

  odlGameLoopState.timeAccumulatedMs += timeElapsedMs;

  //printf("timeElapsed: %d timeAccumulatedMs: %d DESIRED_STATE_UPDATE_DURATION_MS: %d \n", (int)timeElapsedMs, (int)odlGameLoopState.timeAccumulatedMs, DESIRED_STATE_UPDATE_DURATION_MS);
  
  while(odlGameLoopState.timeAccumulatedMs >= DESIRED_STATE_UPDATE_DURATION_MS) { //

      Component::fixedUpdateAll(timeElapsedMs);
      //ODLGameLoop_updateState();
      odlGameLoopState.timeAccumulatedMs -= DESIRED_STATE_UPDATE_DURATION_MS;
  
      odlGameLoopState.upsCount++;
      ODLGameLoop_updateMeasurements();
  
      glutPostRedisplay();
      
  }
  
  odlGameLoopState.lastLoopTime = now;  
}

void ODLGameLoop_initOpenGL() {
    char title[] = "Test Window";  // Windowed mode's title
    int windowWidth  = VIEW_WIDTH;     // Windowed mode's width
    int windowHeight = VIEW_HEIGHT;     // Windowed mode's height
    int windowPosX   = WINDOW_POS_X;      // Windowed mode's top-left corner x
    int windowPosY   = WINDOW_POS_Y;      // Windowed mode's top-left corner y

    char *my_argv[] = { "dummyArgs", NULL };
    int   my_argc = 1;
    glutInit(&my_argc, my_argv);

    glutInitDisplayMode(GLUT_DOUBLE); // Enable double buffered mode
    glutInitWindowSize(windowWidth, windowHeight);  // Initial window width and height
    glutInitWindowPosition(windowPosX, windowPosY); // Initial window top-left corner (x, y)
    glutCreateWindow(title);      // Create window with given title

    glShadeModel(GL_SMOOTH);              // Enable Smooth Shading
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);       // Black Background
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClearDepth(1.0f);                 // Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);              // Enables Depth Testing
    glDepthFunc(GL_LEQUAL);               // The Type Of Depth Testing To Do
    glEnable(GL_COLOR_MATERIAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    //...
    glutDisplayFunc(ODLGameLoop_onOpenGLDisplay);
    glutIdleFunc(ODLGameLoop_onOpenGLIdle);
    //glutReshapeFunc(reshape);     // Register callback handler for window re-shape
        //...

    ODLGameLoop_initGameLoopState();

    glutMainLoop();
}

/**
 * End code to handle game loop
 */ 

// Testing
int main() {

  GameObject obj ( 1, 1 );
  if (obj.getComponent( "physics" ).empty()) printf( "Test Passed\n" );
  else printf ( "Test Failed" );

  // Instantiate a Component (adding it to obj), and attempt to retrieve it.
  Component comp ( &obj, "physics" );
  if (obj.getComponent( "physics" ).front() == &comp) printf( "Test Passed\n" );
  else printf ( "Test Failed" );

  ODLGameLoop_initOpenGL();

}
