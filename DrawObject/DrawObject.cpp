#include <GL/glut.h>

// Global settings
bool drawLines = false;
float mynear = 3.0;  	/* near clipping plane in eye coords */
float myfar = 7.0;    	/* far clipping plane in eye coords */
float viewxform_z = -5.0;

/*
** Set initial state.
*/
void init(void)
{
	//glLineWidth(3.0);
	//glShadeModel(GL_FLAT);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST); /* Enable hidden--surface--removal */
}

/*
** Draw the cube.
*/
GLfloat vertices[8][3] = {
	{-1.0,-1.0,-1.0}, {1.0,-1.0,-1.0}, {1.0,1.0,-1.0}, {-1.0,1.0,-1.0},
	{-1.0,-1.0,1.0}, {1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0}
};

GLfloat colors[8][3] = {
	{0.0,0.0,0.0}, {1.0,0.0,0.0}, {1.0,1.0,0.0}, {0.0,1.0,0.0},
	{0.0,0.0,1.0}, {1.0,0.0,1.0}, {1.0,1.0,1.0}, {0.0,1.0,1.0}
};

void polygon(int a, int b, int c, int d)
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

void drawcube(void)
{
	/* map vertices to faces */

	polygon(1, 0, 3, 2);
	polygon(3, 7, 6, 2);
	polygon(7, 3, 0, 4);
	polygon(2, 6, 5, 1);
	polygon(4, 5, 6, 7);
	polygon(5, 4, 0, 1);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* view transform */
	glLoadIdentity();
	glTranslatef(0.0, 0.0, viewxform_z);

	drawcube();

	glutSwapBuffers();
	glutPostRedisplay();
}

void myReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//glOrtho(-5.0, 5.0, -5.0, 5.0, mynear, myfar);
	glFrustum(-2.5, 2.5, -2.5, 2.5, mynear, myfar);
	//viewxform_z = -5.0;
	
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