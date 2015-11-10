#include <GL/freeglut.h>
#include <GL/gl.h>
#include <stdio.h>
#include <time.h>

// Set up bubbles
const int NUM_BUBBLES = 2;

// TODO: Refactor to a c++ class with an update method
typedef struct Transform {
    double x;
    double y;
    double dx;
    double dy;
} Transform;

Transform *makeTransform(double x, double y, double dx, double dy) {
    Transform *t = (Transform *) malloc(sizeof(Transform));
    t->x = x;
    t->y = y;
    t->dx = dx;
    t->dy = dy;
    return t;
}

void updateTransform(Transform *transform) {
    transform->x += transform->dx;
    transform->y += transform->dy;
}

// Transform bubbles[NUM_BUBBLES];

Transform *modelState = makeTransform(0, 0, .01, .01);


/* display function - code from:
     http://fly.cc.fer.hr/~unreal/theredbook/chapter01.html
This is the actual usage of the OpenGL library. 
The following code is the same for any platform */
void render(float fractionOfUpdate)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    glBegin(GL_POLYGON);
        glVertex2f(modelState->x-0.5, modelState->y -0.5);
        glVertex2f(modelState->x-0.5, modelState->y + 0.5);
        glVertex2f(modelState->x + 0.5, modelState->y + 0.5);
        glVertex2f(modelState->x + 0.5, modelState->y - 0.5);
    glEnd();

    glFlush();

    printf("%f\n", fractionOfUpdate);
}

long getTimeNS(){
    // Defined in the time.h library
    struct timespec currentTime;
    clock_gettime(CLOCK_REALTIME, &currentTime);
    return currentTime.tv_nsec;
}

void loopIteration() {
    // Define variables (on first run)
    static int const NS_PER_UPDATE = 1000000000 / 60;
    static long previous = getTimeNS();
    static long current;
    static long elapsed;
    static long lag = 0;

    // Update times
    current = getTimeNS();
    elapsed = current - previous;
    previous = current;
    lag += elapsed;

    // Run Model Updates
    while (lag >= NS_PER_UPDATE) {
        // TODO: Run Updates
        updateTransform(modelState);
        lag -= NS_PER_UPDATE;
    }

    // FIXME: This calculation is wrong!
    float fractionOfNextUpdate = - lag / NS_PER_UPDATE;
    render(fractionOfNextUpdate);

    // FIXME: This may cause a stack overflow eventually. Unsure.
    glutPostRedisplay();
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
    glutDisplayFunc(loopIteration);
    glutMainLoop();    
    return 0;
}