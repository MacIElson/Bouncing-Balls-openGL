#include "components.h"
#include "gameLoopConstants.h"
#include "ODLGameLoop_private.h"

#include <stdio.h>
#include <stdlib.h>
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
// list<Component*> Component::components; // Initialize a list of components within the Component namespace

Component::Component( GameObject* parent, string type ) : parent ( parent ), type ( type ) {
  Component::components.push_back( this ); //this is essentially C++ for self (python)
  parent->addComponent( this ); //need the arrow because parent is a pointer, adds the component to the specific game object
}; // Similar as an init method: In this case, initilizes the type and parent fields, and adds self to the static components list.

void Component::update( float dt ) {} //will be overrided in a subclass, so we don't need to specify it
void Component::fixedUpdate( float dt ) {}

void Component::updateAll( float dt ){
  //printf("floatingUpdatedt:%f \n", (float) dt);
  for(Component* elem: Component::components){
    elem->update( dt );
  }
};  // Run all variable updates (eg renderAll)

void Component::fixedUpdateAll( float dt ){
 //printf("fixedUpdatedt:%f \n", (float) dt);
 for(Component* elem: Component::components){
    elem->fixedUpdate( dt );
  }
}; // Updates on fixed interval (eg physicsUpdateAll)


/*class Collider; //Forward declaration, alerts the compiler that collider is coming
typedef void ( * triggerFunc ) ( Collider* c); //New type called triggerFunc, takes a Collider pointer returns void

class Collider : public Component{
  private:
    double radius;
    static list<Collider*> colliders;
    list<triggerFunc> triggers;

  public:
    void addTrigger(triggerFunc);
    Collider( GameObject* parent, double radius);
    void fixedUpdate ( float dt );
*/

list<Collider*> Collider::allColliders;

Collider::Collider( GameObject* parent, double radius ) :
  radius( radius ), Component ( parent, "Collider"){
  Collider::allColliders.push_back( this );
  list<triggerFunc> triggers; //this list is all colliders
};

//addTrigger add trigger function to a list of functions

void Collider::addTrigger(triggerFunc trigger) {
  triggers.push_back(trigger);
}

void Collider::fixedUpdate(float dt) {
  list <Component*> parentList =  parent->getComponent("Collider");
  Collider* parentCollider;
  for(Collider* otherCollider: Collider::allColliders) {
    for(Component* elem: parentList) {
      parentCollider = (Collider*) elem;
      if ((otherCollider) != parentCollider) {
       if (
          sqrt(
            pow((otherCollider->parent->x - parent->x),2) +
            pow((otherCollider->parent->y - parent->y),2)
          ) <
            (otherCollider->radius + parentCollider->radius)
        ) for (triggerFunc trigger: triggers) {
          trigger(parentCollider,otherCollider);
        }
      }
    }
  }
}

/*fixedUpdate iterate through collider list
check to to see that it's not this collider (don't collide w/self)
ask parent object for list of colliders,
  get component gives you list of all collliders attached to parent
  check pointers to see if it's Collider* the same
  if not the same, check to see if radii sum is less than distance
  between colliders parents x and ys parent->x
  if there is a collision, iterate through list of triggerfuncs and
  call every function in that list.
  what you want to pass is the current collider (this) and reference
  to the other collider
*/

/**
 * Begin code to handle CircleRender
 */

CircleRender::CircleRender( GameObject* parent, double radius ) :
radius ( radius ), Component(parent, string("CircleRender")) {
  setColor(0, 0, 1);
}

void CircleRender::setColor(float R, float G, float B) {
  color.R = R;
  color.G = G;
  color.B = B;
}

void CircleRender::update( float dt ) {
  //printf("floatingUpdatedt:%f \n", dt);
  GLfloat ballRadius = (GLfloat) radius;   // Radius of the bouncing ball
  GLfloat ballX = (GLfloat) parent->x;         // Ball's center (x, y) position
  GLfloat ballY = (GLfloat) parent->y;

  glMatrixMode(GL_MODELVIEW);    // To operate on the model-view matrix
  glLoadIdentity();              // Reset model-view matrix

  glTranslatef(ballX, ballY, 0.0f);  // Translate to (xPos, yPos)
  // Use triangular segments to form a circle
  glBegin(GL_TRIANGLE_FAN);
     glColor3f(color.R, color.G, color.B);
     glVertex2f(0.0f, 0.0f);       // Center of circle
     int numSegments = 100;
     GLfloat angle;
     for (int i = 0; i <= numSegments; i++) { // Last vertex same as first vertex
        angle = (i * 2.0f * PI) / numSegments;  // 360 deg for all segments
        glVertex2f(cos(angle) * ballRadius, sin(angle) * ballRadius);
     }
  glEnd();
}

/**
 * Physics Component
 */

Physics::Physics ( GameObject* parent, double dx, double dy, double mass) :
mass ( mass ), dx ( dx ), dy ( dy ), Component (parent, string("Physics")) {};

void Physics::fixedUpdate( float dt ) {
  parent->x += dx * dt;
  parent->y += dy * dt;
  // printf("%f, %f", parent->x, parent->y);
};

/**
 * Begin code to handle wall collisions
 */

WallBounceScript::WallBounceScript( GameObject* parent, double radius ) :
radius ( radius ), Component (parent, string( "WallBounceScript" )) {};

void WallBounceScript::fixedUpdate( float dt ) {
  double x = parent->x;
  double y = parent->y;
  Physics* physComp = dynamic_cast<Physics*> (parent->getComponent(string("Physics")).front());
  double dx = physComp->dx;
  double dy = physComp->dy;

  if (x+radius >= 1) {
    dx = -1 * fabs(dx);
  }
  if (x-radius <= -1) {
    dx = fabs(dx);
  }
  if (y+radius >= 1) {
    dy = -1 * fabs(dy);
  }
  if (y-radius <= -1) {
    dy = fabs(dy);
  }

  physComp->dx = dx;
  physComp->dy = dy;
}


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
    printf("On Demand Game Loop. FPS:%d UPS:%d \n", (int) fps, (int)ups);
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

  //printf("lastLoop:%d lastMeasure:%d time timeAccumulated:%d \n", (int) odlGameLoopState.lastLoopTime, (int) odlGameLoopState.lastMeasurementTime, (int) odlGameLoopState.timeAccumulatedMs);
}

void ODLGameLoop_onOpenGLDisplay() {

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen
  double now = glutGet(GLUT_ELAPSED_TIME);
  double dt = now - odlGameLoopState.lastLoopTime;
  //printf("lastLoop:%f now:%f dt:%f \n", odlGameLoopState.lastLoopTime, now, dt);
  Component::updateAll(dt);
  odlGameLoopState.fpsCount++;
  glutSwapBuffers();
}

void ODLGameLoop_onOpenGLIdle() {

  double now = glutGet(GLUT_ELAPSED_TIME);
  double timeElapsedMs = ((now-odlGameLoopState.lastLoopTime)*1000000)/(CLOCKS_PER_SEC);


  odlGameLoopState.timeAccumulatedMs += timeElapsedMs;

  while(odlGameLoopState.timeAccumulatedMs >= DESIRED_STATE_UPDATE_DURATION_MS) { //
  	  //printf("dt:%f \n", odlGameLoopState.timeAccumulatedMs);


      Component::fixedUpdateAll( (float) odlGameLoopState.timeAccumulatedMs);
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

    char *my_argv[] = { (char*) "dummyArgs", NULL };
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
