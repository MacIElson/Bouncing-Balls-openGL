/*-------------------------------------------------------

Implements all of the components used program.

---------------------------------------------------------*/

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

/* This function is a constructor for a GameObject. 
 * It takes in 2 doubles, x and y, which define the statring 
 * location of the object.
 */
GameObject::GameObject( double x, double y ) : x( x ), y( y ) {}

/* This function takes in a string (type) and returns a list of all
 * components that have that sting as their type
 */
list<Component*> GameObject::getComponent( string type ) {
  list<Component*> res;
  for(Component* elem: componentList) {
    if ((elem->type).compare( type ) == 0) res.push_back( elem );
  }
  return res;
}

/**
 * This function takes in a component (c) and adds it to a 
 * GameObject's component list. 
 */
void GameObject::addComponent( Component* c) {
  componentList.push_back( c );
}

/* This function is a constructor for a general Component. 
 * It takes in a GameObject (parent) and a string (type) and saves them as object variables. 
 * The parent is the GameObject the component belongs to. The type is the type of Component.
 * It also adds the component constructed component to the static list of components and adds 
 * the component to its parent gameObject's list of components.
 */

Component::Component( GameObject* parent, string type ) : parent ( parent ), type ( type ) {
  Component::components.push_back( this ); 
  parent->addComponent( this );
};

/* This function takes in a float (dt)
 * The defualt implementation does nothing
 * This function is intended to be run at a variable interval (useful for things like rendering)
 */
void Component::update( float dt ) {}

/* This function takes in a float (dt)
 * The defualt implementation does nothing.
 * This function is intended to be run at a set interval (useful for things like physics)
 */
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
