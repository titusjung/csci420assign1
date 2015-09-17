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
#include <math.h> //for doing square roots to get norms
#include <chrono> //for getting milliseconds
#include <string>
int g_iMenuId;

int g_vMousePos[2] = { 0, 0 };
int g_iLeftMouseButton = 0;    /* 1 if pressed, 0 if not */
int g_iMiddleMouseButton = 0;
int g_iRightMouseButton = 0;
int framecount = 0;
float xl = 0.f;
float yl = 1.f;
float zl = 0.f;
typedef enum { ROTATE, TRANSLATE, SCALE } CONTROLSTATE;
bool recordstate;
int frames = 0;
GLuint listName;
std::chrono::time_point<std::chrono::high_resolution_clock> start, end, fstart, fend;

//typedef enum { POINT, LINES, TRIANGLES } RENDERSTATE; // render as cloud of points, as wireframe, or as triangle mesh

CONTROLSTATE g_ControlState = ROTATE;
//RENDERSTATE g_RenderState = TRIANGLES;

GLenum g_render;
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

void drawHeightMap()
{
	glEnable(GL_NORMALIZE);
	float a0, a1, a2, norm;
	listName = glGenLists(1);
	glNewList(listName, GL_COMPILE);
	for (int i = 0; i < g_pHeightData->ny - 1; i++)
	{
		glBegin(g_render);
		for (int j = 0; j < g_pHeightData->nx - 1; j++)
		{
			unsigned char heightVal = PIC_PIXEL(g_pHeightData, j, i, 0);
			unsigned char heightVal2 = PIC_PIXEL(g_pHeightData, j, i + 1, 0);
			unsigned char heightVal3 = PIC_PIXEL(g_pHeightData, j + 1, i + 1, 0);

			a0 = -(heightVal2 - heightVal);
			a1 = (heightVal3 - heightVal);
			a2 = (float)1;
			norm = sqrt(a0*a0 + a1*a1 + a2*a2);
			a0 = a0 / norm;
			a1 = a1 / norm;
			a2 = a2 / norm;
			glNormal3f(a0, a1, a2);
			glColor3f(1 - (float)heightVal / (float)255, 1 - (float)heightVal / (float)255, 1.0);
			glVertex3f((float)j - (float)g_pHeightData->nx / (float)2.0, (float)i - (float)g_pHeightData->ny / (float)2.0, (float)heightVal - 255);


			glColor3f(1 - (float)heightVal2 / (float)255, 1 - (float)heightVal2 / (float)255, 1.0);
			glVertex3f((float)j - (float)g_pHeightData->nx / (float) 2.0, (float)(i + 1) - (float)g_pHeightData->ny / (float)2.0, (float)heightVal2 - 255);

			//glColor3f(1 - (float)heightVal3 / (float)255, 1 - (float)heightVal3 / (float)255, 1.0);
			//glVertex3f((float)(j+1) - (float)g_pHeightData->nx / (float) 2.0, (float)(i+1) - (float)g_pHeightData->ny / (float)2.0, (float)heightVal3 - 255);

		}
		glEnd();

	}
	glBegin(GL_LINES);
	glColor3f(1.0, 0, 0);
	glVertex3f(0, 0, 0);
	glColor3f(1.0, 0, 0);
	glVertex3f(0, 0, 100);
	glEnd();

	//green x axis line
	glBegin(GL_LINES);
	glColor3f(0, 1.0, 0);
	glVertex3f(0, 0, 0);
	glColor3f(1.0, 1.0, 1.0);
	glVertex3f(100, 0, 0);
	glEnd();

	//blue y axis line
	glBegin(GL_LINES);
	glColor3f(0, 0, 1.0);
	glVertex3f(0, 0, 0);
	glColor3f(0, 0, 1.0);
	glVertex3f(0, 100, 0);
	glEnd();
	glEndList();
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
	drawHeightMap();

}

void reshape(int w, int h)
{
	// setup image size
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// setup camera
	gluPerspective(60, 1, .01, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}



void record()
{
	if (recordstate &frames<300)
	{
		frames++;
		end = std::chrono::high_resolution_clock::now();
		char myFilenm[2048];
		std::chrono::duration<double> elapsed_seconds = end - start;
		if (elapsed_seconds.count() >= 1.0 / 30.0) // does a capture every 1/30 of a second for 30 fps
		{
			start = std::chrono::high_resolution_clock::now();

			sprintf_s(myFilenm, "anim.%04d.jpg", frames);
			saveScreenshot(myFilenm);

		}
	}
}
void measurefps()
{
	framecount++;
	fend = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed_seconds = fend - fstart;
	if (elapsed_seconds.count() >= 1.0)
	{
		printf("fps is %d \n", framecount);
		framecount = 0;
		fstart = std::chrono::high_resolution_clock::now();

	}
}
void display()
{
	/* draw 1x1 cube about origin */
	/* replace this code with your height field implementation */
	/* you may also want to precede it with your
  rotation/translation/scaling */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity(); // reset transformation
	
	glTranslatef(-g_vLandTranslate[0],-g_vLandTranslate[1],-g_vLandTranslate[2]);
	glRotatef(g_vLandRotate[0], 1, 0, 0);
	glRotatef(g_vLandRotate[1], 0, 1, 0);
	glRotatef(g_vLandRotate[2], 0, 0, 1);
	glScalef(g_vLandScale[0], g_vLandScale[1], g_vLandScale[2]);

	glCallList(listName);//draws height map based on projection state
	glPopMatrix();
	//glEnd();
	//global reference axis to help me navigate
	//red z axis line


	//measurefps();
	//record();
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
	//Sleep(1);//program ran really slow without using this; 
	//record();
	measurefps();

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
        g_vLandTranslate[0] += vMouseDelta[0]*0.1;
        g_vLandTranslate[1] -= vMouseDelta[1]*0.1;
      }
      if (g_iMiddleMouseButton)
      {
        g_vLandTranslate[2] += vMouseDelta[1]*0.1;
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
void startrecord()
{
	recordstate = true;
	start = std::chrono::high_resolution_clock::now();
	frames = 0;
}

void keyboardbuttons(unsigned char c, int x, int y)
{
	switch (glutGetModifiers())
	{
		case GLUT_ACTIVE_ALT:
		{
			switch (c)
			{
				case 'p':
				{
					 g_render = GL_POINTS;
					 drawHeightMap();
					break;
				}
				case 'l':
				{
					g_render = GL_LINES;
					drawHeightMap();
					break;
				}
				case 't':
				{
					g_render = GL_TRIANGLE_STRIP;

					 drawHeightMap();
					break;
				}
				case 'h':
				{

					g_vLandRotate[0] = 0;
					g_vLandRotate[1] = 0;
					g_vLandRotate[2] = 0;

					g_vLandTranslate[0] = 0;
					g_vLandTranslate[1] = 0;
					g_vLandTranslate[2] = 0;

					g_vLandScale[0] = 1.0;
					g_vLandScale[1] = 1.0;
					 g_vLandScale[2] = 1.0;

					break;
				}
				case 'a':
					startrecord();
					break;
			}
			
			break;
		}
	}
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
  
	recordstate = false;
	fstart = std::chrono::high_resolution_clock::now();

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

	glutKeyboardFunc(keyboardbuttons);
	glutReshapeFunc(reshape);

	/* do initialization */
	glutDisplayFunc(display);



	glutMainLoop();
	return 0;
}