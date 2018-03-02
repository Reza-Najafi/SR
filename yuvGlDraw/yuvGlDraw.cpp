// yuvGlDraw.cpp : Defines the entry point for the console application.
//
#include <windows.h>
#include "stdafx.h"
#include <GL/glut.h> 

int imageWidth;
int imageHeight;
unsigned char*** imageData;
void init()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    //makeCheckImage();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

}
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glRasterPos2i(0, 0);
    glDrawPixels(imageWidth, imageHeight, GL_RGB,
        GL_UNSIGNED_BYTE, imageData);
    glFlush();
}

void draw(int argc, char** argv) {
    glutInit(&argc, argv);                 // Initialize GLUT
    glutCreateWindow("OpenGL Setup Test"); // Create a window with the given title
    glutInitWindowSize(imageWidth +40, imageHeight +40);   // Set the window's initial width & height
    glutInitWindowPosition(10, 10); // Position the window's initial top-left corner
    glutDisplayFunc(display); // Register display callback handler for window re-paint
    glutMainLoop();           // Enter the infinitely event-processing loop
    return ;
}


