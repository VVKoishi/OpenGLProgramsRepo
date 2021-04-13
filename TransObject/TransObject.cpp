#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <windows.h>

// Global settings
bool drawLines = false;
float mynear = 3.0;  	/* near clipping plane in eye coords */
float myfar = 7.0;    	/* far clipping plane in eye coords */
float viewxform_z = -5.0;
float time = 0.0;
float deltaTime = 0.01;

/*
** Set initial state.
*/
void init(void)
{
	glLineWidth(3.0);
	glEnable(GL_DEPTH_TEST); /* Enable hidden--surface--removal */
}


/*
** Draw the cube.
*/
GLfloat vertices[][3] = {
	{-1.0,-1.0,-1.0}, {1.0,-1.0,-1.0}, {1.0,1.0,-1.0}, {-1.0,1.0,-1.0},
	{-1.0,-1.0,1.0}, {1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0}
};

GLfloat colors[][3] = {
	{0.0,0.0,0.0}, {1.0,0.0,0.0}, {1.0,1.0,0.0}, {0.0,1.0,0.0},
	{0.0,0.0,1.0}, {1.0,0.0,1.0}, {1.0,1.0,1.0}, {0.0,1.0,1.0}
};

GLfloat objectXform[4][4] = {
	{1.0, 0.0, 0.0, 0.0},
	{0.0, 1.0, 0.0, 0.0},
	{0.0, 0.0, 1.0, 0.0},
	{0.0, 0.0, 0.0, 1.0}
};


void polygon(int a, int b, int c, int d, int face)
{

	/* draw a polygon via list of vertices */

	if (drawLines) {
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINE_LOOP);
		glVertex3fv(vertices[a]);
		glVertex3fv(vertices[b]);
		glVertex3fv(vertices[c]);
		glVertex3fv(vertices[d]);
		glEnd();
	}
	else {
		glBegin(GL_POLYGON);
		
		glColor3fv(colors[a]);
		glVertex3fv(vertices[a]);
		
		glColor3fv(colors[b]);
		glVertex3fv(vertices[b]);
		
		glColor3fv(colors[c]);
		glVertex3fv(vertices[c]);
		
		glColor3fv(colors[d]);
		glVertex3fv(vertices[d]);

		glEnd();
	}
}

void colorcube(void)
{
	/* map vertices to faces */

	polygon(1, 0, 3, 2, 0);
	polygon(3, 7, 6, 2, 1);
	polygon(7, 3, 0, 4, 2);
	polygon(2, 6, 5, 1, 3);
	polygon(4, 5, 6, 7, 4);
	polygon(5, 4, 0, 1, 5);
}



void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* view transform */
	glLoadIdentity();
	glTranslatef(0.0, 0.0, viewxform_z);

	//////////////////////////////////////////////////////////////////////////
	// Æ½ÒÆ
	glPushMatrix();
	glLoadIdentity();
	GLfloat deltaHeight = (GLfloat) ( sin(time + deltaTime) - sin(time) ) * 2;
	glTranslatef(0.0, deltaHeight, 0.0);
	glMultMatrixf((GLfloat *)objectXform);
	glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *)objectXform);
	glPopMatrix();

	// Ðý×ª
	glPushMatrix();
	glLoadIdentity();
	glRotatef(1.0, -0.1, 1.0, 0.1);
	glMultMatrixf((GLfloat *)objectXform);
	glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *)objectXform);
	glPopMatrix();
	
	// Ëõ·Å
	glPushMatrix();
	glLoadIdentity();
	GLfloat deltaScale = (GLfloat) ( sin(time + deltaTime) - sin(time) ) * 0.1 + 1.0;
	glScalef(deltaScale, deltaScale, deltaScale);
	glMultMatrixf((GLfloat *)objectXform);
	glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *)objectXform);
	glPopMatrix();
	//////////////////////////////////////////////////////////////////////////
	glPushMatrix();
	glMultMatrixf((GLfloat *)objectXform);
	colorcube();
	glPopMatrix();

	glFlush();
	glutSwapBuffers();

	Sleep(deltaTime * 1000);
	time += deltaTime;
	glutPostRedisplay();
}

void myReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-5.0, 5.0, -5.0, 5.0, mynear, myfar);
	//glFrustum(-2.5, 2.5, -2.5, 2.5, mynear, myfar);
	viewxform_z = -5.0;
	
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1000, 1000);
	glutCreateWindow("Example");

	init();
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}