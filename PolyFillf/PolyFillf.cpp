#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

static GLdouble dx = 1.0, dy = 1.0;
static GLsizei winw, winh;
struct Color
{
	GLfloat r, g, b;

	Color() = default;
	Color(GLfloat inR, GLfloat inG, GLfloat inB)
		: r(inR)
		, g(inG)
		, b(inB)
	{}

	bool operator==(Color otherColor)
	{
		return r == otherColor.r && g == otherColor.g && b == otherColor.b;
	}
	bool operator!=(Color otherColor)
	{
		return !(*this == otherColor); 
	}
};

void getPixel(GLdouble x, GLdouble y, Color& ptColor)
{
	glFlush();
	GLdouble winx, winy, winz;
	GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);
	gluProject(x, y, 0.0f, modelview, projection, viewport, &winx, &winy, &winz);
	// 由于世界坐标系X,Y和屏幕像素坐标系不同，此处容易溢出
	glReadPixels((GLint)round(winx), (GLint)round(winy), 1, 1, GL_RGB, GL_FLOAT, (GLvoid*)&ptColor);
}

void setPixel (GLdouble xCoord, GLdouble yCoord)
{
    glBegin (GL_POINTS);
        glVertex2d (xCoord, yCoord);
    glEnd ( );
}

static int count = 0;
static int stackCount = 0;
void boundaryFill4 (GLdouble x, GLdouble y, Color fillColor, Color borderColor)
{
   Color interiorColor;
   count++; if (count >= 5000) return;
   /* Set current color to fillColor, then perform the following operations. */
   getPixel (x, y, interiorColor);
   if ((interiorColor != borderColor) && (interiorColor != fillColor)) {
      setPixel (x, y);    // Set color of pixel to fillColor.  
      boundaryFill4 (x + dx, y , fillColor, borderColor);
      boundaryFill4 (x - dx, y , fillColor, borderColor);
      boundaryFill4 (x , y + dy, fillColor, borderColor);
      boundaryFill4 (x , y - dy, fillColor, borderColor);
   }
}

void floodFill4 (GLdouble x, GLdouble y, Color fillColor, Color interiorColor)
{
   Color color;

   /* Set current color to fillColor, then perform the following operations. */
   getPixel (x, y, color);
   if (color == interiorColor) {
      setPixel (x, y);    // Set color of pixel to fillColor.
      floodFill4 (x + dx, y, fillColor, interiorColor);
      floodFill4 (x - dx, y, fillColor, interiorColor);
      floodFill4 (x, y + dy, fillColor, interiorColor);
      floodFill4 (x, y - dy, fillColor, interiorColor);
   }
}


void init (void)
{
    glClearColor (0.0, 0.0, 0.0, 0.0);  // Set display-window color to white.
    gluOrtho2D (0.0, 600.0, 0.0, 300.0);
}

void renderScene(void)
{
	glClear (GL_COLOR_BUFFER_BIT);  // Clear display window.

	glColor3f (1, 1, 1);
	glBegin(GL_LINE_LOOP);
		glVertex2i(50, 100);
		glVertex2i(150, 100);
		glVertex2i(75, 200);
		glVertex2i(175, 200);
	glEnd();
	glFlush();

	glColor3f (1, 0, 0);
	boundaryFill4(100, 101, Color(1, 0, 0), Color(1, 1, 1));
	glFlush();

	glColor3f (1, 0, 0);
	glBegin(GL_POLYGON);
		glVertex2i(225, 100);
		glVertex2i(360, 100);
		glVertex2i(300, 70);
	glEnd();
	glColor3f (0, 1, 0);
	glBegin(GL_POLYGON);
		glVertex2i(310, 175);
		glVertex2i(250, 100);
		glVertex2i(200, 125);
	glEnd();
	glColor3f (0, 0, 1);
	glBegin(GL_POLYGON);
		glVertex2i(280, 150);
		glVertex2i(350, 100);
		glVertex2i(375, 125);
	glEnd();
	glFlush();

	glColor3f (1, 1, 1);
	//floodFill4(300, 101, Color(1, 1, 1), Color(0, 0, 0));
	glFlush();

	glColor3f (1, 1, 1);
	glBegin(GL_POLYGON);
		glVertex2i(450, 100);
		glVertex2i(550, 100);
		glVertex2i(475, 200);
		glVertex2i(575, 200);
	glEnd();

	glFlush ( );     // Process all OpenGL routines as quickly as possible.
}

void myReshape(GLsizei w, GLsizei h)
{
	winw = w;
	winh = h;
	//glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//gluOrtho2D (0, w, 0, h);
	gluOrtho2D (0, 600, 0, 300);
	count = 0;
	stackCount = 0;
	
	dx = 1.0 * 600.0 / (GLdouble)w;
	dy = 1.0 * 300.0 / (GLdouble)h;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
}

void main (int argc, char** argv)
{
    glutInit (&argc, argv);                         // Initialize GLUT.
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);   // Set display mode.
    glutInitWindowPosition (50, 100);   // Set top-left display-window position.
    glutInitWindowSize (600, 300);      // Set display-window width and height.
    glutCreateWindow ("PolyFill"); // Create display window.

    init ( );                            // Execute initialization procedure.
    glutDisplayFunc (renderScene);       // Send graphics to display window.
	glutReshapeFunc(myReshape);
    glutMainLoop ( );                    // Display everything and wait.
}