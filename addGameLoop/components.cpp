#include "components.h"
using namespace std;

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


// -- TESTING SCRIPT -- //
#include <stdio.h>
int main ()  {
  // Create a bubble object at Position (0, 0)
  Transform testBubble(0, 0);

  // Attach physics to the bubble w. initial velocity of (0, 10)
  Physics p(&testBubble, 0, 10);

  // Attach a renderer to the bubble that displays the current position of the bubble
  auto dispCoords = [](Transform* t) { printf("Position: (%f, %f)\n", t->x, t->y); };
  Renderable r(&testBubble, dispCoords);

  // Inital Coordinate
  Renderable::renderAll();

  // After 1 update
  Physics::updateAll();
  Renderable::renderAll();

  // After 2 updates
  Physics::updateAll();
  Renderable::renderAll();
}
