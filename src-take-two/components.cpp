#include "components.h"
#include <stdio.h>
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
list<Component*> Component::components; // Initialize a list of components within the Component namespace

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


// Testing
int main() {
  GameObject obj ( 1, 1 );
  if (obj.getComponent( "physics" ).empty()) printf( "Test Passed\n" );
  else printf ( "Test Failed" );

  // Instantiate a Component (adding it to obj), and attempt to retrieve it.
  Component comp ( &obj, "physics" );
  if (obj.getComponent( "physics" ).front() == &comp) printf( "Test Passed\n" );
  else printf ( "Test Failed" );
  
}
