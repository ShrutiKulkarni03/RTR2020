#include <GL/freeglut.h>

bool bFullscreen=false;


int main(int argc, char** argv)
{
	
	//function declarations
	void initialize(void);
	void resize(int, int);
	void display(void);
	void keyboard(unsigned char, int, int);
	void mouse(int, int, int, int);
	void uninitialize(void);

	//function call
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("GLUT: SHRUTI PARESH KULKARNI");

	initialize();

	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutCloseFunc(uninitialize);

	glutMainLoop();

	return(0);      //this line is not necessary

}


void initialize(void)
{
	
	//code
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);    //black

}


void resize(int width, int height)
{
	
	//code
	if (height <= 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

}

void  display(void)
{

	//code
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBegin(GL_TRIANGLES);

		glColor3f(1.0f,0.0f,0.0f);      //red
		glVertex3f(0.0f,1.0f,0.0f);     //apex

		glColor3f(0.0f,1.0f,0.0f);      //green
		glVertex3f(-1.0f,-1.0f,0.0f);   //left bottom

		glColor3f(0.0f,0.0f,1.0f);      //apex
		glVertex3f(1.0f, -1.0f, 0.0f);  //right bottom

		glEnd();
		glFlush();

}

void keyboard(unsigned char key, int x, int y)
{

	//code
	switch(key)
	{
		case 27:
			glutLeaveMainLoop();
				break;

		case 'f':
		case 'F':

			if (bFullscreen==false)
			{
				glutFullScreen();
				bFullscreen=true;
			}

			else
			{
				glutLeaveFullScreen();
				bFullscreen=false;
			}

				break;
		default:
				break;
	}

}

void mouse(int button, int state, int x, int y)
{

	//code
	switch(button)
	{
		case GLUT_LEFT_BUTTON:
				break;
		case GLUT_RIGHT_BUTTON:
				break;
		default:
				break;
	}

}

void uninitialize(void)
{

	//code

}
