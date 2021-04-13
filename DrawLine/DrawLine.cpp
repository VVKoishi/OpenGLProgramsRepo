#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>


void setPixel (GLint xCoord, GLint yCoord)
{
    glBegin (GL_POINTS);
        glVertex2i (xCoord, yCoord);
    glEnd ( );
}

//////////////////////////////////////////////////////////////////////////
inline int round (const float a)  { return int (a + 0.5); }

void lineDDA (int x0, int y0, int xEnd, int yEnd)
{
   int dx = xEnd - x0,  dy = yEnd - y0,  steps,  k;
   float xIncrement, yIncrement, x = x0, y = y0;

   if (fabs (dx) > fabs (dy))
      steps = fabs (dx);
   else
      steps = fabs (dy);
   xIncrement = float (dx) / float (steps);
   yIncrement = float (dy) / float (steps);

   setPixel (round (x), round (y));
   for (k = 0; k < steps; k++) {
      x += xIncrement;
      y += yIncrement;
      setPixel (round (x), round (y));
   }
}

/*  Bresenham line-drawing procedure for |m| < 1.0.  */
void lineBres (int x0, int y0, int xEnd, int yEnd)
{
   int dx = fabs (xEnd - x0),  dy = fabs(yEnd - y0);
   int p = 2 * dy - dx;
   int twoDy = 2 * dy,  twoDyMinusDx = 2 * (dy - dx);
   int x, y;

   /* Determine which endpoint to use as start position.  */
   if (x0 > xEnd) {
      x = xEnd;
      y = yEnd;
      xEnd = x0;
   }
   else {
      x = x0;
      y = y0;
   }
   setPixel (x, y);

   while (x < xEnd) {
      x++;
      if (p < 0)
         p += twoDy;
      else {
         y++;
         p += twoDyMinusDx;
      }
      setPixel (x, y);
   }
}
//////////////////////////////////////////////////////////////////////////

void init (void)
{
    glClearColor (0.0, 0.0, 0.0, 0.0);  // Set display-window color to white.
    gluOrtho2D (0.0, 200.0, 0.0, 150.0);
}

void renderScene(void)
{
	glClear (GL_COLOR_BUFFER_BIT);  // Clear display window.

	glColor3f (1, 0, 0);
	lineDDA(50, 75, 150, 125);

	glColor3f (0, 1, 0);
	lineBres(50, 50, 150, 100);

	glColor3f (0, 0, 1);
	glBegin(GL_LINES);
		glVertex2i(50, 25);
		glVertex2i(150, 75);
	glEnd();

	glFlush ( );     // Process all OpenGL routines as quickly as possible.
}

void main (int argc, char** argv)
{
    glutInit (&argc, argv);                         // Initialize GLUT.
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);   // Set display mode.
    glutInitWindowPosition (50, 100);   // Set top-left display-window position.
    glutInitWindowSize (400, 300);      // Set display-window width and height.
    glutCreateWindow ("DrawLine"); // Create display window.

    init ( );                            // Execute initialization procedure.
    glutDisplayFunc (renderScene);       // Send graphics to display window.
    glutMainLoop ( );                    // Display everything and wait.
}