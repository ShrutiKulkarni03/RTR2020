#include <gl/freeglut.h>

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
	glutInitWindowSize(800, 800);
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
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);    //white

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

		//red
		glColor3f(0.9f,0.1f,0.4f);     
		glVertex3f(-0.35f,0.75f,0.0f);     

		glColor3f(0.8f,0.1f,0.3f);    
		glVertex3f(0.4f,0.75f,0.0f);   

		glColor3f(0.9f,0.0f,0.5f);   
		glVertex3f(-0.15f, 0.1f, 0.0f);  

		
		//yellow
		glColor3f(0.9f, 0.6f, 0.0f);
		glVertex3f(0.45f, 0.7f, 0.0f);  

		glColor3f(0.9f, 0.6f, 0.05f);    
		glVertex3f(0.85f, 0.05f, 0.0f);  

		glColor3f(1.0f, 0.8f, 0.0f);    
		glVertex3f(0.0f, 0.15f, 0.0f);  


		//green
		glColor3f(0.4f, 0.6f, 0.1f);     
		glVertex3f(0.85f, -0.02f, 0.0f);  

		glColor3f(0.4, 0.7f, 0.1f);  
		glVertex3f(0.45f, -0.7f, 0.0f);  

		glColor3f(0.5f, 0.8f, 0.1f);   
		glVertex3f(0.15f, 0.06f, 0.0f); 

		
		//cyan
		glColor3f(0.0f, 0.6f, 1.0f);     
		glVertex3f(0.38f, -0.75f, 0.0f); 

		glColor3f(0.3f, 0.3f, 0.8f);     
		glVertex3f(-0.4f, -0.75f, 0.0f);  

		glColor3f(0.0f, 0.6f, 0.7f);     
		glVertex3f(0.11f, -0.05f, 0.0f);


		//blue
		glColor3f(0.07f, 0.1f, 0.3f);      
		glVertex3f(-0.46f, -0.71f, 0.0f);   

		glColor3f(0.1f, 0.2f, 0.4f);     
		glVertex3f(-0.85f, 0.0f, 0.0f);   

		glColor3f(0.2f, 0.3f, 0.7f);      
		glVertex3f(-0.05f, -0.15f, 0.0f);  


		//magenta
		glColor3f(0.5f, 0.05f, 0.6f);    
		glVertex3f(-0.85f, 0.07f, 0.0f);  

		glColor3f(0.7f, 0.08f, 0.9f);    
		glVertex3f(-0.43f, 0.75f, 0.0f);  

		glColor3f(0.8f, 0.3f, 1.0f);   
		glVertex3f(-0.18f, -0.05f, 0.0f);  


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
