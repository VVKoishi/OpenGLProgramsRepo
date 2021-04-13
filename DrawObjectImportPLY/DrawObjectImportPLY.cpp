#include "PLYImporter.h"

// Global settings
bool drawLines = 0;
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

void triangle(Vertex* vertexPointer, int a, int b, int c)
{

	/* draw a polygon via list of vertices */

	if (drawLines) {
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINE_LOOP);
			glVertex3f(vertexPointer[a].x, vertexPointer[a].y, vertexPointer[a].z);
			glVertex3f(vertexPointer[b].x, vertexPointer[b].y, vertexPointer[b].z);
			glVertex3f(vertexPointer[c].x, vertexPointer[c].y, vertexPointer[c].z);
		glEnd();
	}
	else {
		glBegin(GL_TRIANGLES);

		GLfloat Vab[3] = {
			vertexPointer[b].x - vertexPointer[a].x,
			vertexPointer[b].y - vertexPointer[a].y,
			vertexPointer[b].z - vertexPointer[a].z
		};
		GLfloat Vac[3] = {
			vertexPointer[c].x - vertexPointer[a].x,
			vertexPointer[c].y - vertexPointer[a].y,
			vertexPointer[c].z - vertexPointer[a].z
		};

		glNormal3f(
			Vab[1] * Vac[2] - Vab[2] * Vac[1],
			Vab[2] * Vac[0] - Vab[0] * Vac[2],
			Vab[0] * Vac[1] - Vab[1] * Vac[0]
		);
		
		//glColor3f(vertexPointer[a].r, vertexPointer[a].g, vertexPointer[a].b);
		glVertex3f(vertexPointer[a].x, vertexPointer[a].y, vertexPointer[a].z);
		
		//glColor3f(vertexPointer[b].r, vertexPointer[b].g, vertexPointer[b].b);
		glVertex3f(vertexPointer[b].x, vertexPointer[b].y, vertexPointer[b].z);
		
		//glColor3f(vertexPointer[c].r, vertexPointer[c].g, vertexPointer[c].b);
		glVertex3f(vertexPointer[c].x, vertexPointer[c].y, vertexPointer[c].z);

		glEnd();
	}
}

void drawbunny()
{
	static PLYImporter bunny("./bunny/reconstruction/bun_zipper.ply");

	Vertex* vertexPointer = bunny.GetVertexPointer();
	Face* facePointer = bunny.GetFacePointer();
	for (int i = 0; i < bunny.GetFaceNum(); i++)
	{
		triangle(vertexPointer, facePointer[i].a, facePointer[i].b, facePointer[i].c);
	}

	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);
	//glVertexPointer(3, GL_FLOAT, 6*sizeof(GL_FLOAT), bunny.GetVertexPointer());
	//glColorPointer(3, GL_FLOAT, 6*sizeof(GL_FLOAT), bunny.GetVertexPointer() + 3*sizeof(GL_FLOAT));
	//glDrawElements(GL_TRIANGLES, bunny.GetFaceNum() * 3, GL_UNSIGNED_INT, bunny.GetIndices());

	////typedef GLint vertex3 [3], color3 [3];

	////vertex3 pt [8] = { {0, 0, 0}, {0, 1, 0}, {1, 0, 0},
	////	{1, 1, 0}, {0, 0, 1}, {0, 1, 1}, {1, 0, 1}, {1, 1, 1} };
	////color3 hue [8] = { {1, 0, 0}, {1, 0, 0}, {0, 0, 1},
	////	{0, 0, 1}, {1, 0, 0}, {1, 0, 0}, {0, 0, 1}, {0, 0, 1} };

	////glVertexPointer (3, GL_INT, 0, pt);
	////glColorPointer (3, GL_INT, 0, hue);
	////static GLubyte vertIndex[] = { 6, 2, 3, 7, 5, 1, 0, 4, 7, 3, 1, 5,
	////	 4, 0, 2, 6, 2, 0, 1, 3, 7, 5, 4, 6 };

	////glDrawElements (GL_QUADS, 24, GL_UNSIGNED_BYTE, vertIndex);
	////glDrawArrays(GL_TRIANGLES, 0, 24);


	//glDisableClientState (GL_VERTEX_ARRAY);
	//glDisableClientState (GL_COLOR_ARRAY);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* view transform */
	glLoadIdentity();
	glTranslatef(0.0, -2, -5);
	glScalef(15, 15, 15);


	/* Enable a single OpenGL light. */
	glEnable(GL_NORMALIZE);
	GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	//glutSolidTeapot(1.0);

	//drawcube();
	drawbunny();

	glutSwapBuffers();
	//glutPostRedisplay();
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