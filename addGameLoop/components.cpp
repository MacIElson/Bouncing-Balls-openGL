#include "components.h"
#include "gameLoopConstants.h"
#include "ODLGameLoop_private.h"

#include <time.h>
#include <stdio.h>
#include <math.h> 
#include <string>
#include <GL/gl.h>     // The GL Header File
#include <GL/glut.h>   // The GL Utility Toolkit (Glut) Header

#define PI 3.14159265f

using namespace std;

ODLGameLoopState odlGameLoopState;

GLfloat ballXMax, ballXMin, ballYMax, ballYMin; // Ball's center (x, y) bounds

// Projection clipping area
GLdouble clipAreaXLeft, clipAreaXRight, clipAreaYBottom, clipAreaYTop;

GLfloat ballRadius = 0.5f;

string someString("This is a string.");

// -- TRANSFORM -- //
Transform::Transform(double x, double y) : x ( x ), y ( y ) { }
double Transform::getX( void ) { return x; }
double Transform::getY( void ) { return y; }


// -- PHYSICS -- //
// Declare static members
list<Physics*> Physics::updateList;

Physics::Physics (
  Transform *transform, double dx, double dy
) : transform ( transform ), dx( dx ), dy( dy ) {
  // Register this Physics component for later updates
  Physics::updateList.push_back(this);
}

void Physics::updateAll() {
  for(Physics* elem: Physics::updateList) elem->update();
}

void Physics::update() {
  // Update transform state
  transform->x += dx;
  transform->y += dy;

  //Check if the ball exceeds the edges
  if (transform->x > ballXMax) {
     transform->x = ballXMax;
     dx = -dx;
  } else if (transform->x < ballXMin) {
     transform->x = ballXMin;
     dx = -dx;
  }
  if (transform->y > ballYMax) {
     transform->y = ballYMax;
     dy = -dy;
  } else if (transform->y < ballYMin) {
     transform->y = ballYMin;
     dy = -dy;
  }

}


// -- RENDERABLE -- //
// Declare static members
list<Renderable*> Renderable::renderList;

Renderable::Renderable (
  Transform *transform, renderFunc renderFunc
) : transform ( transform ), render ( renderFunc ) {
  Renderable::renderList.push_back(this);
}

void Renderable::renderAll() {
  for(Renderable* elem: Renderable::renderList) elem->render(elem->transform);
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

/* Call back when the windows is re-sized */
void reshape(GLsizei width, GLsizei height) {
   // Compute aspect ratio of the new window
   if (height == 0) height = 1;                // To prevent divide by 0
   GLfloat aspect = (GLfloat)width / (GLfloat)height;
 
   // Set the viewport to cover the new window
   glViewport(0, 0, width, height);
 
   // Set the aspect ratio of the clipping area to match the viewport
   glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
   glLoadIdentity();             // Reset the projection matrix
   if (width >= height) {
      clipAreaXLeft   = -1.0 * aspect;
      clipAreaXRight  = 1.0 * aspect;
      clipAreaYBottom = -1.0;
      clipAreaYTop    = 1.0;
   } else {
      clipAreaXLeft   = -1.0;
      clipAreaXRight  = 1.0;
      clipAreaYBottom = -1.0 / aspect;
      clipAreaYTop    = 1.0 / aspect;
   }
   gluOrtho2D(clipAreaXLeft, clipAreaXRight, clipAreaYBottom, clipAreaYTop);
   ballXMin = clipAreaXLeft + ballRadius;
   ballXMax = clipAreaXRight - ballRadius;
   ballYMin = clipAreaYBottom + ballRadius;
   ballYMax = clipAreaYTop - ballRadius;
}

void ODLGameLoop_updateMeasurements() {
  double now = glutGet(GLUT_ELAPSED_TIME);
  //printf("now:%d last:%d \n", (int) now, (int) odlGameLoopState.lastMeasurementTime);
  double timeElapsedMs = ((now-odlGameLoopState.lastMeasurementTime)*1000000)/(CLOCKS_PER_SEC);
  //printf("timeElapsed:%d \n", (int) timeElapsedMs);
  if(timeElapsedMs>=500) {

    double ups = (odlGameLoopState.upsCount*1000)/timeElapsedMs;
    double fps = (odlGameLoopState.fpsCount*1000)/timeElapsedMs;
    char title[100];
    printf("C Game Loop Study - On Demand Game Loop. FPS:%d UPS:%d \n", (int)fps, (int)ups);
    //glutSetWindowTitle(someString);
    odlGameLoopState.upsCount = 0;
    odlGameLoopState.fpsCount = 0;
    odlGameLoopState.lastMeasurementTime = now;
  }
}

void ODLGameLoop_updateGraphics() {

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen
  Renderable::renderAll();
  odlGameLoopState.fpsCount++;
  glutSwapBuffers();
}

void ODLGameLoop_onOpenGLIdle() {

  double now = glutGet(GLUT_ELAPSED_TIME);
  double timeElapsedMs = ((now-odlGameLoopState.lastLoopTime)*1000000)/(CLOCKS_PER_SEC);

  odlGameLoopState.timeAccumulatedMs += timeElapsedMs;

  //printf("timeElapsed: %d timeAccumulatedMs: %d DESIRED_STATE_UPDATE_DURATION_MS: %d \n", (int)timeElapsedMs, (int)odlGameLoopState.timeAccumulatedMs, DESIRED_STATE_UPDATE_DURATION_MS);
  
  while(odlGameLoopState.timeAccumulatedMs >= DESIRED_STATE_UPDATE_DURATION_MS) { //
      //printf("%s\n",someString.c_str());

      Physics::updateAll();
      //ODLGameLoop_updateState();
      odlGameLoopState.timeAccumulatedMs -= DESIRED_STATE_UPDATE_DURATION_MS;
  
      odlGameLoopState.upsCount++;
      ODLGameLoop_updateMeasurements();
  
      glutPostRedisplay();
      
  }
  //glutPostRedisplay();
  //printf("%s\n",someString.c_str());
  odlGameLoopState.lastLoopTime = now;



  
}


void ODLGameLoop_initOpenGL() {
    char title[] = "Test Window";  // Windowed mode's title
    int windowWidth  = 640;     // Windowed mode's width
    int windowHeight = 480;     // Windowed mode's height
    int windowPosX   = 50;      // Windowed mode's top-left corner x
    int windowPosY   = 50;      // Windowed mode's top-left corner y

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
    glutDisplayFunc(ODLGameLoop_updateGraphics);
    glutIdleFunc(ODLGameLoop_onOpenGLIdle);
    glutReshapeFunc(reshape);     // Register callback handler for window re-shape
        //...

    ODLGameLoop_initGameLoopState();

    glutMainLoop();
}

// -- TESTING SCRIPT -- //
int main ()  {

  // Create a bubble object at Position (0, 0)
  Transform testBubble(0, 0);

  // Attach physics to the bubble w. initial velocity of (0, 10)
  Physics p(&testBubble, 0.005, 0.005);

  // Attach a renderer to the bubble that displays the current position of the bubble
  auto dispCoords = [](Transform* t) { //printf("Position: (%f, %f)\n", t->x, t->y); 
    GLfloat ballRadius = 0.5f;   // Radius of the bouncing ball
    GLfloat ballX = (GLfloat) t->x;         // Ball's center (x, y) position
    GLfloat ballY = (GLfloat) t->y;

    glMatrixMode(GL_MODELVIEW);    // To operate on the model-view matrix
    glLoadIdentity();              // Reset model-view matrix
    
    glTranslatef(ballX, ballY, 0.0f);  // Translate to (xPos, yPos)
    // Use triangular segments to form a circle
    glBegin(GL_TRIANGLE_FAN);
       glColor3f(0.0f, 0.0f, 1.0f);  // Blue
       glVertex2f(0.0f, 0.0f);       // Center of circle
       int numSegments = 100;
       GLfloat angle;
       for (int i = 0; i <= numSegments; i++) { // Last vertex same as first vertex
          angle = (i * 2.0f * PI) / numSegments;  // 360 deg for all segments
          glVertex2f(cos(angle) * ballRadius, sin(angle) * ballRadius);
       }
    glEnd();

  };

  Renderable r(&testBubble, dispCoords);

  ODLGameLoop_initOpenGL();
}
