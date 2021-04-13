#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <gl/glut.h>
#define MAX_OBJECTS 10000
#define DEFAULT_POINT_SIZE 10
#define DEFAULT_LINE_WIDTH 5
#define DEFAULT_BEZIER_CONTROL_POINTS_NUM 4
#define MAX_BEZIER_CONTROL_POINTS_NUM 13
#define DEFAULT_PICK_BUFFER_SIZE 32
#define DEFAULT_PICK_WINDOW_SIZE 10
#define PI 3.14159265358979323846   // pi

enum GLState {
	MoveObject,
	MakePoint
};

struct GLColor;
struct GLTransform;

class GLObject;
class GLWorld;

// GLObject 派生类
class GLPoint;		// 点
class GLBezier;		// 贝塞尔曲线


struct GLColor
{
	GLfloat r = 0, g = 0, b = 0;
};

struct GLTransform
{
	// 世界坐标，轴旋转，轴缩放
	GLdouble x = 0, y = 0, z = 0;
	GLdouble rx = 0, ry = 0, rz = 0;
	GLdouble sx = 1, sy = 1, sz = 1;
};

class GLObject
{
public:
	GLint id = 0;
	GLColor color;
	GLTransform transform;
	bool movable = false;
	bool visible = true;

	GLWorld* parentWorld;
	
	virtual void Draw(GLenum RenderMode = GL_RENDER) {}

	virtual void MakePoint(GLdouble x, GLdouble y, GLdouble z) {}

	void SetPosition(GLdouble x, GLdouble y, GLdouble z) {
		transform.x = x;
		transform.y = y;
		transform.z = z;
	}
};

class GLWorld
{
public:
	GLObject* objects[MAX_OBJECTS];
	GLint count = 0;

	GLWorld() = default;
	~GLWorld() {
		for (int i = count - 1; i>=0; --i)
		{
		    delete objects[i];
		}
	}

	template<typename T>
	T* NewObject() {
		if (count < MAX_OBJECTS)
		{
			T* ret = new T;
			objects[count] = (GLObject*) ret;
			objects[count]->parentWorld = this;
			objects[count]->id = count;
			++count;
			return ret;
		}
		else return nullptr;
	}

	void DeleteObject(GLint id) {
		if (id >= 0 && id < count)
		{
			delete objects[id];
			for (int i = id; i < count - 1; ++i)
			{
				objects[i] = objects[i+1];
			}
			--count;
		}
	}

	void Empty() {
		for (int i = count - 1; i >= 0; --i)
		{
			DeleteObject(i);
		}
	}

	void DrawObjects(GLenum RenderMode = GL_RENDER) {
		for (int i = 0; i < count; ++i)
		{
			objects[i]->Draw(RenderMode);
		}
	}

	GLint pickedObjectId = -1;
	void PickObject(GLint xMouse, GLint yMouse) {
		pickRects(GLUT_LEFT_BUTTON, GLUT_DOWN, xMouse, yMouse);
	}
private:
	void pickRects (GLint button, GLint action, GLint xMouse, GLint yMouse);
	void processPicks (GLint nPicks, GLuint pickBuffer [ ]);
};

class GLPoint : public GLObject
{
public:
	GLPoint() {
		color.r = 1;
	}

	virtual void Draw(GLenum RenderMode = GL_RENDER) {
		// 需要在绘制前调用SetPostion，否则绘制在原点
		if (!visible) return;
		if (RenderMode == GL_SELECT) glLoadName((GLuint)id);
		glColor3f(color.r, color.g, color.b);
		glPointSize(DEFAULT_POINT_SIZE);
		glBegin(GL_POINTS);
		glVertex3d(transform.x, transform.y, transform.z);
		glEnd();
	}
};

class GLBezier : public GLObject
{
public:
	GLPoint* ctrlPoints[MAX_BEZIER_CONTROL_POINTS_NUM];
	int count = 0;
	GLint nBezCurvePoints = 1000;

	// Usage:
	// GLWorld::NewObject<GLBezier>()->Init();
	// 说明：GLWorld::objects[0] - 贝塞尔曲线对象
	//		GLWorld::objects[1-4] - 控制点对象
	void Init() {
		for (int i = 0; i < DEFAULT_BEZIER_CONTROL_POINTS_NUM; ++i)
		{
			ctrlPoints[i] = parentWorld->NewObject<GLPoint>();
			ctrlPoints[i]->SetPosition(10*i, 10*i, 0.0);	// 初始位置
			ctrlPoints[i]->movable = true;					// 可以移动
		}
	}

	// Usage:
	// GLWorld::NewObject<GLBezier>()->MakePoint(x,y,z);
	virtual void MakePoint(GLdouble x, GLdouble y, GLdouble z) {
		if (count >= MAX_BEZIER_CONTROL_POINTS_NUM) return;
		ctrlPoints[count] = parentWorld->NewObject<GLPoint>();
		ctrlPoints[count]->SetPosition(x, y, z);			// 初始位置
		ctrlPoints[count]->movable = true;					// 可以移动
		count++;
	}

	virtual void Draw(GLenum RenderMode = GL_RENDER) {
		// 需要在绘制前初始化控制点
		if (count == 0) return;
		for (int i = 0; i < count; ++i)
		{
			if (!ctrlPoints[i]) return;
		}
		if (RenderMode == GL_SELECT) glLoadName((GLuint)id);
		glColor3f(color.r, color.g, color.b);
		glPointSize(DEFAULT_LINE_WIDTH);
		bezier(ctrlPoints, nBezCurvePoints);
	}

private:
	class wcPt3D {
	public:
		GLfloat x, y, z;
	};
	void plotPoint (wcPt3D bezCurvePt);
	void binomialCoeffs (GLint n, GLint * C);
	void computeBezPt (GLfloat u, wcPt3D * bezPt, GLint nCtrlPts, wcPt3D * ctrlPts, GLint * C);
	void bezier (wcPt3D * ctrlPts, GLint nCtrlPts, GLint nBezCurvePts);
	void bezier(GLPoint* ctrlPts[], GLint nBezCurvePts) {
		// 函数重载转发
		wcPt3D innerCtrlPts[MAX_BEZIER_CONTROL_POINTS_NUM];
		for (int i = 0; i < count; ++i)
		{
			innerCtrlPts[i].x = (GLfloat) ctrlPts[i]->transform.x;
			innerCtrlPts[i].y = (GLfloat) ctrlPts[i]->transform.y;
			innerCtrlPts[i].z = (GLfloat) ctrlPts[i]->transform.z;
		}
		bezier(innerCtrlPts, count, nBezCurvePts);

		// 绘制控制点连线
		if (ctrlPts[0] && ctrlPts[0]->visible == false) return;
		glColor3f(0.0, 1.0, 0.0);
		glLineWidth(2.0);
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < count; ++i)
		{
			glVertex3d(ctrlPts[i]->transform.x, ctrlPts[i]->transform.y, ctrlPts[i]->transform.z);
		}
		glEnd();
	}
};


//////////////////////////////////////////////////////////////////////////

/*  Print the contents of the pick buffer for each mouse selection.  */
void GLWorld::processPicks (GLint nPicks, GLuint pickBuffer [ ])
{
	// 储存获取到的最后一个ID，即认为后绘制的图像在先绘制的图形之上
	if (nPicks == 0) pickedObjectId = -1;
	else
	{
		// 因为只使用一个栈顶元素，所以可以由公式计算得出最后一个图形ID
		int chunkId = 4 * (nPicks - 1);
		pickedObjectId = pickBuffer[chunkId + 3];
	}
	return;

	GLint j, k;
	GLuint objID, *ptr;

	printf (" Number of objects picked = %d\n", nPicks);
	printf ("\n");
	ptr = pickBuffer;

	/*  Output all items in each pick record.  */
	for (j = 0; j < nPicks; j++) {
		objID = *ptr;

		printf ("   Stack position = %d\n", objID);
		ptr++;

		printf ("   Min depth = %g,", float (*ptr/0x7fffffff));
		ptr++;

		printf ("   Max depth = %g\n", float (*ptr/0x7fffffff));
		ptr++;

		printf ("   Stack IDs are: \n");
		for (k = 0; k < objID; k++) {
			printf ("   %d ",*ptr);
			ptr++;
		}
		printf ("\n\n");
	}
}

// 使用外部定义的世界坐标系裁剪空间，供设置选取时的裁剪窗口
extern GLfloat xwcMin, xwcMax, ywcMin, ywcMax;

void GLWorld::pickRects (GLint button, GLint action, GLint xMouse, GLint yMouse)
{
	GLuint pickBuffer [DEFAULT_PICK_BUFFER_SIZE];
	GLint nPicks, vpArray [4];

	if (button != GLUT_LEFT_BUTTON || action != GLUT_DOWN)
		return;

	glSelectBuffer (DEFAULT_PICK_BUFFER_SIZE, pickBuffer);  //  Designate pick buffer.
	glRenderMode (GL_SELECT);             //  Activate picking operations.
	glInitNames ( );                   //  Initialize the object-ID stack.
	
	glPushName(MAX_OBJECTS);	// 只使用一个栈顶元素，后续为图形命名使用 glLoadName(id) 替换栈顶

	/*  Save current viewing matrix.  */
	glMatrixMode (GL_PROJECTION);
	glPushMatrix ( );
	glLoadIdentity ( );

	/*  Obtain the parameters for the current viewport.  Set up 
	*  a 5 x 5 pick window, and invert the input yMouse value 
	*  using the height of the viewport, which is the fourth 
	*  element of vpArray.  
	*/
	glGetIntegerv (GL_VIEWPORT, vpArray);
	gluPickMatrix (GLdouble (xMouse), GLdouble (vpArray [3] - yMouse), 
					DEFAULT_PICK_WINDOW_SIZE, DEFAULT_PICK_WINDOW_SIZE, vpArray);

	gluOrtho2D (xwcMin, xwcMax, ywcMin, ywcMax);
	//rects (GL_SELECT);       // Process the rectangles in selection mode.
	this->DrawObjects(GL_SELECT);

	/*  Restore original viewing matrix.  */
	glMatrixMode (GL_PROJECTION);
	glPopMatrix ( );

	glFlush ( );

	/*  Determine the number of picked objects and return to the 
	*  normal rendering mode.  
	*/
	nPicks = glRenderMode (GL_RENDER);

	processPicks (nPicks, pickBuffer);  // Process picked objects.

	glutPostRedisplay ( );
}

void GLBezier::plotPoint(wcPt3D bezCurvePt)
{
   glBegin (GL_POINTS);
      glVertex2f (bezCurvePt.x, bezCurvePt.y);
   glEnd ( );
}

void GLBezier::binomialCoeffs(GLint n, GLint * C)
{
   GLint k, j;

   for (k = 0;  k <= n;  k++) {
      /*  Compute n!/(k!(n - k)!).  */
      C [k] = 1;
      for (j = n;  j >= k + 1;  j--)
         C [k] *= j;
      for (j = n - k;  j >= 2;  j--)
         C [k] /= j;
   }
}

void GLBezier::computeBezPt(GLfloat u, wcPt3D * bezPt, GLint nCtrlPts, wcPt3D * ctrlPts, GLint * C)
{
   GLint k, n = nCtrlPts - 1;
   GLfloat bezBlendFcn;

   bezPt->x = bezPt->y = bezPt->z = 0.0;

   /*  Compute blending functions and blend control points. */
   for (k = 0; k < nCtrlPts; k++) {
      bezBlendFcn = C [k] * pow (u, k) * pow (1 - u, n - k);
      bezPt->x += ctrlPts [k].x * bezBlendFcn;
      bezPt->y += ctrlPts [k].y * bezBlendFcn;
      bezPt->z += ctrlPts [k].z * bezBlendFcn;
   }
}

void GLBezier::bezier(wcPt3D * ctrlPts, GLint nCtrlPts, GLint nBezCurvePts)
{
   wcPt3D bezCurvePt;
   GLfloat u;
   GLint *C, k;

   /*  Allocate space for binomial coefficients  */
   C = new GLint [nCtrlPts];

   binomialCoeffs (nCtrlPts - 1, C);
   for (k = 0;  k <= nBezCurvePts;  k++) {
      u = GLfloat (k) / GLfloat (nBezCurvePts);
      computeBezPt (u, &bezCurvePt, nCtrlPts, ctrlPts, C);
      plotPoint (bezCurvePt);
   }
   delete [ ] C;
}

//////////////////////////////////////////////////////////////////////////
