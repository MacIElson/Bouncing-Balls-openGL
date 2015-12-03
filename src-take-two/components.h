#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <list>
#include <string>
using namespace std;

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
  	const GameObject* parent;
};

#endif