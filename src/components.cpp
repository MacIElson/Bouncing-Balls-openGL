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

/*Begin GameObject-------------------------------------------------------*/

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

/*End GameObject---------------------------------------------------------*/

/*Begin Component-------------------------------------------------------*/

/* This function is a constructor for a general Component. 
 * It takes in a GameObject (parent) and a string (type) and saves them as object variables. 
 * The parent is the GameObject the component belongs to. The type is the type of Component.
 * It also adds the newly constructed component to the static list of all components and adds 
 * the component to its parent gameObject's list of components.
 */

Component::Component( GameObject* parent, string type ) : parent ( parent ), type ( type ) {
  Component::components.push_back( this ); 
  parent->addComponent( this );
};

/* This function takes in a float (dt) representing the time since the last fixedUpdate
 * This function is intended to be run at a variable interval (useful for things like rendering)
 * The defualt implementation does nothing
 */
void Component::update( float dt ) {}

/* This function takes in a float (dt) representing the time since the last fixedUpdate
 * This function is intended to be run at a set interval (useful for things like physics)
 * The defualt implementation does nothing.
 */
void Component::fixedUpdate( float dt ) {}

/* This function takes in a float (dt) representing the time since the last fixedUpdate
 * This function is intended to be run at a variable interval (useful for things like rendering)
 * It runs the update(float dt) funtion for every component
 */
void Component::updateAll( float dt ){
  //printf("floatingUpdateDt:%f \n", (float) dt);
  for(Component* elem: Component::components){
    elem->update( dt );
  }
};

/* This function takes in a float (dt) representing the time since the last fixedUpdate
 * This function is intended to be run at a set interval (useful for things like physics)
 * It runs the fixedUpdate(float dt) funtion for every component
 */
void Component::fixedUpdateAll( float dt ){
  //printf("fixedUpdateDt:%f \n", (float) dt);
  for(Component* elem: Component::components){
    elem->fixedUpdate( dt );
  }
};

/*End Component-------------------------------------------------------*/


/*Begin Collider (extends Component)-------------------------------------------------------*/

list<Collider*> Collider::allColliders;

/* This function is a constructor for a Collider. 
 * It takes in a GameObject (parent) and a double (radius). It saves radius as an object variable.
 * It also adds the newly constructed collider to the static list of all colliders (for access to all colliders later).
 * Finally, it runs the parent constructor for Component (to complete setup).
 */
Collider::Collider( GameObject* parent, double radius ) :
  radius( radius ), Component ( parent, "Collider"){
  Collider::allColliders.push_back( this );
  list<triggerFunc> triggers; //this list is all colliders
};

/* This function takes in a triggerFunc (trigger)
 * This function adds a trigger function (fuction called when there is a collision) to the collider.
 */
void Collider::addTrigger(triggerFunc trigger) {
  triggers.push_back(trigger);
}

/* This function takes in a float (dt) representing the time since the last fixedUpdate
 * This function is intended to be run at a set interval and detects if the parent object has collided
 * with another object. If it has collided then it call the trigger functions which will react to the collision
 */
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

/*End Coliider-------------------------------------------------------*/


/*Begin CircleRender-------------------------------------------------------*/

/* This function is a constructor for a CircleRender. 
 * It takes in a GameObject (parent) and a double (radius). It saves radius as an object variable.
 * It then initializes the color the circle should render
 * Finally, it runs the parent constructor for Component (to complete setup).
 */
CircleRender::CircleRender( GameObject* parent, double radius ) :
radius ( radius ), Component(parent, string("CircleRender")) {
  setColor(0, 0, 1);
}

/* This function sets the color the circle should render as.  
 * It takes in 3 doubles, one for each color in RGB.
 * It then saves the individual RGB values as a color
 */
void CircleRender::setColor(float R, float G, float B) {
  color.R = R;
  color.G = G;
  color.B = B;
}

/* This function takes in a float (dt) representing the time since the last fixedUpdate
 * This function is intended to be run at a variable interval and when called, will render a circle
 */
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
/*End CircleRenderer-------------------------------------------------------*/

/*Begin Physics-------------------------------------------------------*/

/* This function is a constructor for a Physics componenent. 
 * It takes in a GameObject (parent) and 3 doubles (dx, dy, mass). It saves dx, dy, mass as an object variables.
 * Finally, it runs the parent constructor for Component (to complete setup).
 */
Physics::Physics ( GameObject* parent, double dx, double dy, double mass) :
mass ( mass ), dx ( dx ), dy ( dy ), Component (parent, string("Physics")) {};

/* This function takes in a float (dt) representing the time since the last fixedUpdate
 * This function updates the x and y position of the object based on its speed and time elepsed.
 */
void Physics::fixedUpdate( float dt ) {
  parent->x += dx * dt;
  parent->y += dy * dt;
};

/*End Physics------------------------------------------------------*/

/*Begin WalBounceScript-------------------------------------------------------*/

/* This function is a constructor for a WallBounceScript. 
 * It takes in a GameObject (parent) and a double (radius). It saves radius as an object variable.
 * It then runs the parent constructor for Component (to complete setup).
 */
WallBounceScript::WallBounceScript( GameObject* parent, double radius ) :
radius ( radius ), Component (parent, string( "WallBounceScript" )) {};

/* This function takes in a float (dt) representing the time since the last fixedUpdate
 * This function updates the x and y speeds in the Physics component of the GameObject to make objects bounce off walls.
 */
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
/*End WalBounceScript-------------------------------------------------------*/
