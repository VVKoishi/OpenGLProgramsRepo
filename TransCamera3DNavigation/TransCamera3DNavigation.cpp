#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <windows.h>
#define M_PI       3.14159265358979323846   // pi

// Global settings
enum TransMode {
	Translate,
	Rotate,
	Scale
};
TransMode transModeSelected = Rotate;
float angle = 0.0, axis[3];
int lastX, lastY;
int winWidth, winHeight;

bool drawLines = false;
float mynear = 3.0;  	/* near clipping plane in eye coords */
float myfar = 20.0;    	/* far clipping plane in eye coords */
float viewxform_z = -5.0;

GLfloat xview = 0.0, yview = 0.0, zview = 0.0;	//  Viewing-coordinate origin.
GLfloat xref = 0.0, yref = 0.0, zref = -1.0;	//  Look-at point.
GLfloat Vx = 0.0, Vy = 1.0, Vz = 0.0;			//  View-up vector.

GLfloat VRightx = (yref - yview) * Vz - (zref - zview) * Vy;
GLfloat VRighty = (zref - zview) * Vx - (xref - xview) * Vz;
GLfloat VRightz = (xref - xview) * Vy - (yref - yview) * Vx;

bool rotateCamera = false;

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
	glShadeModel(GL_FLAT);

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

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glFrustum(-2.5, 2.5, -2.5, 2.5, mynear, myfar);

	glMatrixMode(GL_MODELVIEW);

	/* view transform */
	glLoadIdentity();

	gluLookAt(xview, yview, zview, xref, yref, zref, Vx, Vy, Vz);

	glTranslatef(0.0, 0.0, viewxform_z);

	//////////////////////////////////////////////////////////////////////////
	if (transModeSelected == Translate)
	{
		// 平移
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(axis[0], axis[1], axis[2]);
		glMultMatrixf((GLfloat *)objectXform);
		glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *)objectXform);
		glPopMatrix();
	} 
	else if (transModeSelected == Rotate)
	{
		// 旋转
		glPushMatrix();
		glLoadIdentity();
		glRotatef(angle, axis[0], axis[1], axis[2]);
		glMultMatrixf((GLfloat *)objectXform);
		glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *)objectXform);
		glPopMatrix();
	}
	else if (transModeSelected == Scale)
	{
		// 缩放
		glPushMatrix();
		glLoadIdentity();
		glScalef(axis[0] + 1, axis[1] + 1, axis[2] + 1);
		glMultMatrixf((GLfloat *)objectXform);
		glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *)objectXform);
		glPopMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	glPushMatrix();
	glMultMatrixf((GLfloat *)objectXform);
	colorcube();
	glPopMatrix();

	// 底部平面参考
	glPushMatrix();
	glTranslatef(0.0, -1.0, 0.0);
	glScalef(5.0, 0.1, 5.0);
	glColor3f(0.5, 0.5, 0.5);
	glutSolidCube(1.0);
	glPopMatrix();

	glFlush();
	glutSwapBuffers();
}

void myReshape(int w, int h)
{
	winWidth = w;
	winHeight = h;

	glViewport(0, 0, w, h);
}

//////////////////////////////////////////////////////////////////////////
void startMotion(int x, int y)
{
	lastX = x;
	lastY = y;
}

void stopMotion(int x, int y)
{
	angle = 0.0f;
	memset(axis, 0, sizeof(axis));
	if(!rotateCamera) {
		printf("objectXform:\n");
		for (int i = 0; i < 4; i++)
		{
			printf("\t");
			for (int j = 0; j < 4; j++)
			{
				printf("%.2f\t", objectXform[i][j]);
			}
			printf("\n");
		}
	}
	else {
		// 保持镜头不倾斜
		printf("VLocalX & WorldY should be vertical:\n");
		printf("\t VLcoalX . WorldY = %.2f \n", VRightx * 0.0 + VRighty * 1.0 + VRightz * 0.0);
	}
}

void mouseButton(int button, int state, int x, int y)
{
	switch (button) {
	case GLUT_LEFT_BUTTON:
		rotateCamera = false;
		break;
	case GLUT_RIGHT_BUTTON:
		rotateCamera = true;
		break;
	default:
		return;
	}
	switch (state) {
	case GLUT_DOWN:
		startMotion(x, y);
		break;
	case GLUT_UP:
		stopMotion(x, y);
		break;
	}
}

void quatRotate(
	GLfloat theta, GLfloat ux, GLfloat uy, GLfloat uz, 
	GLfloat& px, GLfloat& py, GLfloat& pz
)
{
	GLfloat inpx = px;
	GLfloat inpy = py;
	GLfloat inpz = pz;
	px = 
		( ux * ux * (1 - cos(theta)) + cos(theta) ) * inpx +
		( ux * uy * (1 - cos(theta)) - uz * sin(theta) ) * inpy +
		( ux * uz * (1 - cos(theta)) + uy * sin(theta) ) * inpz ;
	py =
		( uy * ux * (1 - cos(theta)) + uz * sin(theta) ) * inpx +
		( uy * uy * (1 - cos(theta)) + cos(theta) ) * inpy +
		( uy * uz * (1 - cos(theta)) - ux * sin(theta) ) * inpz ;
	pz = 
		( uz * ux * (1 - cos(theta)) - uy * sin(theta) ) * inpx +
		( uz * uy * (1 - cos(theta)) + ux * sin(theta) ) * inpy +
		( uz * uz * (1 - cos(theta)) + cos(theta) ) * inpz ;
}

void mouseMotion(int x, int y)
{
	float dx = x - lastX;
	float dy = y - lastY;
	if (!dx && !dy) return;

	if (rotateCamera) {
		// 以参考点为中心，绕局部X轴旋转俯仰角，绕平行世界Y轴旋转环视；
		// 左乘局部向右向量(cross(v,vup))旋转，左乘世界Y轴向量(0,1,0)旋转
		VRightx = (yref - yview) * Vz - (zref - zview) * Vy;
		VRighty = (zref - zview) * Vx - (xref - xview) * Vz;
		VRightz = (xref - xview) * Vy - (yref - yview) * Vx;
		// 四元数旋转，旋转角由dx和dy确定
		GLfloat thetaX = -dy * (M_PI / winHeight);
		GLfloat thetaY = -dx * (M_PI / winWidth);
		GLfloat px = xref - xview;
		GLfloat py = yref - yview;
		GLfloat pz = zref - zview;
		quatRotate(thetaX, VRightx, VRighty, VRightz, px, py, pz);
		quatRotate(thetaY, 0.0, 1.0, 0.0, px, py, pz);

		xref = xview + px;
		yref = yview + py;
		zref = zview + pz;

		// 计算vup轴
		quatRotate(thetaX, VRightx, VRighty, VRightz, Vx, Vy, Vz);
		quatRotate(thetaY, 0.0, 1.0, 0.0, Vx, Vy, Vz);
	}
	else {
		if (transModeSelected == Translate) {
			axis[0] = dx * (10.0f/winWidth);
			axis[1] = -dy * (10.0f/winHeight);
			axis[2] = 0;
		}
		else if (transModeSelected == Rotate) {
			angle = 3.0;
			axis[0] = dy * (360.0f/winHeight);
			axis[1] = dx * (360.0f/winWidth);
			axis[2] = 0;
		}
		else if (transModeSelected == Scale) {
			axis[0] = dx * (4.0f/winWidth);
			axis[1] = -dy * (4.0f/winHeight);
			axis[2] = 0;	// 在调用时会 +1
		}
	}

	lastX = x;
	lastY = y;
	glutPostRedisplay();
}

void userEventAction(int key) {
	switch (key) {
	case '0':
		angle = 0.0f;
		memset(axis, 0, sizeof(axis));
		memset(objectXform, 0, sizeof(objectXform));
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (i == j) {
					objectXform[i][j] = 1.0f;
				}
			}
		}
		xview = 0.0; yview = 0.0; zview = 0.0;	//  Viewing-coordinate origin.
		xref = 0.0; yref = 0.0; zref = -1.0;	//  Look-at point.
		Vx = 0.0; Vy = 1.0; Vz = 0.0;			//  View-up vector.
		break;
	case 'r':		// 旋转
		transModeSelected = Rotate;
		break;
	case 't':		// 平移
		transModeSelected = Translate;
		break;
	case 'y':		// 缩放
		transModeSelected = Scale;
		break;
	case '1':		// 线框图/多边形
		drawLines = !drawLines;
		break;
	case '2':		// 正视图
		xview = 0.0; yview = 0.0; zview = 0.0;	//  Viewing-coordinate origin.
		xref = 0.0; yref = 0.0; zref = -1.0;	//  Look-at point.
		Vx = 0.0; Vy = 1.0; Vz = 0.0;			//  View-up vector.
		break;
	case '4':		// 左视图
		xview = -5.0; yview = 0.0; zview = -5.0;
		xref = -4.0; yref = 0.0; zref = -5.0;
		Vx = 0.0; Vy = 1.0; Vz = 0.0;
		break;
	case '6':		// 右视图
		xview = 5.0; yview = 0.0; zview = -5.0;
		xref = 4.0; yref = 0.0; zref = -5.0;
		Vx = 0.0; Vy = 1.0; Vz = 0.0;
		break;
	case '8':		// 后视图
		xview = 0.0; yview = 0.0; zview = -10.0;
		xref = 0.0; yref = 0.0; zref = -9.0;
		Vx = 0.0; Vy = 1.0; Vz = 0.0;
		break;
	case '5':		// 顶视图
		xview = 0.0; yview = 5.0; zview = -5.0;
		xref = 0.0; yref = 4.0; zref = -5.0;
		Vx = 0.0; Vy = 0.0; Vz = -1.0;
		break;
	case 'w':
		xview += 0.1 * (xref - xview);
		xref += 0.1 * (xref - xview);
		yview += 0.1 * (yref - yview);
		yref += 0.1 * (yref - yview);
		zview += 0.1 * (zref - zview);
		zref += 0.1 * (zref - zview);
		break;
	case 'a':
		VRightx = (yref - yview) * Vz - (zref - zview) * Vy;
		VRighty = (zref - zview) * Vx - (xref - xview) * Vz;
		VRightz = (xref - xview) * Vy - (yref - yview) * Vx;
		xview -= 0.1 * VRightx;
		xref -= 0.1 * VRightx;
		yview -= 0.1 * VRighty;
		yref -= 0.1 * VRighty;
		zview -= 0.1 * VRightz;
		zref -= 0.1 * VRightz;
		break;
	case 's':
		xview -= 0.1 * (xref - xview);
		xref -= 0.1 * (xref - xview);
		yview -= 0.1 * (yref - yview);
		yref -= 0.1 * (yref - yview);
		zview -= 0.1 * (zref - zview);
		zref -= 0.1 * (zref - zview);
		break;
	case 'd':
		VRightx = (yref - yview) * Vz - (zref - zview) * Vy;
		VRighty = (zref - zview) * Vx - (xref - xview) * Vz;
		VRightz = (xref - xview) * Vy - (yref - yview) * Vx;
		xview += 0.1 * VRightx;
		xref += 0.1 * VRightx;
		yview += 0.1 * VRighty;
		yref += 0.1 * VRighty;
		zview += 0.1 * VRightz;
		zref += 0.1 * VRightz;
		break;
	case 'q':
		yview += 0.1 * 1.0;
		yref += 0.1 * 1.0;
		break;
	case 'e':
		yview -= 0.1 * 1.0;
		yref -= 0.1 * 1.0;
		break;
	case 27:		// ESC键（ASCII: 27）退出
		exit(0);
		break;
	default:
		break;
	}
	glutPostRedisplay();	// 重绘
}

void keyboard(unsigned char key, int x, int y)
{
	userEventAction(key);
}

/*----------------------------------------------------------------------*/
// 菜单栏

struct menuEntryStruct {
	const char *label;
	char key;
};

static menuEntryStruct mainMenu[] = {
	"Reset",			'0',
	"Rotate", 			'r',
	"Translate",		't',
	"Scale",			'y',
	"lines/polygons", 	'1',
	"正视图",			'2',
	"左视图",			'4',
	"右视图",			'6',
	"后视图",			'8',
	"顶视图",			'5',
	"quit", 			27,	//ESC键（ASCII: 27）
};
int mainMenuEntries = sizeof(mainMenu) / sizeof(menuEntryStruct);

void selectMain(int choice)			// 序号->key 映射
{
	userEventAction(mainMenu[choice].key);		// 调用通用动作解释函数
}

//////////////////////////////////////////////////////////////////////////

/*
** Set initial state.
*/
void init(void)
{
	glLineWidth(3.0);
	glEnable(GL_DEPTH_TEST); /* Enable hidden--surface--removal */

	// 绑定菜单
	glutCreateMenu(selectMain);					// 使用selectMain作为菜单调用函数
	for (int i = 0; i < mainMenuEntries; i++) {
		glutAddMenuEntry(mainMenu[i].label, i);	// label显示在菜单界面，i传入selectMain
	}

	glutAttachMenu(GLUT_MIDDLE_BUTTON);			// 菜单绑定
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
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMotion);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}