#include "OBJImporter.h"

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
	//glShadeModel(GL_SMOOTH);
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

void triangle(OBJImporter& obj, Face& f)
{
	auto& vertexList = obj.vertexList;
	auto& normalList = obj.normalList;
	auto& materialTable = obj.materialTable;
	if (drawLines) {
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINE_LOOP);
			glVertex3f(vertexList[f.a].x, vertexList[f.a].y, vertexList[f.a].z);
			glVertex3f(vertexList[f.b].x, vertexList[f.b].y, vertexList[f.b].z);
			glVertex3f(vertexList[f.c].x, vertexList[f.c].y, vertexList[f.c].z);
		glEnd();
	}
	else {
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialTable[f.mtl_name].ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialTable[f.mtl_name].diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialTable[f.mtl_name].specular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialTable[f.mtl_name].shininess);

		glBegin(GL_TRIANGLES);

		//glNormal3f(normalList[f.na].x, normalList[f.na].y, normalList[f.na].z);
		glNormal3f(normalList[f.a].x, normalList[f.a].y, normalList[f.a].z);
		glVertex3f(vertexList[f.a].x, vertexList[f.a].y, vertexList[f.a].z);
		
		//glNormal3f(normalList[f.nb].x, normalList[f.nb].y, normalList[f.nb].z);
		glNormal3f(normalList[f.b].x, normalList[f.b].y, normalList[f.b].z);
		glVertex3f(vertexList[f.b].x, vertexList[f.b].y, vertexList[f.b].z);

		//glNormal3f(normalList[f.nc].x, normalList[f.nc].y, normalList[f.nc].z);
		glNormal3f(normalList[f.c].x, normalList[f.c].y, normalList[f.c].z);
		glVertex3f(vertexList[f.c].x, vertexList[f.c].y, vertexList[f.c].z);

		glEnd();
	}
}

void drawteapot()
{
	static OBJImporter teapot("./newell_teaset/porsche.obj");

	for (auto& f : teapot.faceList)
	{
		triangle(teapot, f);
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* view transform */
	glLoadIdentity();
	glTranslatef(0.0, 0.0, viewxform_z);

	/* Enable a single OpenGL light. */
	glEnable(GL_NORMALIZE);
	static const GLfloat light_position[] = { 0, 0, 1, 0 };
	//static const GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1 };
	//static const GLfloat light_diffuse[] = { 0.5, 0.5, 0.5, 1 };
	//static const GLfloat light_specular[] = { 1, 1, 1, 1 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	//glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	//glTranslatef(0, -2, 0);
	glRotatef(60, 0, 1, 0);
	glScalef(0.04, 0.04, 0.04);

	//glutSolidSphere(1.0, 100, 100);

	//drawcube();
	drawteapot();

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