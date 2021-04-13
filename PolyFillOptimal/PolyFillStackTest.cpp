#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <windows.h>

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

void getPixel(GLint x, GLint y, Color& ptColor)
{
	glFlush();
	glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, (GLvoid*)&ptColor);
}

void setPixel (GLint xCoord, GLint yCoord)
{
    glBegin (GL_POINTS);
        glVertex2i (xCoord, yCoord);
    glEnd ( );
}

//////////////////////////////////////////////////////////////////////////
int stackMemSize = 0;
void* stackMemBasePtr = nullptr;
MEMORY_BASIC_INFORMATION memInfo;

void boundaryFill4 (GLint x, GLint y, Color fillColor, Color borderColor)
{
	Color interiorColor;

	if (VirtualQuery((LPCVOID)&interiorColor, &memInfo, sizeof(memInfo))) {
		int stackDeep = (int)stackMemBasePtr - (int)memInfo.BaseAddress;
		if (stackDeep > stackMemSize) {
			stackMemSize = stackDeep;
			//printf("0x%p\n", memInfo.BaseAddress);
		}
	}

	//int stackDeep = (int)stackMemBasePtr - (int)&interiorColor;
	//if (stackDeep > stackMemSize) {
	//	stackMemSize = stackDeep;
	//}
   
	/* Set current color to fillColor, then perform the following operations. */
	getPixel (x, y, interiorColor);
	if ((interiorColor != borderColor) && (interiorColor != fillColor)) {
		setPixel (x, y);    // Set color of pixel to fillColor.  
		boundaryFill4 (x + 1, y , fillColor, borderColor);
		boundaryFill4 (x - 1, y , fillColor, borderColor);
		boundaryFill4 (x , y + 1, fillColor, borderColor);
		boundaryFill4 (x , y - 1, fillColor, borderColor);
	}
}

void floodFill4 (GLint x, GLint y, Color fillColor, Color interiorColor)
{
   Color color;

   	if (VirtualQuery((LPCVOID)&interiorColor, &memInfo, sizeof(memInfo))) {
		int stackDeep = (int)stackMemBasePtr - (int)memInfo.BaseAddress;
		if (stackDeep > stackMemSize) {
			stackMemSize = stackDeep;
			//printf("0x%p\n", memInfo.BaseAddress);
		}
	}

   /* Set current color to fillColor, then perform the following operations. */
   getPixel (x, y, color);
   if (color == interiorColor) {
      setPixel (x, y);    // Set color of pixel to fillColor.
      floodFill4 (x + 1, y, fillColor, interiorColor);
      floodFill4 (x - 1, y, fillColor, interiorColor);
      floodFill4 (x, y + 1, fillColor, interiorColor);
      floodFill4 (x, y - 1, fillColor, interiorColor);
   }
}
//////////////////////////////////////////////////////////////////////////

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

	stackMemSize = 0;

	glColor3f (1, 0, 0);
	boundaryFill4(100, 101, Color(1, 0, 0), Color(1, 1, 1));
	glFlush();

	printf("StackMemSize: %d KB \n", stackMemSize / 1024);

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

	stackMemSize = 0;

	glColor3f (1, 1, 1);
	floodFill4(300, 101, Color(1, 1, 1), Color(0, 0, 0));
	glFlush();

	printf("StackMemSize: %d KB \n", stackMemSize / 1024);

	glColor3f (1, 1, 1);
	glBegin(GL_POLYGON);
		glVertex2i(450, 100);
		glVertex2i(550, 100);
		glVertex2i(475, 200);
		glVertex2i(575, 200);
	glEnd();

	glFlush ( );     // Process all OpenGL routines as quickly as possible.
}

void main (int argc, char** argv)
{
	int stackBase = 0;
	if (VirtualQuery(&stackBase, &memInfo, sizeof(memInfo))) {
		stackMemBasePtr = memInfo.BaseAddress;
		printf("StackBase : 0x%p\n", memInfo.BaseAddress);
	}
	//stackMemBasePtr = &stackBase;

    glutInit (&argc, argv);                         // Initialize GLUT.
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);   // Set display mode.
    glutInitWindowPosition (50, 100);   // Set top-left display-window position.
    glutInitWindowSize (600, 300);      // Set display-window width and height.
    glutCreateWindow ("PolyFill"); // Create display window.

    init ( );                            // Execute initialization procedure.
    glutDisplayFunc (renderScene);       // Send graphics to display window.
    glutMainLoop ( );                    // Display everything and wait.
}