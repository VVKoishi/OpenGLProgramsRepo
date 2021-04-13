#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gl/glut.h>

//////////////////////////////////////////////////////////////////////////
class screenPt
{
private:
	GLint x, y;
public:
	screenPt() {
		x = y = 0;
	}
	void setCoords(GLint xCoordValue, GLint yCoordValue) {
		x = xCoordValue;
		y = yCoordValue;
	}
	GLint getx() const {
		return x;
	}
	GLint gety() const {
		return y;
	}
	void incrementx() {
		x ++;
	}
	void decrementy() {
		y --;
	}
};

void setPixel(GLint xCoord, GLint yCoord)
{
	glBegin(GL_POINTS);
	glVertex2i(xCoord, yCoord);
	glEnd();
}

void circleMidpoint(GLint xc, GLint yc, GLint radius)
{
	screenPt circPt;
	GLint p = 1 - radius;
	circPt.setCoords(0, radius);

	void circlePlotPoints(GLint, GLint, screenPt);
	circlePlotPoints(xc, yc, circPt);

	while (circPt.getx() < circPt.gety()) {
		circPt.incrementx();
		if (p < 0)
			p += 2 * circPt.getx() + 1;
		else {
			circPt.decrementy();
			p += 2 * (circPt.getx() - circPt.gety()) + 1;
		}
		circlePlotPoints(xc, yc, circPt);
	}
}

void circlePlotPoints(GLint xc, GLint yc, screenPt circPt)
{
	setPixel(xc + circPt.getx(), yc + circPt.gety());
	setPixel(xc - circPt.getx(), yc + circPt.gety());
	setPixel(xc + circPt.getx(), yc - circPt.gety());
	setPixel(xc - circPt.getx(), yc - circPt.gety());
	setPixel(xc + circPt.gety(), yc + circPt.getx());
	setPixel(xc - circPt.gety(), yc + circPt.getx());
	setPixel(xc + circPt.gety(), yc - circPt.getx());
	setPixel(xc - circPt.gety(), yc - circPt.getx());
}
//////////////////////////////////////////////////////////////////////////

void init(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
}

void myReshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w <= h)
		gluOrtho2D(-200.0, 200.0, -200.0*(GLfloat)h / (GLfloat)w, 200.0*(GLfloat)h / (GLfloat)w);
	else
		gluOrtho2D(-200.0*(GLfloat)w / (GLfloat)h, 200.0*(GLfloat)w / (GLfloat)h, -200.0, 200.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
}

void renderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0, 0, 0);
	circleMidpoint(0, 0, 50);

	glFlush();
}



int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(400, 400);
	glutCreateWindow("DrawCircle");

	init();
	glutDisplayFunc(renderScene);
	glutReshapeFunc(myReshape);
	glutMainLoop();
	return 0;
}