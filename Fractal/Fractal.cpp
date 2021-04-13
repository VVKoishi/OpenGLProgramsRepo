#include <stdio.h>
#include <math.h>
#include <GL/glut.h>

/*  Set initial size of the display window.  */
GLsizei winWidth = 500, winHeight = 500;  

/*  Set limits for the rectangular area in complex plane.  */
GLdouble xComplexMin = -2.00, xComplexMax = 0.50;  
GLdouble yComplexMin = -1.25, yComplexMax = 1.25;

GLdouble complexWidth = xComplexMax - xComplexMin;
GLdouble complexHeight = yComplexMax - yComplexMin;

GLint newX = 0, newY = 0;

class complexNum {
   public:
      GLdouble x, y;
};

struct color { GLfloat r, g, b; };

void init (void)
{
   /*  Set display-window color to white.  */
   glClearColor (1.0, 1.0, 1.0, 0.0); 
}

void plotPoint (complexNum z)
{
   glBegin (GL_POINTS);
      glVertex2d (z.x, z.y);
   glEnd ( );
}

/*  Calculate the square of a complex number.  */
complexNum complexSquare (complexNum z)
{
   complexNum zSquare;

   zSquare.x = z.x * z.x - z.y * z.y;
   zSquare.y = 2 * z.x * z.y;
   return zSquare;
}

GLint mandelSqTransf (complexNum z0, GLint maxIter)
{
   complexNum z = z0;
   GLint count = 0;

   /*  Quit when z * z > 4  */
   while ((z.x * z.x + z.y * z.y <= 4.0) && (count < maxIter)) {
      z = complexSquare (z);
      z.x += z0.x;
      z.y += z0.y;
      count++;
   }
   return count;
}

void mandelbrot (GLint nx, GLint ny, GLint maxIter)
{
   complexNum z, zIncr;
   color ptColor;

   GLint iterCount;

   zIncr.x = (GLdouble) complexWidth / nx;
   zIncr.y = (GLdouble) complexHeight / ny;

   for (z.x = xComplexMin;  z.x < xComplexMax;  z.x += zIncr.x)
      for (z.y = yComplexMin;  z.y < yComplexMax;  z.y += zIncr.y) {
         iterCount = mandelSqTransf (z, maxIter);
         if (iterCount >= maxIter)
            /*  Set point color to black.  */
            ptColor.r = ptColor.g = ptColor.b = 0.0;
		 else
		 {
			 GLfloat value = (GLfloat) iterCount / maxIter;
			 
			 ptColor.r = (GLfloat) ( sin( value * 2 + 0.1 ) / 2 + 0.5 );
			 ptColor.g = (GLfloat) ( sin( value * 3 + 0.3 ) / 2 + 0.5 );
			 ptColor.b = (GLfloat) ( sin( value * 5 + 0.5 ) / 2 + 0.5 );
		 }
            //else if (iterCount > (maxIter / 8)) {
            //        /*  Set point color to orange.  */
            //        ptColor.r = 1.0;
            //        ptColor.g = 0.5;
            //        ptColor.b = 0.0;
            //     }
            //     else if (iterCount > (maxIter / 10)) {
            //              /*  Set point color to red.  */
            //              ptColor.r = 1.0;
            //              ptColor.g = ptColor.b = 0.0;
            //          }
            //          else if (iterCount > (maxIter /20)) {
            //                  /*  Set point color to dark blue.  */
            //                  ptColor.b = 0.5;
            //                  ptColor.r = ptColor.g = 0.0;
            //               }
            //               else if (iterCount > (maxIter / 40)) {
            //                       /*  Set point color to yellow.  */
            //                       ptColor.r = ptColor.g = 1.0;
            //                       ptColor.b = 0.0;
            //                    }
            //                    else if (iterCount > (maxIter / 100)) {
            //                            /*  Set point color to dark green.  */
            //                            ptColor.r = ptColor.b = 0.0;
            //                            ptColor.g = 0.3;
            //                         }

            //                         else {
            //                               /*  Set point color to cyan.  */
            //                               ptColor.r = 0.0;
            //                               ptColor.g = ptColor.b = 1.0;
            //                         }
         /*  Plot the color point.  */
         glColor3f (ptColor.r, ptColor.g, ptColor.b);
         plotPoint (z);
    }
}

void displayFcn (void)
{
   /*  Set number of x and y subdivisions and the max iterations.  */
   GLint nx = winWidth, ny = winHeight, maxIter = 100;

   glClear (GL_COLOR_BUFFER_BIT);   //  Clear display window.

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ( );
	gluOrtho2D (xComplexMin, xComplexMax, yComplexMin, yComplexMax);
	glMatrixMode (GL_MODELVIEW);

   mandelbrot (nx, ny, maxIter);
   glFlush ( );
   
   printf("Zoom: %.2f \n", ( 2.5 / complexHeight ));
}

void winReshapeFcn (GLint newWidth, GLint newHeight)
{
	winWidth = newHeight;
	winHeight = newHeight;
   /*  Maintain an aspect ratio of 1.0, assuming that
    *  complexWidth = complexHeight.
    */
   glViewport (0, 0, newHeight, newHeight);

   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ( );

   gluOrtho2D (xComplexMin, xComplexMax, yComplexMin, yComplexMax);

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

void zoom()
{
	// 向下缩放
	// 从窗口到世界坐标系
	GLdouble winx = (GLdouble) newX;
	GLdouble winy = (GLdouble) winHeight - (GLdouble) newY;
	GLdouble winz = 0;
	GLdouble objx = 0;
	GLdouble objy = 0;
	GLdouble objz = 0;
	unProject(winx, winy, winz, &objx, &objy, &objz);
	xComplexMin += (objx - xComplexMin) / 5;
	xComplexMax += (objx - xComplexMax) / 5;
	yComplexMin += (objy - yComplexMin) / 5;
	yComplexMax += (objy - yComplexMax) / 5;
	complexWidth = xComplexMax - xComplexMin;
	complexHeight = yComplexMax - yComplexMin;
	glutPostRedisplay();
}

void mouseButton(int button, int state, int x, int y)
{
	switch (button) {
	case GLUT_LEFT_BUTTON:
		break;
	case GLUT_RIGHT_BUTTON:
		// 重置
		xComplexMin = -2.00; xComplexMax = 0.50;  
		yComplexMin = -1.25; yComplexMax = 1.25;
		complexWidth = xComplexMax - xComplexMin;
		complexHeight = yComplexMax - yComplexMin;
		glutPostRedisplay();
		return;
	default:
		return;
	}
	switch (state) {
	case GLUT_DOWN:
		newX = x;
		newY = y;
		glutIdleFunc(zoom);
		break;
	case GLUT_UP:
		newX = 0;
		newY = 0;
		glutIdleFunc(NULL);
		break;
	}
}

void mouseMotion(int x, int y)
{
	newX = x;
	newY = y;
}

void main (int argc, char** argv)
{
   glutInit (&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
   glutInitWindowPosition (50, 50);
   glutInitWindowSize (winWidth, winHeight);
   glutCreateWindow ("Mandelbrot Set");

   init ( );
   glutDisplayFunc (displayFcn);
   glutReshapeFunc (winReshapeFcn);
   glutMouseFunc (mouseButton);
   glutMotionFunc (mouseMotion);

   glutMainLoop ( );
}
