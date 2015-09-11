// assign1.cpp : Defines the entry point for the console application.
//

/*
  CSCI 480 Computer Graphics
  Assignment 1: Height Fields
  C++ starter code
*/
#include "stdafx.h"
#include <pic.h>
#include <windows.h>
#include <stdlib.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
int g_iMenuId;

int g_vMousePos[2] = { 0, 0 };
int g_iLeftMouseButton = 0;    /* 1 if pressed, 0 if not */
int g_iMiddleMouseButton = 0;
int g_iRightMouseButton = 0;

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROLSTATE;

CONTROLSTATE g_ControlState = ROTATE;

/* state of the world */
float g_vLandRotate[3] = { 0.0, 0.0, 0.0 };
float g_vLandTranslate[3] = { 0.0, 0.0, 0.0 };
float g_vLandScale[3] = { 1.0, 1.0, 1.0 };

/* see <your pic directory>/pic.h for type Pic */
Pic * g_pHeightData;

/* Write a screenshot to the specified filename */
void saveScreenshot(char *filename)
{
	int i, j;
	Pic *in = NULL;

	if (filename == NULL)
		return;

	/* Allocate a picture buffer */
	in = pic_alloc(640, 480, 3, NULL);

	printf("File to save to: %s\n", filename);

	for (i = 479; i >= 0; i--) {
		glReadPixels(0, 479 - i, 640, 1, GL_RGB, GL_UNSIGNED_BYTE,
			&in->pix[i*in->nx*in->bpp]);
	}

	if (jpeg_write(filename, in))
		printf("File saved Successfully\n");
	else
		printf("Error in Saving\n");

	pic_free(in);
}

void myinit()
{
	/* setup gl view here */
	glClearColor(0.0, 0.0, 0.0, 0.0);   // set background color
	glEnable(GL_DEPTH_TEST);            // enable depth buffering
	glShadeModel(GL_SMOOTH); // interpolate colors 


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1, .01, 1000);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity(); // reset transformation
	glTranslatef(g_pHeightData->ny/(float)2, g_pHeightData->nx/ (float)2, 300);
	glRotatef(0, 1, 0, 0);


	glScalef(0,0, 0);
	glPushMatrix();

}

void display()
{
	/* draw 1x1 cube about origin */
	/* replace this code with your height field implementation */
	/* you may also want to precede it with your
  rotation/translation/scaling */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity(); // reset transformation
	glTranslatef(g_vLandTranslate[0], g_vLandTranslate[1], g_vLandTranslate[2]);
	glRotatef(g_vLandRotate[0], 1, 0, 0);
	glRotatef(g_vLandRotate[1], 0, 1, 0);
	glRotatef(g_vLandRotate[2], 0, 0, 1);

	glScalef(g_vLandScale[0], g_vLandScale[1], g_vLandScale[2]);
	glPushMatrix();

	glMatrixMode(GL_MODELVIEW);

	//glBegin(GL_POLYGON);
	for (int i = 0; i < g_pHeightData->ny-1; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (int j = 0; j < g_pHeightData->nx; j++)
		{
			//(float)i / (float)g_pHeightData->ny

		unsigned char heightVal = PIC_PIXEL(g_pHeightData,j, i+1, 0);
		unsigned char heightVal2 = PIC_PIXEL(g_pHeightData, j, i, 0);

		glColor3f((float)heightVal / (float)255, (float)heightVal / (float)255, (float)heightVal/(float)255);
		glVertex3f((float)i / (float)g_pHeightData->ny, (float)j / (float)g_pHeightData->nx, (float)heightVal/(float)255);
		glColor3f((float)heightVal2 / (float)255, (float)heightVal2 / (float)255, (float)heightVal2/ (float)255);

		glVertex3f((float)(i +1)/ (float)g_pHeightData->ny, (float)j / (float)g_pHeightData->nx, (float)heightVal2 / (float)255);

		}
		glEnd();

	}
	//glEnd();
	glBegin(GL_LINES);
	glColor3f(1.0, 0,0 );
	glVertex3f(0,0,0);
	glColor3f(1.0, 0, 0);
	glVertex3f(0, 0, 100);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0, 1.0, 0);
	glVertex3f(0, 0, 0);
	glColor3f(1.0, 1.0, 1.0);
	glVertex3f(100, 0, 0);
	glEnd();
	
	glBegin(GL_LINES);
	glColor3f(0, 0, 1.0);
	glVertex3f(0, 0, 0);
	glColor3f(0, 0, 1.0);
	glVertex3f(0, 100, 0);
	glEnd();
	/*
	glBegin(GL_POLYGON);

  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(-0.5, -0.5, 0.0);
  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(-0.5, 0.5, 0.0);
  glColor3f(0.0, 0.0, 0.0);
  glVertex3f(0.5, 0.5, 0.0);
  glColor3f(1.0, 1.0, 0.0);
  glVertex3f(0.5, -0.5, 0.0);
  glEnd();
  */
  glutSwapBuffers();
}

void menufunc(int value)
{
  switch (value)
  {
    case 0:
      exit(0);
      break;
  }
}

void doIdle()
{
  /* do some stuff... */

  /* make the screen update */
	Sleep(1);//program ran really slow without using this; 
  glutPostRedisplay();
}

/* converts mouse drags into information about 
rotation/translation/scaling */
void mousedrag(int x, int y)
{
  int vMouseDelta[2] = {x-g_vMousePos[0], y-g_vMousePos[1]};
  
  switch (g_ControlState)
  {
    case TRANSLATE:  
      if (g_iLeftMouseButton)
      {
        g_vLandTranslate[0] += vMouseDelta[0]*0.01;
        g_vLandTranslate[1] -= vMouseDelta[1]*0.01;
      }
      if (g_iMiddleMouseButton)
      {
        g_vLandTranslate[2] += vMouseDelta[1]*0.01;
      }
      break;
    case ROTATE:
      if (g_iLeftMouseButton)
      {
        g_vLandRotate[0] += vMouseDelta[1];
        g_vLandRotate[1] += vMouseDelta[0];
      }
      if (g_iMiddleMouseButton)
      {
        g_vLandRotate[2] += vMouseDelta[1];
      }
      break;
    case SCALE:
      if (g_iLeftMouseButton)
      {
        g_vLandScale[0] *= 1.0+vMouseDelta[0]*0.01;
        g_vLandScale[1] *= 1.0-vMouseDelta[1]*0.01;
      }
      if (g_iMiddleMouseButton)
      {
        g_vLandScale[2] *= 1.0-vMouseDelta[1]*0.01;
      }
      break;
  }
  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

void mouseidle(int x, int y)
{
  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

void mousebutton(int button, int state, int x, int y)
{

  switch (button)
  {
    case GLUT_LEFT_BUTTON:
      g_iLeftMouseButton = (state==GLUT_DOWN);
      break;
    case GLUT_MIDDLE_BUTTON:
      g_iMiddleMouseButton = (state==GLUT_DOWN);
      break;
    case GLUT_RIGHT_BUTTON:
      g_iRightMouseButton = (state==GLUT_DOWN);
      break;
  }

  switch(glutGetModifiers())
  {
    case GLUT_ACTIVE_CTRL:
      g_ControlState = TRANSLATE;
      break;
    case GLUT_ACTIVE_SHIFT:
      g_ControlState = SCALE;
      break;
    default:
      g_ControlState = ROTATE;
      break;
  }

  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

int main(int argc, char* argv[])
{
	// I've set the argv[1] to spiral.jpg.
	// To change it, on the "Solution Explorer",
	// right click "assign1", choose "Properties",
	// go to "Configuration Properties", click "Debugging",
	// then type your texture name for the "Command Arguments"

	if (argc<2)
	{  
		printf ("usage: %s heightfield.jpg\n", argv[0]);
		exit(1);
	}
	g_pHeightData = jpeg_read((char*)argv[1], NULL);
	if (!g_pHeightData)
	{
		system("pause");

	    printf ("error reading %s.\n", argv[1]);

	    exit(1);
	}
	glutInit(&argc,(char**)argv);
  
	/*
		create a window here..should be double buffered and use depth testing
  
	    the code past here will segfault if you don't have a window set up....
	    replace the exit once you add those calls.
	*/
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);

	// set window size
	glutInitWindowSize(800, 800);

	// set window position
	glutInitWindowPosition(0, 0);

	// creates a window
	glutCreateWindow("Assign One");
  
	/* allow the user to quit using the right mouse button menu */
	g_iMenuId = glutCreateMenu(menufunc);
	glutSetMenu(g_iMenuId);
	glutAddMenuEntry("Quit",0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
  
	/* replace with any animate code */
	glutIdleFunc(doIdle);
	myinit();

	/* callback for mouse drags */
	glutMotionFunc(mousedrag);
	/* callback for idle mouse movement */
	glutPassiveMotionFunc(mouseidle);
	/* callback for mouse button changes */
	glutMouseFunc(mousebutton);



	/* do initialization */
	glutDisplayFunc(display);



	glutMainLoop();
	return 0;
}