#include "GLObject.h"

GLWorld MyWorld;

GLState MyState = MoveObject;

/*  Set initial size of the display window.  */
GLsizei winWidth = 900, winHeight = 600;  

/*  Set size of world-coordinate clipping window.  */
GLfloat xwcMin = -450.0, xwcMax = 450.0;
GLfloat ywcMin = -300.0, ywcMax = 300.0;

/*----------------------------------------------------------------------*/
// 菜单栏

struct menuEntryStruct {
	const char *label;
	char key;
};

static menuEntryStruct mainMenu[] = {
	"Reset",			'0',
	"Create Line",		'1',
	"Create Circle",	'2',
	"Create Bezier/Finish Bezier",	'3',
	"Paint Color",		'4',
	"Hide/Show All Control Points",	'5',
	"Quit", 			27,	//ESC键（ASCII: 27）
};
int mainMenuEntries = sizeof(mainMenu) / sizeof(menuEntryStruct);


bool globalControlPointsVisible = true;

void userEventAction(int key) {
	switch (key) {
	case '0':
		MyWorld.Empty();
		MyWorld.pickedObjectId = -1;
		MyState = MoveObject;
		break;
	case '1':	// line
		if (MyState == MoveObject)
		{
			GLLine* linePtr = MyWorld.NewObject<GLLine>();
			MyState = MakePoint;
			MyWorld.pickedObjectId = linePtr->id;
		} 
		else if (MyState == MakePoint)
		{
			MyState = MoveObject;
			MyWorld.pickedObjectId = -1;
		}
		break;
	case '2':	// circle
		if (MyState == MoveObject)
		{
			GLCircle* cirPtr = MyWorld.NewObject<GLCircle>();
			MyState = MakePoint;
			MyWorld.pickedObjectId = cirPtr->id;
		} 
		else if (MyState == MakePoint)
		{
			MyState = MoveObject;
			MyWorld.pickedObjectId = -1;
		}
		break;
	case '3':	// bezier
		//MyWorld.NewObject<GLBezier>()->Init();
		if (MyState == MoveObject)
		{
			GLBezier* bezPtr = MyWorld.NewObject<GLBezier>();
			MyState = MakePoint;
			MyWorld.pickedObjectId = bezPtr->id;
		}
		else if (MyState == MakePoint)
		{
			MyState = MoveObject;
			MyWorld.pickedObjectId = -1;
		}
		break;
	case '4':	// paint color
		if (MyState == MoveObject)
		{
			MyState = PaintColor;
		}
		else if (MyState == MakePoint)
		{
			MyState = MoveObject;
			MyWorld.pickedObjectId = -1;
		}
		break;
	case '5':	// hide/show ctrl points
		globalControlPointsVisible = !globalControlPointsVisible;
		for (int i = 0; i < MyWorld.count; ++i)
		{
			GLPoint* pointPtr = dynamic_cast<GLPoint *>(MyWorld.objects[i]);
			if (NULL != pointPtr)
			{
				pointPtr->visible = globalControlPointsVisible;
			}
		}
		break;
	case 27:		// ESC键（ASCII: 27）退出
		MyWorld.Empty();
		exit(0);
		break;
	default:
		break;
	}
	glutPostRedisplay();	// 重绘
}

void selectMain(int choice)			// 序号->key 映射
{
	userEventAction(mainMenu[choice].key);		// 调用通用动作解释函数
}

void init (void)
{
	/*  Set color of display window to white.  */
	glClearColor (1.0, 1.0, 1.0, 0.0);

	// 绑定菜单
	glutCreateMenu(selectMain);					// 使用selectMain作为菜单调用函数
	for (int i = 0; i < mainMenuEntries; i++) {
		glutAddMenuEntry(mainMenu[i].label, i);	// label显示在菜单界面，i传入selectMain
	}
	glutAttachMenu(GLUT_RIGHT_BUTTON);			// 菜单绑定
}

void displayFcn (void)
{
	glClear (GL_COLOR_BUFFER_BIT);   //  Clear display window.

	MyWorld.DrawObjects(GL_RENDER);

	glFlush ( );
}

void winReshapeFcn (GLint newWidth, GLint newHeight)
{
	winWidth = newWidth;
	winHeight = newHeight;

	glViewport (0, 0, newWidth, newHeight);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ( );

	gluOrtho2D (xwcMin, xwcMax, ywcMin, ywcMax);

	glClear (GL_COLOR_BUFFER_BIT);
}

void unProject(GLdouble winx, GLdouble winy, GLdouble winz, GLdouble *objx, GLdouble *objy, GLdouble *objz)
{
	// 从窗口到世界坐标系
	GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);
	gluUnProject(winx, winy, winz, modelview, projection, viewport, objx, objy, objz);
}

void mouseButton(int button, int state, int x, int y)
{
	switch (button) {
	case GLUT_LEFT_BUTTON:
		break;
	default:
		return;
	}
	switch (state) {
	case GLUT_DOWN:
		if (MyState == MoveObject)
		{
			MyWorld.PickObject(x, y);
			printf("MyWorld.pickedObjectId: %d \n", MyWorld.pickedObjectId);
		}
		else if (MyState == MakePoint)
		{
			if (GLObject* ptr = MyWorld.objects[MyWorld.pickedObjectId])
			{
				// 从窗口到世界坐标系
				GLdouble winx = (GLdouble) x;
				GLdouble winy = (GLdouble) winHeight - (GLdouble) y;
				GLdouble winz = 0;
				GLdouble objx = 0;
				GLdouble objy = 0;
				GLdouble objz = 0;
				unProject(winx, winy, winz, &objx, &objy, &objz);
				ptr->MakePoint(objx, objy, objz);
				// 检查完成
				if (ptr->IsFinished())
				{
					MyState = MoveObject;
					MyWorld.pickedObjectId = -1;
				}
			}
		}
		else if (MyState == PaintColor)
		{
			GLPaint* paintPtr = MyWorld.NewObject<GLPaint>();
			MyWorld.pickedObjectId = paintPtr->id;
			// 从窗口到世界坐标系
			GLdouble winx = (GLdouble) x;
			GLdouble winy = (GLdouble) winHeight - (GLdouble) y;
			GLdouble winz = 0;
			GLdouble objx = 0;
			GLdouble objy = 0;
			GLdouble objz = 0;
			unProject(winx, winy, winz, &objx, &objy, &objz);
			paintPtr->SetPosition(objx, objy, objz);
			paintPtr->SetColor(0, 0, 1);

			MyState = MoveObject;
			MyWorld.pickedObjectId = -1;
		}
		break;
	case GLUT_UP:
		if (MyState == MoveObject)
		{
			MyWorld.pickedObjectId = -1;
		}
		glutPostRedisplay();
		break;
	}
}

void mouseMotion(int x, int y)
{
	if ( MyWorld.pickedObjectId == -1 ) return;
	if ( ! MyWorld.objects[MyWorld.pickedObjectId]->movable ) return;

	GLObject* ptr = MyWorld.objects[MyWorld.pickedObjectId];
	
	// 从窗口到世界坐标系
	GLdouble winx = (GLdouble) x;
	GLdouble winy = (GLdouble) winHeight - (GLdouble) y;
	GLdouble winz = 0;
	GLdouble objx = 0;
	GLdouble objy = 0;
	GLdouble objz = 0;
	unProject(winx, winy, winz, &objx, &objy, &objz);

	ptr->SetPosition(objx, objy, objz);

	glutPostRedisplay();
}

void main (int argc, char** argv)
{
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition (50, 50);
	glutInitWindowSize (winWidth, winHeight);
	glutCreateWindow ("Bezier Curve");

	init ( );
	glutDisplayFunc (displayFcn);
	glutReshapeFunc (winReshapeFcn);
	glutMouseFunc (mouseButton);
	glutMotionFunc (mouseMotion);

	glutMainLoop ( );
}
