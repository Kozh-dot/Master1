//
//  tp3_squelette.cpp
//  
//
//  Created by Caroline Larboulette on 16/10/2017.
// gcc tp3_squelette.cpp -o tp3.exe -lglut -lGL -lGLU
//

//Those includes are the good ones for Mac OSX to use OpenGL and GLUT (The OpenGL Utility Toolkit) -- to make windows among other things.
#include <stdio.h>
#include <stdlib.h>

#ifdef __gnu_linux__
    #include <GL/freeglut.h>
    #include <GL/gl.h>
#else
    #include <OpenGL/gl.h>
    #include <GLUT/glut.h>
#endif
#include <math.h>       /* cos */

#define PI 3.14159265
int _width = 10;
int _height = 10;

/*
double R[16] = {
    0,0,0,0,
    0,0,0,0,
    0,0,1,0,
    0,0,0,1
};
double T[16] = {
    1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    0,0,0,1
};
double S[16] = {
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,1
};*/

double * translation(double x, double y,double z){
    double * T = new double[16];
    T[0] = 1;    T[1] = 0;    T[2] = 0;    T[3] = 0;
    T[4] = 0;    T[5] = 1;    T[6] = 0;    T[7] = 0;
    T[8] = 0;    T[9] = 0;    T[10] = 1;   T[11] = 0;
    T[12] = x;   T[13] = y;   T[14] = z;   T[15] = 1;
    return T;
}

double * scale(double x, double y,double z){
    double * S = new double[16];
    S[0] = x;    S[1] = 0;    S[2] = 0;    S[3] = 0;
    S[4] = 0;    S[5] = y;    S[6] = 0;    S[7] = 0;
    S[8] = 0;    S[9] = 0;    S[10] = z;   S[11] = 0;
    S[12] = 0;   S[13] = 0;   S[14] = 0;   S[15] = 1;
    return S;
}

double * rotation(double angle, double x, double y, double z){
    double * R = new double[16];
    R[0] = cos(angle*PI/180);    R[1] = sin(angle*PI/180);    R[2] = 0;    R[3] = 0;
    R[4] = -sin(angle*PI/180);    R[5] = cos(angle*PI/180);    R[6] = 0;    R[7] = 0;
    R[8] = 0;   R[9] = 0;    R[10] = 1;    R[11] = 0;
    R[12] = 0;    R[13] = 0;    R[14] = 0;    R[15] = 1;
    return R;
}

double * compose(double * M1, double * M2){
    double * C = new double[16];
    C[0] = M1[0]*M2[0] + M1[4]* M2[1] + M1[8]* M2[2] + M1[12] * M2[3];
    C[1] = M1[1]*M2[0] + M1[5]* M2[1] + M1[9]* M2[2] + M1[13] * M2[3];
    C[2] = M1[2]*M2[0] + M1[6]* M2[1] + M1[10]* M2[2] + M1[14] * M2[3];
    C[3] = M1[3]*M2[0] + M1[7]* M2[1] + M1[11]* M2[2] + M1[15] * M2[3];

    C[4] = M1[0]*M2[4] + M1[4]* M2[5] + M1[8]* M2[6] + M1[12] * M2[7];
    C[5] = M1[1]*M2[4] + M1[5]* M2[5] + M1[9]* M2[6] + M1[13] * M2[7];
    C[6] = M1[2]*M2[4] + M1[6]* M2[5] + M1[10]* M2[6] + M1[14] * M2[7];
    C[7] = M1[3]*M2[4] + M1[7]* M2[5] + M1[11]* M2[6] + M1[15] * M2[7];

    C[8] = M1[0]*M2[8] + M1[4]* M2[9] + M1[8]* M2[10] + M1[12] * M2[11];
    C[9] = M1[1]*M2[8] + M1[5]* M2[9] + M1[9]* M2[10] + M1[13] * M2[11];
    C[10] = M1[2]*M2[8] + M1[6]* M2[9] + M1[10]* M2[10] + M1[14] * M2[11];
    C[11] = M1[3]*M2[8] + M1[7]* M2[9] + M1[11]* M2[10] + M1[15] * M2[11];

    C[12] = M1[0]*M2[12] + M1[4]* M2[13] + M1[8]* M2[14] + M1[12] * M2[15];
    C[13] = M1[1]*M2[12] + M1[5]* M2[13] + M1[9]* M2[14] + M1[13] * M2[15];
    C[14] = M1[2]*M2[12] + M1[6]* M2[13] + M1[10]* M2[14] + M1[14] * M2[15];
    C[15] = M1[3]*M2[12] + M1[7]* M2[13] + M1[11]* M2[14] + M1[15] * M2[15];
    return C;
}

void draw(){
    glBegin(GL_POLYGON);
    glVertex3f(2,6,0);
    glVertex3f(2,3,0);
    glVertex3f(4,3,0);
    glVertex3f(4,6,0);
    glVertex3f(3,7,0);
    glVertex3f(2,6,0);
    glVertex3f(4,6,0);
    glEnd();
}

/**
 void display(void)
 Draw function to render the scene
 **/
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    glLoadIdentity();
    glColor3f(1.0, 0.0, 0.0);
    glLineWidth(2.0);
    glBegin(GL_LINES); //Draws x-axis
    glVertex3f(-_width,0,0);
    glVertex3f(_width,0,0);
    glEnd();

    glLoadIdentity();
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINES); //Draws y-axis
    glVertex3f(0,-_height,0);
    glVertex3f(0,_height,0);
    glEnd();

    glLoadIdentity();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    draw();
    /*-
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(2.0,2.0,1.0);
    glTranslatef(4, 0, 0);
    draw();
    //Rotate
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(0.0,0.0,1.0);
    glRotatef(100,0,0,1);
    draw();
    //Scale
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(1.0,0.0,0.0);
    glScalef(1.5,1.5,1);
    draw();*/
  
    glLoadIdentity();
    glColor3f(0.0,0.0,1.0);
    glLoadMatrixd(rotation(100,0,0,1));
    draw();

    glLoadIdentity();
    glLoadMatrixd(scale(1.5,1.5,1));
    glColor3f(1.0,0.0,0.0);
    draw();

    glLoadIdentity();
    glLoadMatrixd(translation(4, 0, 0));
    glColor3f(2.0,2.0,1.0);
    draw();
    glFlush();
}



void reshape(int w, int h)
{
    _width = w/2.; //Used to draw the axis with correct length
    _height = h/2.;
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho (-w/20., w/20., -h/20., h/20., -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW); //Ready to transform the object matrix
}

/**
 void init(void)
 Init function where you load your scene or create procedural objects
 **/
void init(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0); //Background color
    glMatrixMode(GL_PROJECTION); //Projection matrix
    glLoadIdentity();
    //left, right, bottom, top, near, far
    glOrtho(-5.0, 5.0, -5.0, 5.0, -1.0, 1.0);
    glPolygonMode(GL_FRONT, GL_LINE); //Polygons are outlined by default
    glMatrixMode(GL_MODELVIEW);

}

/**
 void keyboard(unsigned char key, int x, int y)
 key returns the character hit by the user
 **/
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'q': 
            exit(0); 
            break;  //exits the program
        case 'f': 
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
            display();
            break;
        case 'p': 
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); 
            display();
            break;
        case 'l': 
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
            display();
            break;
        }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize (250, 250); //Window size in pixels
    glutInitWindowPosition (100, 100); //Window position on screen in pixels
    glutCreateWindow ("Bonjour les amis !"); //Creates a window with the title of your choice^^
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMainLoop(); //Calls the rendering loop
    return 0;
}

