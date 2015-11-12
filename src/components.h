#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <list>
using namespace std;

class Transform {
  public:
    Transform(double x, double y);
    double getX( void );
    double getY( void );
    double x;
    double y;
};

class Physics {
  public:
    Physics ( Transform *transform, double dx = 0, double dy =0);
    static void updateAll( void );
    void update( void );

  protected:
    static list<Physics*> updateList;
    Transform *transform;
    double dx;
    double dy;
};

typedef void ( * renderFunc ) ( Transform* t );

class Renderable {
  public:
    Renderable (Transform* transform, renderFunc renderFunc);
    static void renderAll( void );
    Transform* transform;
    renderFunc render;

  protected:
    static list<Renderable*> renderList;
};

#endif