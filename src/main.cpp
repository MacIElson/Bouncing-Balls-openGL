#include "components.h"
#include "components.cpp"
#include "gameloop.cpp"

void printOnCollide(Collider* c1, Collider* c2){
  printf("collide");
}

void freezeTag(Collider* c1, Collider* c2){
  // Freeze both objects
  Physics* p1 = (Physics*) c1->parent->getComponent("Physics").front();
  Physics* p2 = (Physics*) c2->parent->getComponent("Physics").front();
  p1->dx = 0;
  p1->dy = 0;
  p2->dx = 0;
  p2->dy = 0;

  // Change their color to red
  CircleRender* cr1 = (CircleRender*) c1->parent->getComponent("CircleRender").front();
  CircleRender* cr2 = (CircleRender*) c2->parent->getComponent("CircleRender").front();
  cr1->setColor(1, 0, 0);
  cr2->setColor(1, 0, 0);
}

void createBall(double x, double y, double dx, double dy, double radius) {
  GameObject* obj = new GameObject(x, y);
  CircleRender* circleRender = new CircleRender(obj, radius);
  Physics* physics = new Physics(obj, dx, dy);
  Collider* collider = new Collider(obj, radius);
  WallBounceScript* wallBounceScript = new WallBounceScript(obj, radius);
  collider->addTrigger(freezeTag);
}

int main() {
  // Set up objects
  createBall(.5, .5, -.00025, .0005, .1);
  createBall(.75, .45, .00025, -.00005, .1);
  createBall(-.75, .45, -.0003, .00015, .1);
  createBall(0, 0, .0007, -.00005, .1);
  createBall(.6, -.45, .0003, -.0002, .05);
  createBall(-.35, -.45, .0003, -.0002, .05);
  createBall(-.2, -.9, -.0003, -.0002, .2);

  ODLGameLoop_initOpenGL();
}
