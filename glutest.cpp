#include "GL/freeglut.h"
#include "GL/gl.h"
#include <time.h>

/* display function - code from:
     http://fly.cc.fer.hr/~unreal/theredbook/chapter01.html
This is the actual usage of the OpenGL library. 
The following code is the same for any platform */
void renderFunction()
{
    static float offset = 0;
    static float increment = .0005;
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glBegin(GL_POLYGON);
        glVertex2f(-0.5 + offset, -0.5);
        glVertex2f(-0.5 + offset, 0.5);
        glVertex2f(0.5 + offset, 0.5);
        glVertex2f(0.5 + offset, -0.5);
    glEnd();
    offset += increment;
    if (offset >= .5 || offset <= -.5)
        increment = -increment;
    glFlush();
}

const float timeDelta = 1000/60; //refresh rate 1000/Hz

void gameLoop (){
    static float timeAccumulator = 0;
    renderFunction();
    glutPostRedisplay(); //calls gameLoop
}

/* Main method - main entry point of application
the freeglut library does the window creation work for us, 
regardless of the platform. */
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(500,500);
    glutInitWindowPosition(100,100);
    glutCreateWindow("OpenGL - First window demo");
    glutDisplayFunc(gameLoop);
    glutMainLoop();    
    return 0;
}