#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <list>
#include <string>
#include <GL/glut.h>

using namespace std;

//g++ -std=c++11 components.cpp -o components && ./components

class Component; // Forward declaration to resolve circular dependency
class GameObject {
  friend class Component;
  private:
  	list<Component*> componentList;
  	void addComponent( Component* c );

  public:
  	GameObject( double x, double y );
  	list<Component*> getComponent( string type );	// Returns a list of all components matching the type parameter
  	double x;
  	double y;
};

class Component {
	private:
  	static list<Component*> components;

  public:
  	Component( GameObject* parent, string type ); // Similar as an init method: In this case, initilizes the type and parent fields, and adds self to the static components list.
  	static void updateAll( float dt );  // Run all variable updates (eg renderAll)
  	static void fixedUpdateAll( float dt ); // Updates on fixed interval (eg physicsUpdateAll)
  	virtual void update( float dt );
  	virtual void fixedUpdate( float dt );
  	const string type;
  	GameObject* const parent; // Declare the pointer constant without making the data constant
};

struct color_t {
  float R;
  float G;
  float B;
};

class CircleRender : public Component {
  private:
    color_t color;

  public:
    CircleRender( GameObject* parent, double radius);
    void setColor(float R, float G, float B);
    double radius;
    void update( float dt );
};

class Physics : public Component {
  public:
    Physics ( GameObject* parent, double dx = 0, double dy = 0, double mass = 1 );
    void fixedUpdate( float dt );
    double dx;
    double dy;
    double mass;
};

class WallBounceScript : public Component {
  public:
    WallBounceScript( GameObject* parent, double radius );
    void fixedUpdate( float dt );
    double radius;
};

#endif
