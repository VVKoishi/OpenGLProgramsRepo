#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gl/glut.h>

static GLfloat dx = 1.0,  dy = 1.0;

class screenPt
{
private:
	GLfloat x, y;
public:
	screenPt() {
		x = y = 0;
	}
	void setCoords(GLfloat xCoordValue, GLfloat yCoordValue) {
		x = xCoordValue;
		y = yCoordValue;
	}
	GLfloat getx() const {
		return x;
	}
	GLfloat gety() const {
		return y;
	}
	void incrementx() {
		//x ++;
		x += dx;
	}
	void decrementy() {
		//y --;
		y -= dy;
	}
};

void setPixelf(GLfloat xCoord, GLfloat yCoord)
{
	glBegin(GL_POINTS);
	glVertex2f(xCoord, yCoord);
	glEnd();
}

void circleMidpoint(GLfloat xc, GLfloat yc, GLfloat radius)
{
	screenPt circPt;
	GLfloat p = 1 - radius;
	circPt.setCoords(0, radius);

	void circlePlotPoints(GLfloat, GLfloat, screenPt);
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

void circlePlotPoints(GLfloat xc, GLfloat yc, screenPt circPt)
{
	setPixelf(xc + circPt.getx(), yc + circPt.gety());
	setPixelf(xc - circPt.getx(), yc + circPt.gety());
	setPixelf(xc + circPt.getx(), yc - circPt.gety());
	setPixelf(xc - circPt.getx(), yc - circPt.gety());
	setPixelf(xc + circPt.gety(), yc + circPt.getx());
	setPixelf(xc - circPt.gety(), yc + circPt.getx());
	setPixelf(xc + circPt.gety(), yc - circPt.getx());
	setPixelf(xc - circPt.gety(), yc - circPt.getx());
}

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

	dx = dy = w > h ? (1.0 * 400.0 / (GLfloat)w) : (1.0 * 400.0 / (GLfloat)h) ;

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

	// ���ֿ���ݣ�A.���������+ƽ��
	//�򿪿���ݹ���
    //1.������Ϲ���
    glEnable(GL_BLEND);
            
    //2.ָ���������
    //ע��:������޸��˻�Ϸ���ʽ,����ʹ�û�Ͽ���ݹ���ʱ,��һ��Ҫ��ΪĬ�ϻ�Ϸ���ʽ
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
    //3.�����Ե�\��\����εĿ���ݹ���
    glEnable(GL_POINT_SMOOTH);
    //glEnable(GL_LINE_SMOOTH);
    //glEnable(GL_POLYGON_SMOOTH);

	// B.���ز���MSA��������õ㻭��û��
	//glEnable(GLUT_MULTISAMPLE);

	init();
	glutDisplayFunc(renderScene);
	glutReshapeFunc(myReshape);
	glutMainLoop();
	return 0;
}