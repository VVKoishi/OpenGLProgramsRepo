#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <gl/glut.h>
#define MAX_OBJECTS 10000
#define DEFAULT_POINT_SIZE 10
#define DEFAULT_LINE_WIDTH 5
#define DEFAULT_LINE_CONTROL_POINTS_NUM 2
#define DEFAULT_CIRCLE_CONTROL_POINTS_NUM 2		// Բ�ĺͰ뾶
#define DEFAULT_CIRCLE_LINE_SEGMENTS_NUM 100
#define DEFAULT_BEZIER_CONTROL_POINTS_NUM 4
#define MAX_BEZIER_CONTROL_POINTS_NUM 13
#define DEFAULT_BRUSH_WIDTH 10
#define MIN_PAINT_POINT_SIZE 1
#define MAX_PAINT_DEPTH 10000
#define DEFAULT_PICK_BUFFER_SIZE 32
#define DEFAULT_PICK_WINDOW_SIZE 10
#define PI 3.14159265358979323846   // pi

enum GLState {
	MoveObject,
	MakePoint,
	PaintColor
};

struct GLColor;
struct GLTransform;

class GLObject;
class GLWorld;

// GLWorld ������
class GLPallete;	// ��ɫ�� #todo

// GLObject ������
class GLPoint;		// ��
class GLLine;		// ��
class GLTriangle;	// ������ #todo
class GLSquare;		// ������ #todo
class GLCircle;		// Բ��
class GLBezier;		// ����������
class GLBrush;		// ��ˢ #todo
class GLPaint;		// ���


struct GLColor
{
	GLfloat r = 0, g = 0, b = 0;

	bool operator==(GLColor& o) { return r==o.r && g==o.g && b==o.b ; }
	bool operator!=(GLColor& o) { return !(*this==o); }
};

struct GLTransform
{
	// �������꣬����ת��������
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
	virtual bool IsFinished() { return true; }

	void SetPosition(GLdouble x, GLdouble y, GLdouble z) {
		transform.x = x;
		transform.y = y;
		transform.z = z;
	}

	void SetColor(GLfloat r, GLfloat g, GLfloat b) {
		color.r = r;
		color.g = g;
		color.b = b;
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


class GLPallete : public GLWorld
{
public:
	GLPallete() {
		DrawCanvas();
		NewObject<GLPoint>();
		pickedObjectId = 0;
	}

	void DrawCanvas() {

	}

	void SetPoint() {

	}

	void GetColor() {

	}
};


class GLPoint : public GLObject
{
public:
	GLPoint() {
		color.r = 1;
	}

	virtual void Draw(GLenum RenderMode = GL_RENDER) {
		// ��Ҫ�ڻ���ǰ����SetPostion�����������ԭ��
		if (!visible) return;
		if (RenderMode == GL_SELECT) glLoadName((GLuint)id);
		glColor3f(color.r, color.g, color.b);
		glPointSize(DEFAULT_POINT_SIZE);
		glBegin(GL_POINTS);
		glVertex3d(transform.x, transform.y, transform.z);
		glEnd();
	}
};

class GLLine : public GLObject
{
public:
	GLPoint* ctrlPoints[DEFAULT_LINE_CONTROL_POINTS_NUM];
	int count = 0;

	GLLine() {
	}

	virtual bool IsFinished() {
		return count >= DEFAULT_LINE_CONTROL_POINTS_NUM ;
	}

	virtual void MakePoint(GLdouble x, GLdouble y, GLdouble z) {
		if (count >= DEFAULT_LINE_CONTROL_POINTS_NUM) return;
		ctrlPoints[count] = parentWorld->NewObject<GLPoint>();
		ctrlPoints[count]->SetPosition(x, y, z);			// ��ʼλ��
		ctrlPoints[count]->movable = true;					// �����ƶ�
		count++;
	}

	virtual void Draw(GLenum RenderMode = GL_RENDER) {
		// ��Ҫ�ڻ���ǰ��ʼ�����Ƶ�
		if (count == 0) return;
		for (int i = 0; i < count; ++i)
		{
			if (!ctrlPoints[i]) return;
		}
		if (RenderMode == GL_SELECT) glLoadName((GLuint)id);
		glColor3f(color.r, color.g, color.b);
		glLineWidth(DEFAULT_LINE_WIDTH);
		glBegin(GL_LINES);
		for (int i = 0; i < count; ++i)
		{
			GLPoint* ptr = ctrlPoints[i];
			glVertex3d(ptr->transform.x, ptr->transform.y, ptr->transform.z);
		}
		glEnd();
	}
};

class GLCircle : public GLObject
{
public:
	GLPoint* ctrlPoints[DEFAULT_CIRCLE_CONTROL_POINTS_NUM];
	int count = 0;

	virtual bool IsFinished() {
		return count >= DEFAULT_CIRCLE_CONTROL_POINTS_NUM ;
	}

	virtual void MakePoint(GLdouble x, GLdouble y, GLdouble z) {
		if (count >= DEFAULT_CIRCLE_CONTROL_POINTS_NUM) return;
		ctrlPoints[count] = parentWorld->NewObject<GLPoint>();
		ctrlPoints[count]->SetPosition(x, y, z);			// ��ʼλ��
		ctrlPoints[count]->movable = true;					// �����ƶ�
		count++;
	}

	virtual void Draw(GLenum RenderMode = GL_RENDER) {
		// ��Ҫ�ڻ���ǰ��ʼ�����Ƶ�
		if (count == 0) return;
		if (count != 2) return;
		for (int i = 0; i < count; ++i)
		{
			if (!ctrlPoints[i]) return;
		}
		if (RenderMode == GL_SELECT) glLoadName((GLuint)id);
		glColor3f(color.r, color.g, color.b);
		glLineWidth(DEFAULT_LINE_WIDTH);
		Circle();
	}
private:
	void Circle() {
		GLTransform a = ctrlPoints[0]->transform;
		GLTransform b = ctrlPoints[1]->transform;
		GLdouble r = sqrt( pow(b.x - a.x, 2) + pow(b.y - a.y, 2) + pow(b.z - a.z, 2) );
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i <= DEFAULT_CIRCLE_LINE_SEGMENTS_NUM; ++i)
		{
			glVertex3d(
				a.x + r * sin( i * 2 * PI / DEFAULT_CIRCLE_LINE_SEGMENTS_NUM ),
				a.y + r * cos( i * 2 * PI / DEFAULT_CIRCLE_LINE_SEGMENTS_NUM ),
				a.z
			);
		}
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
	// ˵����GLWorld::objects[0] - ���������߶���
	//		GLWorld::objects[1-4] - ���Ƶ����
	void Init() {
		for (int i = 0; i < DEFAULT_BEZIER_CONTROL_POINTS_NUM; ++i)
		{
			ctrlPoints[i] = parentWorld->NewObject<GLPoint>();
			ctrlPoints[i]->SetPosition(10*i, 10*i, 0.0);	// ��ʼλ��
			ctrlPoints[i]->movable = true;					// �����ƶ�
		}
	}

	virtual bool IsFinished() {
		return count >= MAX_BEZIER_CONTROL_POINTS_NUM ;
	}

	// Usage:
	// GLWorld::NewObject<GLBezier>()->MakePoint(x,y,z);
	virtual void MakePoint(GLdouble x, GLdouble y, GLdouble z) {
		if (count >= MAX_BEZIER_CONTROL_POINTS_NUM) return;
		ctrlPoints[count] = parentWorld->NewObject<GLPoint>();
		ctrlPoints[count]->SetPosition(x, y, z);			// ��ʼλ��
		ctrlPoints[count]->movable = true;					// �����ƶ�
		count++;
	}

	virtual void Draw(GLenum RenderMode = GL_RENDER) {
		// ��Ҫ�ڻ���ǰ��ʼ�����Ƶ�
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
		// ��������ת��
		wcPt3D innerCtrlPts[MAX_BEZIER_CONTROL_POINTS_NUM];
		for (int i = 0; i < count; ++i)
		{
			innerCtrlPts[i].x = (GLfloat) ctrlPts[i]->transform.x;
			innerCtrlPts[i].y = (GLfloat) ctrlPts[i]->transform.y;
			innerCtrlPts[i].z = (GLfloat) ctrlPts[i]->transform.z;
		}
		bezier(innerCtrlPts, count, nBezCurvePts);

		// ���ƿ��Ƶ�����
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


// #todo ����Ϳ��Ӧ�����ٴλ��ƣ���Ӧ����������ʽд��֡���棨����������������Ӱ��ϴ�
class GLPaint : public GLObject
{
public:
	virtual void Draw(GLenum RenderMode = GL_RENDER) {
		// ��Ҫ�ڻ���ǰ��ʼλ�ú���ɫ
		// SetPosition & SetColor
		if (finished) return;
		if (RenderMode == GL_SELECT) glLoadName((GLuint)id);
		glColor3f(color.r, color.g, color.b);
		glPointSize(MIN_PAINT_POINT_SIZE);
		FloodFill();
		finished = true;
	}
private:
	bool finished = false;	// ��һ�λ��ƽ������Ϊtrue

	struct Point {
		GLdouble x;
		GLdouble y;
	};
	void FloodFill();
	void fillLine(GLdouble x, GLdouble y, GLColor fillColor, GLColor interiorColor, std::vector<Point>& pointStack);
	void scanLineFilling(GLdouble x, GLdouble y, GLColor fillColor, GLColor interiorColor);
	void getPixel(GLdouble x, GLdouble y, GLColor& ptColor);
	void setPixel(GLdouble xCoord, GLdouble yCoord);
};



//////////////////////////////////////////////////////////////////////////

/*  Print the contents of the pick buffer for each mouse selection.  */
void GLWorld::processPicks (GLint nPicks, GLuint pickBuffer [ ])
{
	// �����ȡ�������һ��ID������Ϊ����Ƶ�ͼ�����Ȼ��Ƶ�ͼ��֮��
	if (nPicks == 0) pickedObjectId = -1;
	else
	{
		// ��Ϊֻʹ��һ��ջ��Ԫ�أ����Կ����ɹ�ʽ����ó����һ��ͼ��ID
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

// ʹ���ⲿ�������������ϵ�ü��ռ䣬������ѡȡʱ�Ĳü�����
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
	
	glPushName(MAX_OBJECTS);	// ֻʹ��һ��ջ��Ԫ�أ�����Ϊͼ������ʹ�� glLoadName(id) �滻ջ��

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

void GLPaint::FloodFill()
{
	GLColor orignColor;
	getPixel(this->transform.x, this->transform.y, orignColor);
	scanLineFilling(this->transform.x, this->transform.y, this->color, orignColor);
}

void GLPaint::fillLine(GLdouble x, GLdouble y, GLColor fillColor, GLColor interiorColor, std::vector<Point>& pointStack)
{

	GLdouble startX = x;
	GLdouble startY = y;

	GLdouble leftX = x;
	GLdouble rightX = x;

	// �����Һ��������
	GLColor oldColor;
	getPixel(x, y, oldColor);
	while (oldColor == interiorColor && oldColor != fillColor)
	{
		setPixel(x, y);
		x = x + MIN_PAINT_POINT_SIZE;
		getPixel(x, y, oldColor);
	}
	rightX = x - MIN_PAINT_POINT_SIZE;
	x = startX - MIN_PAINT_POINT_SIZE;
	getPixel(x, y, oldColor);
	while (oldColor == interiorColor && oldColor != fillColor)
	{
		setPixel(x, y);
		x = x - MIN_PAINT_POINT_SIZE;
		getPixel(x, y, oldColor);
	}
	leftX = x + MIN_PAINT_POINT_SIZE;

	// ��������,���º��ϣ���ȡ4��ͨ
	x = leftX;
	y = startY - MIN_PAINT_POINT_SIZE;
	bool startContinue = false;
	getPixel(x, y, oldColor);
	while (x <= rightX)
	{
		if (!startContinue) {
			if (oldColor == interiorColor && oldColor != fillColor) {
				pointStack.push_back({ x,y });
				startContinue = true;
			}
		}
		else {
			if (oldColor != interiorColor || oldColor == fillColor) startContinue = false;
		}
		x = x + MIN_PAINT_POINT_SIZE;
		getPixel(x, y, oldColor);
	}

	x = leftX;
	y = startY + MIN_PAINT_POINT_SIZE;
	startContinue = false;
	getPixel(x, y, oldColor);
	while (x <= rightX)
	{
		if (!startContinue) {
			if (oldColor == interiorColor && oldColor != fillColor) {
				pointStack.push_back({ x,y });
				startContinue = true;
			}
		}
		else {
			if (oldColor != interiorColor || oldColor == fillColor) startContinue = false;
		}
		x = x + MIN_PAINT_POINT_SIZE;
		getPixel(x, y, oldColor);
	}
}

void GLPaint::scanLineFilling(GLdouble x, GLdouble y, GLColor fillColor, GLColor interiorColor)
{
	std::vector<Point> pointStack;

	fillLine(x, y, fillColor, interiorColor, pointStack);

	while (!pointStack.empty())
	{
		GLdouble popX = pointStack.back().x;
		GLdouble popY = pointStack.back().y;
		pointStack.pop_back();
		fillLine(popX, popY, fillColor, interiorColor, pointStack);
	}
}

void GLPaint::getPixel (GLdouble x, GLdouble y, GLColor& ptColor)
{
	if(!finished) glFlush();
	GLdouble winx, winy, winz;
	GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);
	gluProject(x, y, 0.0f, modelview, projection, viewport, &winx, &winy, &winz);
	// ������������ϵX,Y����Ļ��������ϵ��ͬ���˴��������
	glReadPixels((GLint)round(winx), (GLint)round(winy), 1, 1, GL_RGB, GL_FLOAT, (GLvoid*)&ptColor);
}

void GLPaint::setPixel (GLdouble xCoord, GLdouble yCoord)
{
    glBegin (GL_POINTS);
        glVertex2d (xCoord, yCoord);
    glEnd ( );
}
//////////////////////////////////////////////////////////////////////////
