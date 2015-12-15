#include "components.h"
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
  for(Component* elem: Component::components){
    elem->update( dt );
  }
};  // Run all variable updates (eg renderAll)

void Component::fixedUpdateAll( float dt ){
 for(Component* elem: Component::components){
    elem->fixedUpdate( dt );
  }
}; // Updates on fixed interval (eg physicsUpdateAll)



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

float calculateDistance(GameObject* obj1, GameObject* obj2) {
  return sqrt( pow((obj1->x - obj2->x), 2) + pow((obj1->y - obj2->y), 2) );
}

/**
 * This function, invoked every fixedUpdate, detects if this collider is
 * colliding with any other colliders that are not attached to the same parent
 * object. If it detects a collision, it will invoke any collision resolution
 * functions attached to the current collider.
 *
 * @param dt The elapsed time since the last fixedUpdate in milliseconds
 */
void Collider::fixedUpdate(float dt) {
  // Retrieve a list of all colliders attached to the parent
  list <Component*> parentList =  parent->getComponent("Collider");

  Collider* parentCollider;
  for(Collider* otherCollider: Collider::allColliders) {
    for(Component* elem: parentList) {
      // Skip this loop iteration if the other collider in consideration is
      // attached to the same object as the current collider
      parentCollider = (Collider*) elem;
      if (otherCollider == parentCollider) continue;

      // If there is no collision, skip this loop iteration
      float distance = calculateDistance(otherCollider->parent, parent);
      if (distance > this->radius + otherCollider->radius) continue;

      // Otherwise, execute any registered collision resolution functions
      for (triggerFunc trigger: triggers) trigger(this, otherCollider, dt);
    }
  }
}


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
