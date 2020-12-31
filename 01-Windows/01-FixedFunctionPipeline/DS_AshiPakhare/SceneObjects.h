#pragma once
#define PI 3.1415


//Struct

struct COLOR
{
	GLfloat R, G, B;
};

struct POSITION
{
	GLfloat X, Y, Z;
};



/*****GLOBAL VARIABLE DECLARATION*****/

GLUquadric* quadric = NULL;


//SUN

GLfloat SunTheta = 0.0f;
//COLOR SunColor = { 0.96f, 0.7f, 0.18f };
COLOR SunColor = { 1.0f, 0.49f, 0.15f };
POSITION SunTranslate = { 0.0f, -2.0f, -25.5 };


//SKY

COLOR SkyTop = { 0.38f, 0.55f, 1.0f };
COLOR SkyMid = { 0.47f, 0.56f, 0.96f };
COLOR SkyBottom = { 0.99f, 0.72f, 0.12f };



//BIRDS

GLfloat BirdsScale = 0.3f;

//blue

GLfloat Bird1RotateAngle = 40.0f;
GLfloat Bird1WingsRotate = 165.0f;
POSITION Bird1Translate = { 5.3f, 0.1f, -3.1f };


//red

GLfloat Bird2RotateAngle = 30.0f;
GLfloat Bird2WingsRotate = 165.0f;
POSITION Bird2Translate = { 10.5f, 3.5f, 5.0f };

//purple

GLfloat Bird3RotateAngle = 180.0f;
GLfloat Bird3WingsRotate = 165.0f;
POSITION Bird3Translate = { -13.0f, 6.0f, 0.0f };

//orange

GLfloat Bird4RotateAngle = 30.0f;
GLfloat Bird4WingsRotate = 165.0f;
POSITION Bird4Translate = { 10.5f, 3.5f, 5.0f };


//GIRLS

GLfloat CircleTheta = 0.0f;

GLfloat GirlsScale = 0.4f;

//Girl1

GLfloat Girl1RotateAngle = 80.0f;
GLfloat Girl1RtShoulderRotate = 290.0f;
GLfloat Girl1LtShoulderRotate = 250.0f;
POSITION Girl1Translate = { -4.0f, -0.1f, 2.0f };

//Girl2

GLfloat Girl2RotateAngle = 210.0f;
GLfloat Girl2RtShoulderRotate = 270.0f;
GLfloat Girl2LtShoulderRotate = 270.0f;
POSITION Girl2Translate = { 2.0f, -0.2f, 15.0f };



//update() speed variables

GLfloat FadeInQuadAlphaSpeed = 0.1f;
GLfloat LookAtTranslateSpeed = 0.1f;
GLfloat SunTranslateSpeed = 0.01f;
GLfloat SunColorAnimateSpeed = 0.1f;
GLfloat SkyColorAnimateSpeed = 0.0001f;
GLfloat BirdsTranslateSpeed = 0.02f;
GLfloat BirdsRotateSpeed = 0.05f;
GLfloat BirdsWingsSpeed = 3.0f;
GLfloat GirlsTranslateSpeed = 0.0055f;
GLfloat GirlsRotateSpeed = 1.5f;
GLfloat GirlsHandsRotateSpeed = 0.55f;
GLfloat GirlsHeadRotateSpeed = 0.1f;
GLfloat RiceAlphaSpeed = 0.1f;
GLfloat TimerSpeed = 0.005f;
GLfloat HandSwingMax = 290.0f;
GLfloat HandSwingMin = 250.0f;
GLfloat WingSwingMax = 165.0f;
GLfloat WingSwingMin = 30.0f;


bool isGirl1Visible = false;
bool isGirl2Visible = false;
bool isRiceFeed = false;
bool isGirl1Still = true;
bool isGirl2Still = true;
bool isBird1Still = true;
bool isBird2Still = true;
bool isBird3Still = true;
bool isBird4Still = true;

bool isScene1 = false;
bool isScene2 = false;
bool isScene3 = false;
bool isScene4 = false;
bool isScene5 = false;

GLfloat Timer = -2.0f;
GLfloat LookAtTranslateY = 0.0f;
GLfloat FadeInQuadAlpha = 0.0f;
GLfloat RiceAlpha = 0.0f;



//TEXTURES

GLuint tree1_texture;
GLuint tree2_texture;
GLuint tree_bark1_texture;
GLuint tree_bark2_texture;
GLuint top1_texture;
GLuint skirt1_texture;
GLuint skirt2_texture;
GLuint top2_texture;
GLuint mountain_texture;
GLuint grass_texture;
GLuint concrete_texture;
GLuint wall_texture;
GLuint front_wall_texture;
GLuint railing_texture;
GLuint roof_texture;



//Update Switch Case Variables

int RightHandLoop = 0;
int LeftHandLoop = 0;
int B2WingsRot = 0;
int Scene = 0;
int BirdBlockings = 0;
int Girl1Movements = 0;
int Girl2Movements = 0;




//Draw Objects

void Sun(void)
{
	glBegin(GL_POLYGON);

	//glNormal3f(0.0f, 0.0f, 1.0f);

	glColor3f(SunColor.R, SunColor.G, SunColor.B);
	
	for (SunTheta = 0.0f; SunTheta <= 2 * PI; SunTheta += 0.01f)
	{
		glVertex3f(0.5f * cos(SunTheta), 0.5f * sin(SunTheta), 0.0);
	}

	glColor3f(1.0f, 1.0f, 1.0f);

	glEnd();
}



void Sky(void)
{

	/******SKY******/


	glBegin(GL_QUADS);

		
	glColor3f(SkyTop.R, SkyTop.G, SkyTop.B);

	glVertex3f(35.0f, 50.0f, -26.0f);
	glVertex3f(-35.0f, 50.0f, -26.0f);

	
	glColor3f(SkyMid.R, SkyMid.G, SkyMid.B);

	glVertex3f(-35.0f, 20.0f, -26.0f);
	glVertex3f(35.0f, 20.0f, -26.0f);


	glColor3f(SkyMid.R, SkyMid.G, SkyMid.B);
	

	glVertex3f(35.0f, 20.0f, -26.0f);
	glVertex3f(-35.0f, 20.0f, -26.0f);

	
	glColor3f(SkyBottom.R, SkyBottom.G, SkyBottom.B);

	glVertex3f(-35.0f, -2.5f, -26.0f);
	glVertex3f(35.0f, -2.5f, -26.0f);

	glColor3f(1.0f, 1.0f, 1.0f);

	glEnd();

}



void Floor(void)
{
	/*****FLOOR*****/

	glBindTexture(GL_TEXTURE_2D, grass_texture);
	
	glBegin(GL_QUADS);
	
	glNormal3f(0.0f, 1.0f, 0.0f);

	glColor3f(0.87f, 0.86f, 0.66f);

	glTexCoord2f(20.0f, 20.0f);
	glVertex3f(40.0f, -2.0f, -25.0f);

	glTexCoord2f(0.0f, 20.0f);
	glVertex3f(-40.0f, -2.0f, -25.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-40.0f, -2.0f, 15.0f);

	glTexCoord2f(20.0f, 0.0f);
	glVertex3f(40.0f, -2.0f, 15.0f);
	
	glColor3f(1.0f, 1.0f, 1.0f);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
}



void House(void)
{
	/******CHIMNEY******/

	glBindTexture(GL_TEXTURE_2D, wall_texture);
	
	glBegin(GL_QUADS);
	
	glColor3f(1.0f, 1.0f, 1.0f);


	//front
		
	glNormal3f(0.0f, 0.0f, 1.0f);

	glTexCoord2f(0.3f, 0.3f);
	glVertex3f(1.5f, 3.5f, 0.5f);

	glTexCoord2f(0.0f, 0.3f);
	glVertex3f(1.0f, 3.5f, 0.5f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, 3.0f, 0.5f);

	glTexCoord2f(0.3f, 0.0f);
	glVertex3f(1.5f, 3.0f, 0.5f);


	//right

	glNormal3f(1.0f, 0.0f, 0.0f);

	glTexCoord2f(0.0f, 0.3f);
	glVertex3f(1.5f, 3.5f, 0.0f);

	glTexCoord2f(0.3f, 0.3f);
	glVertex3f(1.5f, 3.5f, 0.5f);

	glTexCoord2f(0.3f, 0.0f);
	glVertex3f(1.5f, 3.0f, 0.5f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.5f, 3.0f, 0.0f);


	//back

	glNormal3f(0.0f, 0.0f, -1.0f);

	glTexCoord2f(0.3f, 0.3f);
	glVertex3f(1.0f, 3.5f, 0.0f);

	glTexCoord2f(0.0f, 0.3f);
	glVertex3f(1.5f, 3.5f, 0.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.5f, 3.0f, 0.0f);

	glTexCoord2f(0.3f, 0.0f);
	glVertex3f(1.0f, 3.0f, 0.0f);


	//left

	glNormal3f(-1.0f, 0.0f, 0.0f);

	glTexCoord2f(0.0f, 0.3f);
	glVertex3f(1.0f, 3.5f, 0.0f);

	glTexCoord2f(0.3f, 0.3f);
	glVertex3f(1.0f, 3.5f, 0.5f);

	glTexCoord2f(0.3f, 0.0f);
	glVertex3f(1.0f, 3.0f, 0.5f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, 3.0f, 0.0f);


	//top

	glNormal3f(0.0f, 1.0f, 0.0f);

	glTexCoord2f(0.3f, 0.3f);
	glVertex3f(1.5f, 3.5f, 0.0f);

	glTexCoord2f(0.0f, 0.3f);
	glVertex3f(1.0f, 3.5f, 0.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, 3.5f, 0.5f);

	glTexCoord2f(0.3f, 0.0f);
	glVertex3f(1.5f, 3.5f, 0.5f);


	glColor3f(1.0f, 1.0f, 1.0f);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);



	/*******ROOF******/

	glBindTexture(GL_TEXTURE_2D, roof_texture);

	glBegin(GL_QUADS);

	//left

	glColor3f(0.68f, 0.58f, 0.56f);

	glNormal3f(2.0f, 1.0f, 0.0f);

	glTexCoord2f(0.6f, 1.0f);
	glVertex3f(-2.0f, 3.0f, 1.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-2.0f, 3.0f, -1.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-2.5f, 2.0f, -1.5f);

	glTexCoord2f(0.6f, 0.0f);
	glVertex3f(-2.5f, 2.0f, 1.5f);

	//front

	//glColor3f(0.63f, 0.26f, 0.19f);

	glNormal3f(0.0f, 2.0f, 4.0f);

	glTexCoord2f(0.9f, 1.0f);
	glVertex3f(2.0f, 3.0f, 1.0f);

	glTexCoord2f(0.1f, 1.0f);
	glVertex3f(-2.0f, 3.0f, 1.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-2.5f, 2.0f, 1.5f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(2.5f, 2.0f, 1.5f);

	//right

	//glColor3f(0.63f, 0.25f, 0.18f);

	glNormal3f(2.0f, 1.0f, 0.0f);

	glTexCoord2f(0.6f, 1.0f);
	glVertex3f(2.0f, 3.0f, -1.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(2.0f, 3.0f, 1.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(2.5f, 2.0f, 1.5f);

	glTexCoord2f(0.6f, 0.0f);
	glVertex3f(2.5f, 2.0f, -1.5f);

	//back

	glNormal3f(0.0f, 4.0f, -2.0f);

	//glColor3f(0.63f, 0.26f, 0.19f);

	glTexCoord2f(0.9f, 1.0f);
	glVertex3f(-2.0f, 3.0f, -1.0f);

	glTexCoord2f(0.1f, 1.0f);
	glVertex3f(2.0f, 3.0f, -1.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(2.5f, 2.0f, -1.5f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-2.5f, 2.0f, -1.5f);

	//bottom

	glNormal3f(0.0f, -1.0f, 0.0f);

	glColor3f(0.78f, 0.68f, 0.66f);

	//glTexCoord2f(1.0f, 1.0f);
	glVertex3f(2.5f, 2.0f, -1.5f);

	//glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-2.5f, 2.0f, -1.5f);

	//glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-2.5f, 2.0f, 1.5f);

	//glTexCoord2f(1.0f, 0.0f);
	glVertex3f(2.5f, 2.0f, 1.5f);

	//top

	glNormal3f(0.0f, 1.0f, 0.0f);

	//glColor3f(0.63f, 0.26f, 0.19f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(2.0f, 3.0f, -1.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-2.0f, 3.0f, -1.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-2.0f, 3.0f, 1.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(2.0f, 3.0f, 1.0f);

	glColor3f(1.0f, 1.0f, 1.0f);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);


	/*******WALLS******/

	glBindTexture(GL_TEXTURE_2D, front_wall_texture);

	glBegin(GL_QUADS);

	//front

	glNormal3f(0.0f, 0.0f, 1.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	//glColor3f(0.85f, 0.83f, 0.78f);

	glTexCoord2f(1.0f, 0.85f);
	glVertex3f(2.0f, 2.0f, 1.0f);

	glTexCoord2f(0.0f, 0.85f);
	glVertex3f(-2.0f, 2.0f, 1.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-2.0f, 0.2f, 1.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(2.0f, 0.2f, 1.0f);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);



	glBindTexture(GL_TEXTURE_2D, wall_texture);

	glBegin(GL_QUADS);

	//right

	glNormal3f(1.0f, 0.0f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(2.0f, 2.0f, -1.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(2.0f, 2.0f, 1.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(2.0f, 0.2f, 1.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(2.0f, 0.2f, -1.0f);

	//back

	glNormal3f(0.0f, 0.0f, -1.0f);

	//glColor3f(0.9f, 0.9f, 0.5f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(2.0f, 2.0f, -1.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-2.0f, 2.0f, -1.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-2.0f, 0.2f, -1.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(2.0f, 0.2f, -1.0f);

	//left

	glNormal3f(-1.0f, 0.0f, 0.0f);

	//glColor3f(0.9f, 0.9f, 0.45f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-2.0f, 2.0f, 1.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-2.0f, 2.0f, -1.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-2.0f, 0.2f, -1.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-2.0f, 0.2f, 1.0f);

	glColor3f(1.0f, 1.0f, 1.0f);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);


	/*******PLATFORM******/

	glBindTexture(GL_TEXTURE_2D, concrete_texture);

	glBegin(GL_QUADS);

	//top

	glNormal3f(0.0f, 1.0f, 0.0f);

	glColor3f(0.7f, 0.7f, 0.7f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(2.5f, 0.2f, -3.5f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-2.5f, 0.2f, -3.5f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-2.5f, 0.2f, 3.5f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(2.5f, 0.2f, 3.5f);

	//front

	glNormal3f(0.0f, 0.0f, 1.0f);

	//glColor3f(0.4f, 0.4f, 0.4f);

	glTexCoord2f(1.0f, 0.2f);
	glVertex3f(2.5f, 0.2f, 3.5f);

	glTexCoord2f(0.0f, 0.2f);
	glVertex3f(-2.5f, 0.2f, 3.5f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-2.5f, 0.0f, 3.5f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(2.5f, 0.0f, 3.5f);

	//right

	glNormal3f(1.0f, 0.0f, 0.0f);

	//glColor3f(0.4f, 0.4f, 0.4f);

	glTexCoord2f(0.0f, 0.5f);
	glVertex3f(2.5f, 0.2f, -3.5f);

	glTexCoord2f(0.5f, 0.5f);
	glVertex3f(2.5f, 0.2f, 3.5f);

	glTexCoord2f(0.5f, 0.0f);
	glVertex3f(2.5f, 0.0f, 3.5f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(2.5f, 0.0f, -3.5f);

	//back

	glNormal3f(0.0f, 0.0f, -1.0f);

	//glColor3f(0.4f, 0.4f, 0.4f);

	glTexCoord2f(0.5f, 0.5f);
	glVertex3f(-2.5f, 0.2f, -3.0f);

	glTexCoord2f(0.0f, 0.5f);
	glVertex3f(2.5f, 0.2f, -3.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(2.5f, 0.0f, -3.0f);

	glTexCoord2f(0.5f, 0.0f);
	glVertex3f(-2.5f, 0.0f, -3.0f);

	//left

	glNormal3f(-1.0f, 0.0f, 0.0f);

	//glColor3f(0.4f, 0.4f, 0.4f);

	glTexCoord2f(0.0f, 0.2f);
	glVertex3f(-2.5f, 0.2f, 3.5f);

	glTexCoord2f(1.0f, 0.2f);
	glVertex3f(-2.5f, 0.2f, -3.5f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-2.5f, 0.0f, -3.5f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-2.5f, 0.0f, 3.5f);

	glColor3f(1.0f, 1.0f, 1.0f);


	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);

}



void Mountains(void)
{
	glBindTexture(GL_TEXTURE_2D, mountain_texture);

	glBegin(GL_TRIANGLES);

	//front

	glNormal3f(0.0f, 0.447214f, 0.894427f);

	glColor3f(0.7f, 0.57f, 0.51f);

	glTexCoord2f(2.0f, 2.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);	

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.5f);

	glTexCoord2f(4.0f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.5f);

	//right

	glNormal3f(0.894427f, 0.447214f, 0.0f);

	glColor3f(0.68f, 0.54f, 0.48f);

	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.5f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.5f, -0.5f, -0.5f);

	//back

	glNormal3f(0.0f, 0.447214f, -0.894427f);

	glColor3f(0.7f, 0.57f, 0.51f);

	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.5f, -0.5f, -0.5f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.5f, -0.5f, -0.5f);


	//left

	glNormal3f(-0.894427f, 0.447214f, 0.0f);

	glColor3f(0.68f, 0.54f, 0.48f);

	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.5f, -0.5f, -0.5f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.5f);

	glColor3f(1.0f, 1.0f, 1.0f);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
}



void RailingBlock(void)
{

	glBindTexture(GL_TEXTURE_2D, railing_texture);

	glBegin(GL_QUADS);

	/*******RAILING******/

	//front

	glNormal3f(0.0f, 0.0f, 1.0f);

	glColor3f(0.9f, 0.9f, 0.9f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(10.2f, 1.5f, 0.2f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(10.0f, 1.5f, 0.2f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(10.0f, 0.0f, 0.2f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(10.2f, 0.0f, 0.2f);

	//right

	glNormal3f(1.0f, 0.0f, 0.0f);

	glColor3f(0.8f, 0.8f, 0.8f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(10.2f, 1.5f, 0.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(10.2f, 1.5f, 0.2f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(10.2f, 0.0f, 0.2f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(10.2f, 0.0f, 0.0f);

	//back

	glNormal3f(0.0f, 0.0f, -1.0f);

	glColor3f(0.9f, 0.9f, 0.9f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(10.0f, 1.5f, 0.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(10.2f, 1.5f, 0.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(10.2f, 0.0f, 0.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(10.0f, 0.0f, 0.0f);

	//left

	glNormal3f(-1.0f, 0.0f, 0.0f);

	glColor3f(0.8f, 0.8f, 0.8f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(10.0f, 1.5f, 0.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(10.0f, 1.5f, 0.2f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(10.0f, 0.0f, 0.2f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(10.0f, 0.0f, 0.0f);

	//top

	glNormal3f(0.0f, 1.0f, 0.0f);

	glColor3f(0.8f, 0.8f, 0.8f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(10.2f, 1.5f, 0.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(10.0f, 1.5f, 0.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(10.0f, 1.5f, 0.2f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(10.2f, 1.5f, 0.2f);

	//bottom

	glNormal3f(0.0f, -1.0f, 0.0f);

	glColor3f(0.8f, 0.8f, 0.8f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(10.2f, 1.0f, 0.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(10.0f, 1.0f, 0.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(10.0f, 1.0f, 0.2f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(10.2f, 1.0f, 0.2f);

	glColor3f(1.0f, 1.0f, 1.0f);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
}



void Tree(void)
{

	/****TREE1****/


	//Big Sphere

	glTranslatef(5.0f, 2.5f, -4.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

	//glColor3f(0.0f, 0.6f, 0.09f);

	glBindTexture(GL_TEXTURE_2D, tree1_texture);

	quadric = gluNewQuadric();

	gluQuadricTexture(quadric, GL_TRUE);
	gluSphere(quadric, 1.8f, 30, 30);   //tree1

	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
	glPushMatrix();



	//Small Sphere

	glTranslatef(7.0f, 0.4f, -2.5f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

	//glColor3f(0.0f, 0.6f, 0.09f);
	glBindTexture(GL_TEXTURE_2D, tree1_texture);

	quadric = gluNewQuadric();

	gluQuadricTexture(quadric, GL_TRUE);
	gluSphere(quadric, 1.0f, 20, 20);   //tree1

	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
	glPushMatrix();



	/****TRUNK****/

	//Big Trunk

	glTranslatef(5.0f, -2.0f, -4.0f);
	glRotatef(270.0f, 1.0f, 0.0f, 0.0f);

	//glColor3f(0.64f, 0.44f, 0.05f);
	glBindTexture(GL_TEXTURE_2D, tree_bark1_texture);

	quadric = gluNewQuadric();

	gluQuadricTexture(quadric, GL_TRUE);
	gluCylinder(quadric, 0.3f, 0.2f, 3.0f, 30, 30);   //tree1
	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
	glPushMatrix();


	//Small Trunk


	glRotatef(50.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(6.0f, -0.5f, 1.5f);
	glRotatef(340.0f, 1.0f, 0.0f, 0.0f);

	//glColor3f(0.64f, 0.44f, 0.05f);
	glBindTexture(GL_TEXTURE_2D, tree_bark1_texture);

	quadric = gluNewQuadric();

	gluQuadricTexture(quadric, GL_TRUE);
	gluCylinder(quadric, 0.2f, 0.1f, 2.0f, 30, 30);   //tree1

	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
	glPushMatrix();



	/****TREE2****/

	//Sphere

	glTranslatef(8.0f, 4.0f, -6.5f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

	//glColor3f(0.0f, 0.4f, 0.09f);
	glBindTexture(GL_TEXTURE_2D, tree2_texture);

	quadric = gluNewQuadric();

	gluQuadricTexture(quadric, GL_TRUE);
	gluSphere(quadric, 2.7f, 20, 20);   //tree1

	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
	glPushMatrix();



	//Trunk

	glTranslatef(8.0f, -2.0f, -6.5f);
	glRotatef(270.0f, 1.0f, 0.0f, 0.0f);

	//glColor3f(0.54f, 0.34f, 0.05f);
	glBindTexture(GL_TEXTURE_2D, tree_bark2_texture);

	quadric = gluNewQuadric();

	gluQuadricTexture(quadric, GL_TRUE);
	gluCylinder(quadric, 0.4f, 0.3f, 4.0f, 30, 30);   //tree1

	glBindTexture(GL_TEXTURE_2D, 0);

	glColor3f(1.0f, 1.0f, 1.0f);

	//glPopMatrix();
	//glPushMatrix();


}



void Girl1(void)
{
	
	glTranslatef(Girl1Translate.X, Girl1Translate.Y, Girl1Translate.Z);      //full body translate
	glScalef(GirlsScale, GirlsScale, GirlsScale);    //Whole girl scale
	glPushMatrix();


	/*****HEAD*****/


	glRotatef(Girl1RotateAngle, 0.0f, 1.0f, 0.0f);   //full body rotation


	glColor3f(0.96f, 0.74f, 0.6f);

	glTranslatef(0.0f, -0.02f, 0.0f);      //head position

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.47f, 20, 20);

	glColor3f(1.0f, 1.0f, 1.0f);

	glPopMatrix();
	glPushMatrix();


	/*****HAIR*****/


	glRotatef(Girl1RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation

	glTranslatef(0.0f, 0.0f, -0.1f);     //position	


	glColor3f(0.1f, 0.1f, 0.1f);

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.5f, 20, 20);

	glColor3f(1.0f, 1.0f, 1.0f);

	glPopMatrix();
	glPushMatrix();


	//BUN


	glRotatef(Girl1RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation

	glTranslatef(0.0f, 0.2f, -0.6f);     //position	


	glColor3f(0.1f, 0.1f, 0.1f);

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.22f, 20, 20);

	glColor3f(1.0f, 1.0f, 1.0f);

	glPopMatrix();
	glPushMatrix();




	/*****EYES*****/


	glRotatef(Girl1RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation

	glTranslatef(-0.15f, 0.05f, 0.44f);     //position	


	glColor3f(0.1f, 0.1f, 0.1f);

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.04f, 5, 5);

	glColor3f(1.0f, 1.0f, 1.0f);

	glPopMatrix();
	glPushMatrix();





	glRotatef(Girl1RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation

	glTranslatef(0.15f, 0.05f, 0.44f);     //position	


	glColor3f(0.1f, 0.1f, 0.1f);

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.04f, 5, 5);

	glColor3f(1.0f, 1.0f, 1.0f);

	glPopMatrix();
	glPushMatrix();




	/*****NECK*****/


	glTranslatef(0.0f, -0.67f, 0.0f);       //position
	glRotatef(270.0f, 1.0f, 0.0f, 0.0f);    //poles


	glRotatef(Girl1RotateAngle, 0.0f, 0.0f, 1.0f);     //full body rotation


	glColor3f(0.96f, 0.74f, 0.6f);

	quadric = gluNewQuadric();

	gluCylinder(quadric, 0.14f, 0.14f, 0.25f, 30, 30);

	glColor3f(1.0f, 1.0f, 1.0f);

	glPopMatrix();
	glPushMatrix();



	/*****ABDOMEN/TOP*****/


	glTranslatef(0.0f, -1.8f, 0.0f);     //position
	glRotatef(270.0f, 1.0f, 0.0f, 0.0f);   //poles

	glRotatef(Girl1RotateAngle, 0.0f, 0.0f, 1.0f);     //full body rotation

	glColor3f(1.0f, 1.0f, 1.0f);

	glBindTexture(GL_TEXTURE_2D, top1_texture);

	quadric = gluNewQuadric();

	gluQuadricTexture(quadric, GL_TRUE);
	gluCylinder(quadric, 0.4f, 0.6f, 1.15f, 30, 30);

	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
	glPushMatrix();




	//Circle

	glRotatef(Girl1RotateAngle, 0.0f, 1.0f, 0.0f);   //full body rotation

	glTranslatef(0.0f, -0.7f, 0.0f);
	glRotatef(90.0, 1.0f, 0.0f, 0.0f);

	glBegin(GL_POLYGON);

	glNormal3f(0.0f, 0.0f, 1.0f);

	glColor3f(0.77f, 0.4f, 0.39f);

	for (CircleTheta = 0.0f; CircleTheta <= 2 * PI; CircleTheta += 0.01f)
	{
		glVertex3f(0.6f * cos(CircleTheta), 0.6f * sin(CircleTheta), 0.0);
	}

	glColor3f(1.0f, 1.0f, 1.0f);

	glEnd();

	glPopMatrix();
	glPushMatrix();




	/******HANDS******/


	//RIGHT


	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);    //shoulder original position
	glRotatef(Girl1RotateAngle, 0.0f, 1.0f, 0.0f);   //full body rotation


	glTranslatef(0.0f, -0.75f, 0.62f);

	glPushMatrix();


	glRotatef(Girl1RtShoulderRotate, 0.0f, 0.0f, 1.0f);   //shoulder rotation
	glTranslatef(0.5f, 0.0f, 0.0f);

	glPushMatrix();

	glScalef(1.0f, 0.22f, 0.22f);

	//glColor3f(0.72f, 0.17f, 0.09f);

	glBindTexture(GL_TEXTURE_2D, top1_texture);

	quadric = gluNewQuadric();

	gluQuadricTexture(quadric, GL_TRUE);
	gluSphere(quadric, 0.5f, 10, 10);

	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();




	glTranslatef(0.3f, 0.0f, 0.0f);

	glRotatef(0.0f, 0.0f, 0.0f, 1.0f);    //elbow rotation

	glTranslatef(0.5f, 0.0f, 0.0f);

	glScalef(0.8f, 0.21f, 0.21f);

	glColor3f(0.96f, 0.74f, 0.6f);

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.5f, 10, 10);

	glColor3f(1.0f, 1.0f, 1.0f);

	glPopMatrix();

	glPopMatrix();
	glPushMatrix();



	//LEFT


	glRotatef(-90.0, 0.0f, 1.0f, 0.0f);    //shoulder original position
	glRotatef(Girl1RotateAngle, 0.0f, 1.0f, 0.0f);   //full body rotation


	glTranslatef(0.0f, -0.75f, -0.62f);

	glPushMatrix();


	glRotatef(Girl1LtShoulderRotate, 0.0f, 0.0f, 1.0f);   //shoulder rotation
	glTranslatef(0.5f, 0.0f, 0.0f);

	glPushMatrix();

	glScalef(1.0f, 0.22f, 0.22f);

	//glColor3f(0.72f, 0.17f, 0.09f);

	glBindTexture(GL_TEXTURE_2D, top1_texture);

	quadric = gluNewQuadric();

	gluQuadricTexture(quadric, GL_TRUE);
	gluSphere(quadric, 0.5f, 10, 10);

	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();




	glTranslatef(0.3f, 0.0f, 0.0f);

	glRotatef(0.0f, 0.0f, 0.0f, 1.0f);    //elbow rotation

	glTranslatef(0.5f, 0.0f, 0.0f);

	glScalef(0.8f, 0.21f, 0.21f);

	glColor3f(0.96f, 0.74f, 0.6f);

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.5f, 10, 10);

	glColor3f(1.0f, 1.0f, 1.0f);

	glPopMatrix();

	glPopMatrix();
	glPushMatrix();




	/*****LEGS/SKIRT*****/


	glTranslatef(0.0f, -3.8f, 0.0f);    //position
	glRotatef(270.0f, 1.0f, 0.0f, 0.0f);    //poles

	glRotatef(Girl1RotateAngle, 0.0f, 0.0f, 1.0f);     //full body rotation

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glColor3f(0.1f, 0.32f, 0.47f);

	glBindTexture(GL_TEXTURE_2D, skirt1_texture);

	
	quadric = gluNewQuadric();

	gluQuadricTexture(quadric, GL_TRUE);
	gluCylinder(quadric, 0.8f, 0.4f, 2.1f, 30, 30);

	glBindTexture(GL_TEXTURE_2D, 0);

	glColor3f(1.0f, 1.0f, 1.0f);

	glPopMatrix();
	glPopMatrix();
	glPushMatrix();
	glPopMatrix();


}



void Girl2(void)
{

	glTranslatef(Girl2Translate.X, Girl2Translate.Y, Girl2Translate.Z);     //full body translate
	glScalef(GirlsScale, GirlsScale, GirlsScale);    //Whole girl scale
	glPushMatrix();


	/*****HEAD*****/


	glRotatef(Girl2RotateAngle, 0.0f, 1.0f, 0.0f);   //full body rotation

	glColor3f(0.96f, 0.74f, 0.6f);

	glTranslatef(0.0f, -0.02f, 0.0f);      //head position

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.47f, 20, 20);

	glColor3f(1.0f, 1.0f, 1.0f);

	glPopMatrix();
	glPushMatrix();


	/*****HAIR*****/


	glRotatef(Girl2RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation

	glTranslatef(0.0f, 0.0f, -0.1f);     //position	

	glColor3f(0.1f, 0.1f, 0.1f);

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.5f, 20, 20);

	glColor3f(1.0f, 1.0f, 1.0f);

	glPopMatrix();
	glPushMatrix();


	//BUN


	glRotatef(Girl2RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation

	glTranslatef(0.0f, 0.2f, -0.6f);     //position	

	glColor3f(0.1f, 0.1f, 0.1f);

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.22f, 20, 20);

	glColor3f(1.0f, 1.0f, 1.0f);

	glPopMatrix();
	glPushMatrix();




	/*****EYES*****/

	glRotatef(Girl2RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation

	glTranslatef(-0.15f, 0.05f, 0.44f);     //position	

	glColor3f(0.1f, 0.1f, 0.1f);

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.04f, 5, 5);

	glColor3f(1.0f, 1.0f, 1.0f);

	glPopMatrix();
	glPushMatrix();





	glRotatef(Girl2RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation

	glTranslatef(0.15f, 0.05f, 0.44f);     //position	

	glColor3f(0.1f, 0.1f, 0.1f);

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.04f, 5, 5);

	glColor3f(1.0f, 1.0f, 1.0f);

	glPopMatrix();
	glPushMatrix();



	/*****NECK*****/

	glTranslatef(0.0f, -0.67f, 0.0f);       //position
	glRotatef(270.0f, 1.0f, 0.0f, 0.0f);    //poles

	glRotatef(Girl2RotateAngle, 0.0f, 0.0f, 1.0f);     //full body rotation

	glColor3f(0.96f, 0.74f, 0.6f);

	quadric = gluNewQuadric();

	gluCylinder(quadric, 0.14f, 0.14f, 0.25f, 30, 30);

	glColor3f(1.0f, 1.0f, 1.0f);

	glPopMatrix();
	glPushMatrix();



	/*****ABDOMEN/TOP*****/


	glTranslatef(0.0f, -1.8f, 0.0f);     //position
	glRotatef(270.0f, 1.0f, 0.0f, 0.0f);   //poles


	glRotatef(Girl2RotateAngle, 0.0f, 0.0f, 1.0f);     //full body rotation

	glColor3f(0.95f, 0.94f, 0.87f);
	
	glBindTexture(GL_TEXTURE_2D, top2_texture);

	quadric = gluNewQuadric();

	gluQuadricTexture(quadric, GL_TRUE);
	gluCylinder(quadric, 0.4f, 0.6f, 1.15f, 30, 30);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glColor3f(1.0f, 1.0f, 1.0f);

	glPopMatrix();
	glPushMatrix();




	//Circle

	glRotatef(Girl2RotateAngle, 0.0f, 1.0f, 0.0f);   //full body rotation

	glTranslatef(0.0f, -0.65f, 0.0f);
	glRotatef(90.0, 1.0f, 0.0f, 0.0f);

	glBegin(GL_POLYGON);

	glNormal3f(0.0f, 0.0f, 1.0f);

	glColor3f(0.93f, 0.9f, 0.78f);

	for (CircleTheta = 0.0f; CircleTheta <= 2 * PI; CircleTheta += 0.01f)
	{
		glVertex3f(0.6f * cos(CircleTheta), 0.6f * sin(CircleTheta), 0.0);
	}

	glColor3f(1.0f, 1.0f, 1.0f);

	glEnd();


	glPopMatrix();
	glPushMatrix();




	/******HANDS******/


	//RIGHT

	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);    //shoulder original position
	glRotatef(Girl2RotateAngle, 0.0f, 1.0f, 0.0f);   //full body rotation


	glTranslatef(0.0f, -0.75f, 0.62f);

	glPushMatrix();

	glRotatef(Girl2RtShoulderRotate, 0.0f, 0.0f, 1.0f);   //shoulder rotation
	glTranslatef(0.5f, 0.0f, 0.0f);

	glPushMatrix();

	glScalef(1.0f, 0.22f, 0.22f);

	glColor3f(0.96f, 0.74f, 0.6f);

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.5f, 10, 10);

	glColor3f(1.0f, 1.0f, 1.0f);


	glPopMatrix();




	glTranslatef(0.3f, 0.0f, 0.0f);

	glRotatef(0.0f, 0.0f, 0.0f, 1.0f);    //elbow rotation

	glTranslatef(0.5f, 0.0f, 0.0f);

	glScalef(0.8f, 0.21f, 0.21f);

	glColor3f(0.96f, 0.74f, 0.6f);

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.5f, 10, 10);

	glColor3f(1.0f, 1.0f, 1.0f);

	glPopMatrix();

	glPopMatrix();
	glPushMatrix();



	//LEFT


	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);    //shoulder original position
	glRotatef(Girl2RotateAngle, 0.0f, 1.0f, 0.0f);   //full body rotation

	glTranslatef(0.0f, -0.75f, -0.62f);

	glPushMatrix();

	glRotatef(Girl2LtShoulderRotate, 0.0f, 0.0f, 1.0f);   //shoulder rotation
	glTranslatef(0.5f, 0.0f, 0.0f);

	glPushMatrix();

	glScalef(1.0f, 0.22f, 0.22f);

	glColor3f(0.96f, 0.74f, 0.6f);

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.5f, 10, 10);

	glColor3f(1.0f, 1.0f, 1.0f);


	glPopMatrix();




	glTranslatef(0.3f, 0.0f, 0.0f);

	glRotatef(0.0f, 0.0f, 0.0f, 1.0f);    //elbow rotation

	glTranslatef(0.5f, 0.0f, 0.0f);

	glScalef(0.8f, 0.21f, 0.21f);

	glColor3f(0.96f, 0.74f, 0.6f);

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.5f, 10, 10);

	glColor3f(1.0f, 1.0f, 1.0f);

	glPopMatrix();

	glPopMatrix();
	glPushMatrix();




	/*****LEGS/SKIRT*****/


	glTranslatef(0.0f, -3.8f, 0.0f);    //position
	glRotatef(270.0f, 1.0f, 0.0f, 0.0f);    //poles


	glRotatef(Girl2RotateAngle, 0.0f, 0.0f, 1.0f);     //full body rotation

	glColor3f(1.0f, 1.0f, 1.0f);

	glBindTexture(GL_TEXTURE_2D, skirt2_texture);

	quadric = gluNewQuadric();

	gluQuadricTexture(quadric, GL_TRUE);

	gluCylinder(quadric, 0.8f, 0.4f, 2.1f, 30, 30);

	glBindTexture(GL_TEXTURE_2D, 0);

	glColor3f(1.0f, 1.0f, 1.0f);

	glPopMatrix();
	glPopMatrix();
	glPushMatrix();
	glPopMatrix();


}



void Bird1(void)   //blue
{

	glTranslatef(Bird1Translate.X, Bird1Translate.Y, Bird1Translate.Z);      //whole bird translate
	glScalef(BirdsScale, BirdsScale, BirdsScale);    //Whole bird scale

	glPushMatrix();

	//EYES


	//left

	glRotatef(Bird1RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation

	glTranslatef(-1.15f, 0.5f, -0.22f);     //position	

	glColor3f(0.15f, 0.15f, 0.15f);

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.095f, 20, 20);

	glPopMatrix();
	glPushMatrix();



	//right

	glRotatef(Bird1RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation

	glTranslatef(-1.15f, 0.5f, 0.22f);

	glColor3f(0.15f, 0.15f, 0.15f);

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.095f, 20, 20);

	glPopMatrix();
	glPushMatrix();



	//BEAK


	glRotatef(Bird1RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation


	glTranslatef(-1.4f, 0.3f, 0.0f);     //position

	glRotatef(110.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(45.0f, 0.0f, 1.0f, 0.0f);

	glScalef(0.18f, 0.2f, 0.18f);


	glBegin(GL_TRIANGLES);

	//glColor3f(0.92f, 0.63f, 0.05f);     //yellow beak
	glColor3f(0.15f, 0.15f, 0.15f);      //black beak

	//front
	
	glNormal3f(0.0f, 0.447214f, 0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);

	//right

	glNormal3f(0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);

	//back

	glNormal3f(0.0f, 0.447214f, -0.894427f);
	
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);

	//left

	glNormal3f(-0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);

	glEnd();


	glPopMatrix();
	glPushMatrix();



	//BODY


	glRotatef(Bird1RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation

	glRotatef(-10.0, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, 0.0f, 0.0f);     //position	
	glScalef(1.2f, 0.8f, 0.8f);

	glColor3f(0.47f, 0.7f, 0.77f);
	
	quadric = gluNewQuadric();

	gluSphere(quadric, 0.7f, 30, 30);

	glPopMatrix();
	glPushMatrix();



	//FACE


	glRotatef(Bird1RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation

	glScalef(1.0f, 1.0f, 0.95f);
	glTranslatef(-1.0f, 0.45f, 0.0f);     //position	

	glColor3f(0.3f, 0.6f, 0.69f);
	
	quadric = gluNewQuadric();

	gluSphere(quadric, 0.35f, 30, 30);

	glPopMatrix();
	glPushMatrix();



	//TAIL


	glRotatef(Bird1RotateAngle, 0.0f, 1.0f, 0.0f);     //full body rotation

	glTranslatef(1.0f, -0.2f, 0.0f);       //position
	glScalef(0.6f, 0.6f, 0.6f);
	glRotatef(70.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(0.0f, 1.0f, 0.0f, 0.0f);


	glBegin(GL_TRIANGLES);
		
	glColor3f(0.3f, 0.6f, 0.69f);
	
	//front

	glNormal3f(0.0f, 0.447214f, 0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);

	//right
	
	glNormal3f(0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);

	//back
	
	glNormal3f(0.0f, 0.447214f, -0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);

	//left
	
	glNormal3f(-0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);

	glEnd();


	glBegin(GL_QUADS);

	glNormal3f(0.0f, -1.0f, 0.0f);

	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);


	glEnd();


	glPopMatrix();
	glPushMatrix();


	//WINGS

	//Right


	glRotatef(Bird1RotateAngle, 0.0f, 1.0f, 0.0f);     //full body rotation

	glTranslatef(0.0f, 0.25f, 0.25f);       //position

	glPushMatrix();

	glRotatef(Bird1WingsRotate, 1.0f, 0.0f, 0.0f);      //Wings rotation

	glTranslatef(0.0f, 0.6f, 0.0f);       //position

	glScalef(1.0f, 1.0f, 1.0f);

	glBegin(GL_TRIANGLES);

	glNormal3f(0.0f, 0.0f, 1.0f);

	glColor3f(0.38f, 0.65f, 0.73f);	

	glVertex3f(0.4f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);


	glEnd();


	glPopMatrix();
	glPopMatrix();
	glPushMatrix();



	//Left


	glRotatef(Bird1RotateAngle, 0.0f, 1.0f, 0.0f);     //full body rotation

	glTranslatef(0.0f, 0.25f, -0.25f);       //position

	glPushMatrix();

	glRotatef(-Bird1WingsRotate, 1.0f, 0.0f, 0.0f);      //Wings rotation

	glTranslatef(0.0f, 0.6f, 0.0f);       //position

	glScalef(1.0f, 1.0f, 1.0f);

	glBegin(GL_TRIANGLES);

	glNormal3f(0.0f, 0.0f, 1.0f);

	glVertex3f(0.4f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);


	glEnd();


	glPopMatrix();
	glPopMatrix();
	glPushMatrix();




	//LEGS


	//Left


	glRotatef(Bird1RotateAngle, 0.0f, 1.0f, 0.0f);     //full body rotation


	glTranslatef(-0.2f, -0.7f, -0.2f);       //position
	glScalef(0.1f, 0.4f, 0.1f);
	glRotatef(170.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(0.0f, 0.0f, 0.0f, 1.0f);


	glBegin(GL_TRIANGLES);

	glColor3f(0.36f, 0.24f, 0.13f);

	
	//front

	glNormal3f(0.0f, 0.447214f, 0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);

	//right

	glNormal3f(0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);

	//back

	glNormal3f(0.0f, 0.447214f, -0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);

	//left

	glNormal3f(-0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);

	glEnd();



	glPopMatrix();
	glPushMatrix();


	//right

	glRotatef(Bird1RotateAngle, 0.0f, 1.0f, 0.0f);     //full body rotation


	glTranslatef(-0.2f, -0.7f, 0.2f);       //position
	glScalef(0.1f, 0.4f, 0.1f);
	glRotatef(170.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(0.0f, 0.0f, 0.0f, 1.0f);


	glBegin(GL_TRIANGLES);

		
	//front

	glNormal3f(0.0f, 0.447214f, 0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);

	//right

	glNormal3f(0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);

	//back

	glNormal3f(0.0f, 0.447214f, -0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);

	//left

	glNormal3f(-0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);


	glEnd();


	glColor3f(1.0f, 1.0f, 1.0f);
	

	glPopMatrix();
	glPopMatrix();
	glPushMatrix();
	glPopMatrix();


}   //blue



void Bird2(void)   //red
{

	glTranslatef(Bird2Translate.X, Bird2Translate.Y, Bird2Translate.Z);      //whole bird translate
	glScalef(BirdsScale, BirdsScale, BirdsScale);    //Whole bird scale

	glPushMatrix();

	//EYES


	//left

	glRotatef(Bird2RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation

	glTranslatef(-1.15f, 0.5f, -0.22f);     //position	

	glColor3f(0.15f, 0.15f, 0.15f);

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.095f, 20, 20);

	glPopMatrix();
	glPushMatrix();



	//right

	glRotatef(Bird2RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation

	glTranslatef(-1.15f, 0.5f, 0.22f);

	glColor3f(0.15f, 0.15f, 0.15f);

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.095f, 20, 20);

	glPopMatrix();
	glPushMatrix();


	//BEAK


	glRotatef(Bird2RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation

	glTranslatef(-1.4f, 0.3f, 0.0f);     //position

	glRotatef(110.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(45.0f, 0.0f, 1.0f, 0.0f);

	glScalef(0.18f, 0.2f, 0.18f);


	glBegin(GL_TRIANGLES);

	//glColor3f(0.92f, 0.63f, 0.05f);     //yellow beak
	glColor3f(0.15f, 0.15f, 0.15f);      //black beak


	//front

	glNormal3f(0.0f, 0.447214f, 0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);

	//right

	glNormal3f(0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);

	//back

	glNormal3f(0.0f, 0.447214f, -0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);

	//left

	glNormal3f(-0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);

	glEnd();


	glPopMatrix();
	glPushMatrix();


	//BODY


	glRotatef(Bird2RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation

	glRotatef(-10.0, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, 0.0f, 0.0f);     //position	
	glScalef(1.2f, 0.8f, 0.8f);

	glColor3f(0.82f, 0.48f, 0.49f);

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.7f, 30, 30);

	glPopMatrix();
	glPushMatrix();



	//FACE


	glRotatef(Bird2RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation

	glScalef(1.0f, 1.0f, 0.95f);
	glTranslatef(-1.0f, 0.45f, 0.0f);     //position	


	glColor3f(0.76f, 0.3f, 0.32f);


	quadric = gluNewQuadric();

	gluSphere(quadric, 0.35f, 30, 30);

	glPopMatrix();
	glPushMatrix();



	//TAIL


	glRotatef(Bird2RotateAngle, 0.0f, 1.0f, 0.0f);     //full body rotation

	glTranslatef(1.0f, -0.2f, 0.0f);       //position
	glScalef(0.6f, 0.6f, 0.6f);
	glRotatef(70.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(0.0f, 1.0f, 0.0f, 0.0f);


	glBegin(GL_TRIANGLES);


	glColor3f(0.76f, 0.3f, 0.32f);


	//front

	glNormal3f(0.0f, 0.447214f, 0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);

	//right

	glNormal3f(0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);

	//back

	glNormal3f(0.0f, 0.447214f, -0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);

	//left

	glNormal3f(-0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);

	glEnd();


	glBegin(GL_QUADS);

	glNormal3f(0.0f, -1.0f, 0.0f);

	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);


	glEnd();


	glPopMatrix();
	glPushMatrix();


	//WINGS

	//Right


	glRotatef(Bird2RotateAngle, 0.0f, 1.0f, 0.0f);     //full body rotation

	glTranslatef(0.0f, 0.25f, 0.25f);       //position

	glPushMatrix();

	glRotatef(Bird2WingsRotate, 1.0f, 0.0f, 0.0f);      //Wings rotation

	glTranslatef(0.0f, 0.6f, 0.0f);       //position

	glScalef(1.0f, 1.0f, 1.0f);

	glBegin(GL_TRIANGLES);

	glNormal3f(0.0f, 0.0f, 1.0f);

	glColor3f(0.79f, 0.39f, 0.41f);

	glVertex3f(0.4f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);


	glEnd();


	glPopMatrix();
	glPopMatrix();
	glPushMatrix();



	//Left


	glRotatef(Bird2RotateAngle, 0.0f, 1.0f, 0.0f);     //full body rotation

	glTranslatef(0.0f, 0.25f, -0.25f);       //position

	glPushMatrix();

	glRotatef(-Bird2WingsRotate, 1.0f, 0.0f, 0.0f);      //Wings rotation

	glTranslatef(0.0f, 0.6f, 0.0f);       //position

	glScalef(1.0f, 1.0f, 1.0f);

	glBegin(GL_TRIANGLES);

	glNormal3f(0.0f, 0.0f, 1.0f);

	glVertex3f(0.4f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);


	glEnd();


	glPopMatrix();
	glPopMatrix();
	glPushMatrix();


	//LEGS


	//Left


	glRotatef(Bird2RotateAngle, 0.0f, 1.0f, 0.0f);     //full body rotation


	glTranslatef(-0.2f, -0.7f, -0.2f);       //position
	glScalef(0.1f, 0.4f, 0.1f);
	glRotatef(170.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(0.0f, 0.0f, 0.0f, 1.0f);


	glBegin(GL_TRIANGLES);

	glColor3f(0.36f, 0.24f, 0.13f);


	//front

	glNormal3f(0.0f, 0.447214f, 0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);

	//right

	glNormal3f(0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);

	//back

	glNormal3f(0.0f, 0.447214f, -0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);

	//left

	glNormal3f(-0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);


	glEnd();


	glPopMatrix();
	glPushMatrix();


	//right

	glRotatef(Bird2RotateAngle, 0.0f, 1.0f, 0.0f);     //full body rotation


	glTranslatef(-0.2f, -0.7f, 0.2f);       //position
	glScalef(0.1f, 0.4f, 0.1f);
	glRotatef(170.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(0.0f, 0.0f, 0.0f, 1.0f);

	glBegin(GL_TRIANGLES);


	//front

	glNormal3f(0.0f, 0.447214f, 0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);

	//right

	glNormal3f(0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);

	//back

	glNormal3f(0.0f, 0.447214f, -0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);

	//left

	glNormal3f(-0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);


	glEnd();


	glColor3f(1.0f, 1.0f, 1.0f);


	glPopMatrix();
	glPopMatrix();
	glPushMatrix();
	glPopMatrix();


}   //blue



void Bird3(void)   //purple
{

	glTranslatef(Bird3Translate.X, Bird3Translate.Y, Bird3Translate.Z);      //whole bird translate
	glScalef(BirdsScale, BirdsScale, BirdsScale);    //Whole bird scale

	glPushMatrix();


	//EYES


	//left

	glRotatef(Bird3RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation


	glTranslatef(-1.15f, 0.5f, -0.22f);     //position	

	glColor3f(0.15f, 0.15f, 0.15f);

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.095f, 20, 20);

	glPopMatrix();
	glPushMatrix();



	//right

	glRotatef(Bird3RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation


	glTranslatef(-1.15f, 0.5f, 0.22f);

	glColor3f(0.15f, 0.15f, 0.15f);

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.095f, 20, 20);

	glPopMatrix();
	glPushMatrix();




	//BEAK


	glRotatef(Bird3RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation


	glTranslatef(-1.4f, 0.3f, 0.0f);     //position

	glRotatef(110.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(45.0f, 0.0f, 1.0f, 0.0f);

	glScalef(0.18f, 0.2f, 0.18f);


	glBegin(GL_TRIANGLES);

	//glColor3f(0.92f, 0.63f, 0.05f);     //yellow beak
	glColor3f(0.15f, 0.15f, 0.15f);      //black beak


	//front

	glNormal3f(0.0f, 0.447214f, 0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);

	//right

	glNormal3f(0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);

	//back

	glNormal3f(0.0f, 0.447214f, -0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);

	//left

	glNormal3f(-0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);


	glEnd();


	glPopMatrix();
	glPushMatrix();


	//BODY
		

	glRotatef(Bird3RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation

	glRotatef(-10.0, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, 0.0f, 0.0f);     //position	
	glScalef(1.2f, 0.8f, 0.8f);

	glColor3f(0.62f, 0.5f, 0.69f);


	quadric = gluNewQuadric();

	gluSphere(quadric, 0.7f, 30, 30);

	glPopMatrix();
	glPushMatrix();



	//FACE


	glRotatef(Bird3RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation

	glScalef(1.0f, 1.0f, 0.95f);
	glTranslatef(-1.0f, 0.45f, 0.0f);     //position	

	glColor3f(0.5f, 0.33f, 0.58f);


	quadric = gluNewQuadric();

	gluSphere(quadric, 0.35f, 30, 30);

	glPopMatrix();
	glPushMatrix();



	//TAIL


	glRotatef(Bird3RotateAngle, 0.0f, 1.0f, 0.0f);     //full body rotation


	glTranslatef(1.0f, -0.2f, 0.0f);       //position
	glScalef(0.6f, 0.6f, 0.6f);
	glRotatef(70.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(0.0f, 1.0f, 0.0f, 0.0f);


	glBegin(GL_TRIANGLES);


	glColor3f(0.5f, 0.33f, 0.58f);


	//front

	glNormal3f(0.0f, 0.447214f, 0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);

	//right

	glNormal3f(0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);

	//back

	glNormal3f(0.0f, 0.447214f, -0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);

	//left

	glNormal3f(-0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);


	glEnd();


	glBegin(GL_QUADS);

	glNormal3f(0.0f, -1.0f, 0.0f);

	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);


	glEnd();


	glPopMatrix();
	glPushMatrix();



	//WINGS

	//Right


	glRotatef(Bird3RotateAngle, 0.0f, 1.0f, 0.0f);     //full body rotation

	glTranslatef(0.0f, 0.25f, 0.25f);       //position

	glPushMatrix();

	glRotatef(Bird3WingsRotate, 1.0f, 0.0f, 0.0f);      //Wings rotation

	glTranslatef(0.0f, 0.6f, 0.0f);       //position

	glScalef(1.0f, 1.0f, 1.0f);

	glBegin(GL_TRIANGLES);

	glNormal3f(0.0f, 0.0f, 1.0f);

	glColor3f(0.56f, 0.42f, 0.63f);

	glVertex3f(0.4f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);


	glEnd();


	glPopMatrix();
	glPopMatrix();
	glPushMatrix();



	//Left


	glRotatef(Bird3RotateAngle, 0.0f, 1.0f, 0.0f);     //full body rotation

	glTranslatef(0.0f, 0.25f, -0.25f);       //position

	glPushMatrix();

	glRotatef(-Bird3WingsRotate, 1.0f, 0.0f, 0.0f);      //Wings rotation

	glTranslatef(0.0f, 0.6f, 0.0f);       //position

	glScalef(1.0f, 1.0f, 1.0f);

	glBegin(GL_TRIANGLES);

	glNormal3f(0.0f, 0.0f, 1.0f);

	glVertex3f(0.4f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);


	glEnd();


	glPopMatrix();
	glPopMatrix();
	glPushMatrix();



	//LEGS


	//Left


	glRotatef(Bird3RotateAngle, 0.0f, 1.0f, 0.0f);     //full body rotation


	glTranslatef(-0.2f, -0.7f, -0.2f);       //position
	glScalef(0.1f, 0.4f, 0.1f);
	glRotatef(170.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(0.0f, 0.0f, 0.0f, 1.0f);


	glBegin(GL_TRIANGLES);

	glColor3f(0.36f, 0.24f, 0.13f);

	//front

	glNormal3f(0.0f, 0.447214f, 0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);

	//right

	glNormal3f(0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);

	//back

	glNormal3f(0.0f, 0.447214f, -0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);

	//left

	glNormal3f(-0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);


	glEnd();


	glPopMatrix();
	glPushMatrix();


	//right

	glRotatef(Bird3RotateAngle, 0.0f, 1.0f, 0.0f);     //full body rotation

	glTranslatef(-0.2f, -0.7f, 0.2f);       //position
	glScalef(0.1f, 0.4f, 0.1f);
	glRotatef(170.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(0.0f, 0.0f, 0.0f, 1.0f);

	glBegin(GL_TRIANGLES);


	//front

	glNormal3f(0.0f, 0.447214f, 0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);

	//right

	glNormal3f(0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);

	//back

	glNormal3f(0.0f, 0.447214f, -0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);

	//left

	glNormal3f(-0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);


	glEnd();


	glColor3f(1.0f, 1.0f, 1.0f);


	glPopMatrix();
	glPopMatrix();
	glPushMatrix();
	glPopMatrix();


}   //blue



void Bird4(void)   //orange
{

	glTranslatef(Bird4Translate.X, Bird4Translate.Y, Bird4Translate.Z);      //whole bird translate
	glScalef(BirdsScale, BirdsScale, BirdsScale);    //Whole bird scale

	glPushMatrix();


	//EYES


	//left

	glRotatef(Bird4RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation


	glTranslatef(-1.15f, 0.5f, -0.22f);     //position	

	glColor3f(0.15f, 0.15f, 0.15f);

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.095f, 20, 20);

	glPopMatrix();
	glPushMatrix();



	//right

	glRotatef(Bird4RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation


	glTranslatef(-1.15f, 0.5f, 0.22f);

	glColor3f(0.15f, 0.15f, 0.15f);

	quadric = gluNewQuadric();

	gluSphere(quadric, 0.095f, 20, 20);

	glPopMatrix();
	glPushMatrix();



	//BEAK


	glRotatef(Bird4RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation


	glTranslatef(-1.4f, 0.3f, 0.0f);     //position

	glRotatef(110.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(45.0f, 0.0f, 1.0f, 0.0f);

	glScalef(0.18f, 0.2f, 0.18f);


	glBegin(GL_TRIANGLES);

	//glColor3f(0.92f, 0.63f, 0.05f);     //yellow beak
	glColor3f(0.15f, 0.15f, 0.15f);      //black beak


	//front

	glNormal3f(0.0f, 0.447214f, 0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);

	//right

	glNormal3f(0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);

	//back

	glNormal3f(0.0f, 0.447214f, -0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);

	//left

	glNormal3f(-0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);


	glEnd();


	glPopMatrix();
	glPushMatrix();


	//BODY


	glRotatef(Bird4RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation

	glRotatef(-10.0, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, 0.0f, 0.0f);     //position	
	glScalef(1.2f, 0.8f, 0.8f);

	glColor3f(1.0f, 0.66f, 0.42f);


	quadric = gluNewQuadric();

	gluSphere(quadric, 0.7f, 30, 30);

	glPopMatrix();
	glPushMatrix();



	//FACE


	glRotatef(Bird4RotateAngle, 0.0f, 1.0f, 0.0f);      //full body rotation

	glScalef(1.0f, 1.0f, 0.95f);
	glTranslatef(-1.0f, 0.45f, 0.0f);     //position	

	glColor3f(0.94f, 0.52f, 0.21f);


	quadric = gluNewQuadric();

	gluSphere(quadric, 0.35f, 30, 30);

	glPopMatrix();
	glPushMatrix();


	//TAIL


	glRotatef(Bird4RotateAngle, 0.0f, 1.0f, 0.0f);     //full body rotation


	glTranslatef(1.0f, -0.2f, 0.0f);       //position
	glScalef(0.6f, 0.6f, 0.6f);
	glRotatef(70.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(0.0f, 1.0f, 0.0f, 0.0f);


	glBegin(GL_TRIANGLES);


	glColor3f(0.94f, 0.52f, 0.21f);;


	//front

	glNormal3f(0.0f, 0.447214f, 0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);

	//right

	glNormal3f(0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);

	//back

	glNormal3f(0.0f, 0.447214f, -0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);

	//left

	glNormal3f(-0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);


	glEnd();


	glBegin(GL_QUADS);

	glNormal3f(0.0f, -1.0f, 0.0f);

	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);


	glEnd();


	glPopMatrix();
	glPushMatrix();



	//WINGS

	//Right


	glRotatef(Bird4RotateAngle, 0.0f, 1.0f, 0.0f);     //full body rotation

	glTranslatef(0.0f, 0.25f, 0.25f);       //position

	glPushMatrix();

	glRotatef(Bird4WingsRotate, 1.0f, 0.0f, 0.0f);      //Wings rotation

	glTranslatef(0.0f, 0.6f, 0.0f);       //position

	glScalef(1.0f, 1.0f, 1.0f);

	glBegin(GL_TRIANGLES);

	glNormal3f(0.0f, 0.0f, 1.0f);

	glColor3f(0.95f, 0.57f, 0.3f);

	glVertex3f(0.4f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);


	glEnd();


	glPopMatrix();
	glPopMatrix();
	glPushMatrix();



	//Left


	glRotatef(Bird4RotateAngle, 0.0f, 1.0f, 0.0f);     //full body rotation

	glTranslatef(0.0f, 0.25f, -0.25f);       //position

	glPushMatrix();

	glRotatef(-Bird4WingsRotate, 1.0f, 0.0f, 0.0f);      //Wings rotation

	glTranslatef(0.0f, 0.6f, 0.0f);       //position

	glScalef(1.0f, 1.0f, 1.0f);

	glBegin(GL_TRIANGLES);

	glNormal3f(0.0f, 0.0f, 1.0f);

	glVertex3f(0.4f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);


	glEnd();


	glPopMatrix();
	glPopMatrix();
	glPushMatrix();


	//LEGS


	//Left


	glRotatef(Bird4RotateAngle, 0.0f, 1.0f, 0.0f);     //full body rotation

	glTranslatef(-0.2f, -0.7f, -0.2f);       //position
	glScalef(0.1f, 0.4f, 0.1f);
	glRotatef(170.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(0.0f, 0.0f, 0.0f, 1.0f);


	glBegin(GL_TRIANGLES);

	glColor3f(0.36f, 0.24f, 0.13f);


	//front

	glNormal3f(0.0f, 0.447214f, 0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);

	//right

	glNormal3f(0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);

	//back

	glNormal3f(0.0f, 0.447214f, -0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);

	//left

	glNormal3f(-0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);


	glEnd();


	glPopMatrix();
	glPushMatrix();


	//right

	glRotatef(Bird4RotateAngle, 0.0f, 1.0f, 0.0f);     //full body rotation

	glTranslatef(-0.2f, -0.7f, 0.2f);       //position
	glScalef(0.1f, 0.4f, 0.1f);
	glRotatef(170.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(0.0f, 0.0f, 0.0f, 1.0f);


	glBegin(GL_TRIANGLES);


	//front

	glNormal3f(0.0f, 0.447214f, 0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, 0.5f);

	//right

	glNormal3f(0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glVertex3f(0.5f, -0.5f, -0.5f);

	//back

	glNormal3f(0.0f, 0.447214f, -0.894427f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, -0.5f);

	//left

	glNormal3f(-0.894427f, 0.447214f, 0.0f);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glVertex3f(-0.5f, -0.5f, 0.5f);


	glEnd();


	glColor3f(1.0f, 1.0f, 1.0f);


	glPopMatrix();
	glPopMatrix();
	glPushMatrix();
	glPopMatrix();


}   //blue



void Rice(void)
{

	glPointSize(4.0f);

	glBegin(GL_POINTS);

	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-0.4f, 0.0f, -0.3f);
	glVertex3f(0.0f, 0.0f, -0.6f);
	glVertex3f(-0.3f, 0.0f, 0.9f);
	glVertex3f(-0.6f, 0.0f, 1.2f);
	glVertex3f(0.0f, 0.0f, 0.5f);
	glVertex3f(0.8f, 0.0f, 0.8f);
	glVertex3f(-0.6f, 0.0f, -0.3f);
	glVertex3f(0.4f, 0.0f, -0.6f);
	glVertex3f(-0.3f, 0.0f, -0.9f);
	glVertex3f(-0.6f, 0.0f, -1.2f);
	glVertex3f(1.0f, 0.0f, -0.6f);
	glVertex3f(-1.2f, 0.0f, -0.3f);
	glVertex3f(1.2f, 0.0f, 0.9f);
	glVertex3f(1.0f, 0.0f, 1.5f);
	glVertex3f(1.6f, 0.0f, -0.2f);
	glVertex3f(4.0f, -1.75f, 1.0f);


	glEnd();

}



void FadeInQuad(void)
{

	glBegin(GL_QUADS);

	glVertex3f(2.0f, 2.0f, 13.0f);
	glVertex3f(-2.0f, 2.0f, 13.0f);
	glVertex3f(-2.0f, -2.0f, 13.0f);
	glVertex3f(2.0f, -2.0f, 13.0f);

	glEnd();

}









void Scene1(void)
{

	//Bird2 (red)

	if (SunTranslate.Y >= 0.0f)
	{
		isBird2Still = false;
		isScene1 = true;
	}


	if (isScene1 == true)
	{


		switch (B2WingsRot)
		{
		case 0:
			Bird2WingsRotate -= BirdsWingsSpeed;
			if (Bird2WingsRotate <= WingSwingMin)
			{
				Bird2WingsRotate = WingSwingMin;
				B2WingsRot++;
			}
			break;

		case 1:
			Bird2WingsRotate += BirdsWingsSpeed;
			if (Bird2WingsRotate >= WingSwingMax)
			{
				Bird2WingsRotate = WingSwingMax;
				B2WingsRot--;
			}
			break;
		}


		Bird2Translate.X -= BirdsTranslateSpeed - 0.005f;
		if (Bird2Translate.X <= 6.5f)
		{
			Bird2Translate.X = 6.5f;
		}

		Bird2Translate.Y -= BirdsTranslateSpeed - 0.005f;
		if (Bird2Translate.Y <= -0.5f)
		{
			Bird2Translate.Y = -0.5f;
		}

		Bird2Translate.Z -= BirdsTranslateSpeed - 0.005f;
		if (Bird2Translate.Z <= 1.0f)
		{
			Bird2Translate.Z = 1.0f;
			isBird2Still = true;
		}

	}

	if (isBird2Still == true)
	{
		Bird2WingsRotate = 165.0f;
	}


	//Bird3 (purple)


	if (SunTranslate.Y >= 0.0f)
	{
		isBird3Still = false;
	}


	if (isBird3Still == false)
	{

		static int B3WingsRot = 0;
		switch (B3WingsRot)
		{
		case 0:
			Bird3WingsRotate -= BirdsWingsSpeed;
			if (Bird3WingsRotate <= WingSwingMin)
			{
				Bird3WingsRotate = WingSwingMin;
				B3WingsRot++;
			}
			break;

		case 1:
			Bird3WingsRotate += BirdsWingsSpeed;
			if (Bird3WingsRotate >= WingSwingMax)
			{
				Bird3WingsRotate = WingSwingMax;
				B3WingsRot--;
			}
			break;
		}

		Bird3Translate.X += BirdsTranslateSpeed;
		if (Bird3Translate.X >= -3.6f)
		{
			Bird3Translate.X = -3.6f;
		}

		Bird3Translate.Y -= BirdsTranslateSpeed - 0.013175f;
		if (Bird3Translate.Y <= 2.8f)
		{
			Bird3Translate.Y = 2.8f;
			isBird3Still = true;
			isGirl1Visible = true;
			isGirl1Still = false;
			isScene2 = true;
			//Scene++;
		}

		Bird3Translate.Z -= BirdsTranslateSpeed;
		if (Bird3Translate.Z <= -0.9f)
		{
			Bird3Translate.Z = -0.9f;
		}

	}

	if (isBird3Still == true)
	{
		Bird3WingsRotate = 165.0f;
	}

}




void Scene2(void)
{
	if (isScene2 == true)
	{




		switch (Girl1Movements)
		{
		case 0:



			switch (RightHandLoop)
			{

			case 0:
				Girl1RtShoulderRotate -= GirlsHandsRotateSpeed;
				if (Girl1RtShoulderRotate <= HandSwingMin)
				{
					Girl1RtShoulderRotate = HandSwingMin;
					RightHandLoop++;
				}
				break;

			case 1:
				Girl1RtShoulderRotate += GirlsHandsRotateSpeed;
				if (Girl1RtShoulderRotate >= HandSwingMax)
				{
					Girl1RtShoulderRotate = HandSwingMax;
					RightHandLoop--;
				}
				break;
			}




			switch (LeftHandLoop)
			{

			case 0:
				Girl1LtShoulderRotate += GirlsHandsRotateSpeed;
				if (Girl1LtShoulderRotate >= HandSwingMax)
				{
					Girl1LtShoulderRotate = HandSwingMax;
					LeftHandLoop++;
				}
				break;

			case 1:
				Girl1LtShoulderRotate -= GirlsHandsRotateSpeed;
				if (Girl1LtShoulderRotate <= HandSwingMin)
				{
					Girl1LtShoulderRotate = HandSwingMin;
					LeftHandLoop--;
				}
				break;
			}




			Girl1Translate.X += GirlsTranslateSpeed;
			if (Girl1Translate.X >= -1.0f)
			{
				Girl1Translate.X = -1.0f;
				//isGirl1Still = true;
				Girl1Movements++;
			}

			Girl1Translate.Z -= GirlsTranslateSpeed - 0.0036f;
			if (Girl1Translate.Z <= 0.8f)
			{
				Girl1Translate.Z = 0.8f;

			}

			break;



		case 1:

			Girl1RtShoulderRotate = 270.0f;
			Girl1LtShoulderRotate = 270.0f;
			Girl1Movements++;

			break;



		case 2:

			if (Girl1RtShoulderRotate < 340.0f)
			{
				Girl1RtShoulderRotate += 0.8f; // GirlsHandsRotateSpeed;
			}
			else
			{
				Girl1RtShoulderRotate = 340.0f;
				Girl1Movements++;
			}
			break;


		case 3:
			RiceAlpha += RiceAlphaSpeed;
			if (RiceAlpha >= 1.0f)
			{
				RiceAlpha = 1.0f;
				isRiceFeed = false;
				isBird1Still = false;
				isBird2Still = false;
				isBird3Still = false;
				isBird4Still = false;
				Girl1Movements++;
			}
			break;

		case 4:

			if (Girl1RtShoulderRotate > 270.0f)
			{
				Girl1RtShoulderRotate -= 1.0f; // GirlsHandsRotateSpeed;
			}
			else
			{
				Girl1RtShoulderRotate = 270.0f;
				isGirl2Visible = true;
				Girl1Movements++;
				//Scene++;
			}
			break;


		case 5:
			//Bird1 blue

			if (isBird1Still == false)
			{


				switch (B2WingsRot)
				{
				case 0:
					Bird1WingsRotate -= BirdsWingsSpeed;
					if (Bird1WingsRotate <= WingSwingMin)
					{
						Bird1WingsRotate = WingSwingMin;
						B2WingsRot++;
					}
					break;

				case 1:
					Bird1WingsRotate += BirdsWingsSpeed;
					if (Bird1WingsRotate >= WingSwingMax)
					{
						Bird1WingsRotate = WingSwingMax;
						B2WingsRot--;
					}
					break;
				}

				Bird1RotateAngle += BirdsRotateSpeed + 0.3f;
				if (Bird1RotateAngle >= 90.0f)
				{
					Bird1RotateAngle = 90.0f;
				}



				Bird1Translate.X -= BirdsTranslateSpeed + 0.015f;
				if (Bird1Translate.X <= 2.0f)
				{
					Bird1Translate.X = 2.0f;
					isBird1Still = true;
				}

				Bird1Translate.Y -= BirdsTranslateSpeed;
				if (Bird1Translate.Y <= -1.75f)
				{
					Bird1Translate.Y = -1.75f;
					//isBird1Still = true;
				}

				Bird1Translate.Z += BirdsTranslateSpeed + 0.01f;
				if (Bird1Translate.Z >= 0.0f)
				{
					Bird1Translate.Z = 0.0f;
					//isBird1Still = true;
				}

			}

			if (isBird1Still == true)
			{
				Bird1WingsRotate = 165.0f;
			}



			//Bird2 red



			if (isBird2Still == false)
			{


				switch (B2WingsRot)
				{
				case 0:
					Bird2WingsRotate -= BirdsWingsSpeed;
					if (Bird2WingsRotate <= WingSwingMin)
					{
						Bird2WingsRotate = WingSwingMin;
						B2WingsRot++;
					}
					break;

				case 1:
					Bird2WingsRotate += BirdsWingsSpeed;
					if (Bird2WingsRotate >= WingSwingMax)
					{
						Bird2WingsRotate = WingSwingMax;
						B2WingsRot--;
					}
					break;
				}



				Bird2RotateAngle += BirdsRotateSpeed;
				if (Bird2RotateAngle >= 90.0f)
				{
					Bird2RotateAngle = 90.0f;
				}


				Bird2Translate.X -= BirdsTranslateSpeed + 0.03f;
				if (Bird2Translate.X <= 3.0f)
				{
					Bird2Translate.X = 3.0f;
					//isBird2Still = true;
				}

				Bird2Translate.Y -= BirdsTranslateSpeed;
				if (Bird2Translate.Y <= -1.75f)
				{
					Bird2Translate.Y = -1.75f;
					isBird2Still = true;
				}

				Bird2Translate.Z += BirdsTranslateSpeed + 0.035f;
				if (Bird2Translate.Z >= 0.3f)
				{
					Bird2Translate.Z = 0.3f;

				}

			}

			if (isBird2Still == true)
			{
				Bird2WingsRotate = 165.0f;
			}




			//Bird3 purple


			if (isBird3Still == false)
			{


				switch (B2WingsRot)
				{
				case 0:
					Bird3WingsRotate -= BirdsWingsSpeed;
					if (Bird3WingsRotate <= WingSwingMin)
					{
						Bird3WingsRotate = WingSwingMin;
						B2WingsRot++;
					}
					break;

				case 1:
					Bird3WingsRotate += BirdsWingsSpeed;
					if (Bird3WingsRotate >= WingSwingMax)
					{
						Bird3WingsRotate = WingSwingMax;
						B2WingsRot--;
					}
					break;
				}


				Bird3RotateAngle -= BirdsRotateSpeed;
				if (Bird3RotateAngle <= 110.0f)
				{
					Bird3RotateAngle = 110.0f;
				}


				Bird3Translate.X += BirdsTranslateSpeed;
				if (Bird3Translate.X >= 1.2f)
				{
					Bird3Translate.X = 1.2f;
					isBird3Still = true;
					//Scene++;
				}

				Bird3Translate.Y -= BirdsTranslateSpeed - 0.00125f;
				if (Bird3Translate.Y <= -1.75f)
				{
					Bird3Translate.Y = -1.75f;
					//isBird3Still = true;
					//Scene++;
				}

				Bird3Translate.Z += BirdsTranslateSpeed;
				if (Bird3Translate.Z >= 0.2f)
				{
					Bird3Translate.Z = 0.2f;
					//isBird3Still = true;
				}

			}

			if (isBird3Still == true)
			{
				Bird3WingsRotate = 165.0f;
				isScene3 = true;
			}



			break;


		}
	}

}




void Scene3(void)
{

	if (isScene3 == true)
	{
		switch (Girl2Movements)
		{
		case 0:


			//Bird4 orange



			if (isBird4Still == false)
			{


				switch (B2WingsRot)
				{
				case 0:
					Bird4WingsRotate -= BirdsWingsSpeed;
					if (Bird4WingsRotate <= WingSwingMin)
					{
						Bird4WingsRotate = WingSwingMin;
						B2WingsRot++;
					}
					break;

				case 1:
					Bird4WingsRotate += BirdsWingsSpeed;
					if (Bird4WingsRotate >= WingSwingMax)
					{
						Bird4WingsRotate = WingSwingMax;
						B2WingsRot--;
					}
					break;
				}



				Bird4RotateAngle -= BirdsRotateSpeed;
				if (Bird4RotateAngle <= 0.0f)
				{
					Bird4RotateAngle = 0.0f;
				}


				Bird4Translate.X -= BirdsTranslateSpeed + 0.00375f;
				if (Bird4Translate.X <= 4.0f)
				{
					Bird4Translate.X = 4.0f;
					//isBird4Still = true;
				}

				Bird4Translate.Y -= BirdsTranslateSpeed;
				if (Bird4Translate.Y <= -1.75f)
				{
					Bird4Translate.Y = -1.75f;
					isBird4Still = true;
				}

				Bird4Translate.Z -= BirdsTranslateSpeed + 0.00175f;
				if (Bird4Translate.Z <= 1.0f)
				{
					Bird4Translate.Z = 1.0f;

				}

			}

			if (isBird4Still == true)
			{
				Bird4WingsRotate = 165.0f;
			}




			//Girl1


			Girl1RotateAngle -= GirlsRotateSpeed;
			if (Girl1RotateAngle <= 10.0f)
			{
				Girl1RotateAngle = 10.0f;
				//Girl2Movements++;
			}




			Girl2Translate.X -= GirlsTranslateSpeed;
			if (Girl2Translate.X <= -2.0f)
			{
				Girl2Translate.X = -2.0f;
				//isGirl1Still = true;
				Girl2Movements++;
			}



			Girl2Translate.Z -= GirlsTranslateSpeed + 0.0073;
			if (Girl2Translate.Z <= 4.0f)
			{
				Girl2Translate.Z = 4.0f;

			}



			switch (RightHandLoop)
			{

			case 0:
				Girl2RtShoulderRotate -= GirlsHandsRotateSpeed;
				if (Girl2RtShoulderRotate <= HandSwingMin)
				{
					Girl2RtShoulderRotate = HandSwingMin;
					RightHandLoop++;
				}
				break;

			case 1:
				Girl2RtShoulderRotate += GirlsHandsRotateSpeed;
				if (Girl2RtShoulderRotate >= HandSwingMax)
				{
					Girl2RtShoulderRotate = HandSwingMax;
					RightHandLoop--;
				}
				break;
			}




			switch (LeftHandLoop)
			{

			case 0:
				Girl2LtShoulderRotate += GirlsHandsRotateSpeed;
				if (Girl2LtShoulderRotate >= HandSwingMax)
				{
					Girl2LtShoulderRotate = HandSwingMax;
					LeftHandLoop++;
				}
				break;

			case 1:
				Girl2LtShoulderRotate -= GirlsHandsRotateSpeed;
				if (Girl2LtShoulderRotate <= HandSwingMin)
				{
					Girl2LtShoulderRotate = HandSwingMin;
					LeftHandLoop--;
				}
				break;
			}



			if (Girl2Translate.X <= 1.0f)
			{
				Girl1RotateAngle -= GirlsRotateSpeed;
				if (Girl1RotateAngle <= 290.0f)
				{
					Girl1RotateAngle = 290.0f;
					//Girl2Movements++;
				}

				Girl1Translate.X -= GirlsTranslateSpeed;
				if (Girl1Translate.X <= -4.0f)
				{
					Girl1Translate.X = -4.0f;
					//isGirl1Still = true;
					isGirl1Visible = false;
				}

				Girl1Translate.Z += GirlsTranslateSpeed - 0.002f;
				if (Girl1Translate.Z >= 2.0f)
				{
					Girl1Translate.Z = 2.0f;
				}




				switch (RightHandLoop)
				{

				case 0:
					Girl1RtShoulderRotate -= GirlsHandsRotateSpeed;
					if (Girl1RtShoulderRotate <= HandSwingMin)
					{
						Girl1RtShoulderRotate = HandSwingMin;
						RightHandLoop++;
					}
					break;

				case 1:
					Girl1RtShoulderRotate += GirlsHandsRotateSpeed;
					if (Girl1RtShoulderRotate >= HandSwingMax)
					{
						Girl1RtShoulderRotate = HandSwingMax;
						RightHandLoop--;
					}
					break;
				}




				switch (LeftHandLoop)
				{

				case 0:
					Girl1LtShoulderRotate += GirlsHandsRotateSpeed;
					if (Girl1LtShoulderRotate >= HandSwingMax)
					{
						Girl1LtShoulderRotate = HandSwingMax;
						LeftHandLoop++;
					}
					break;

				case 1:
					Girl1LtShoulderRotate -= GirlsHandsRotateSpeed;
					if (Girl1LtShoulderRotate <= HandSwingMin)
					{
						Girl1LtShoulderRotate = HandSwingMin;
						LeftHandLoop--;
					}
					break;
				}


			}



			break;



		case 1:

			Girl2Translate.Y += GirlsTranslateSpeed; //- 0.001f;
			if (Girl2Translate.Y >= -0.05f)
			{
				Girl2Translate.Y = -0.05f;
				Girl2Movements++;
			}
			break;


		case 2:


			Girl2Translate.X -= GirlsTranslateSpeed;
			if (Girl2Translate.X <= -4.0f)
			{
				Girl2Translate.X = -4.0f;
				Girl2Movements++;
				isGirl2Still = true;
			}


			Girl2Translate.Z -= GirlsTranslateSpeed;
			if (Girl2Translate.Z <= 2.0f)
			{
				Girl2Translate.Z = 2.0f;

			}



			switch (RightHandLoop)
			{

			case 0:
				Girl2RtShoulderRotate -= GirlsHandsRotateSpeed;
				if (Girl2RtShoulderRotate <= HandSwingMin)
				{
					Girl2RtShoulderRotate = HandSwingMin;
					RightHandLoop++;
				}
				break;

			case 1:
				Girl2RtShoulderRotate += GirlsHandsRotateSpeed;
				if (Girl2RtShoulderRotate >= HandSwingMax)
				{
					Girl2RtShoulderRotate = HandSwingMax;
					RightHandLoop--;
				}
				break;
			}




			switch (LeftHandLoop)
			{

			case 0:
				Girl2LtShoulderRotate += GirlsHandsRotateSpeed;
				if (Girl2LtShoulderRotate >= HandSwingMax)
				{
					Girl2LtShoulderRotate = HandSwingMax;
					LeftHandLoop++;
				}
				break;

			case 1:
				Girl2LtShoulderRotate -= GirlsHandsRotateSpeed;
				if (Girl2LtShoulderRotate <= HandSwingMin)
				{
					Girl2LtShoulderRotate = HandSwingMin;
					LeftHandLoop--;
				}
				break;
			}

			break;


		case 3:
			if (isGirl2Still == true)
			{
				isGirl2Visible = false;
				Girl2RotateAngle = 30.0f;
				isBird1Still = false;
				isBird2Still = false;
				isBird3Still = false;
				isBird4Still = false;
				isScene4 = true;

				//Scene++;
			}

			break;

		}

	}

	
}




void Scene4(void)
{
	if (isScene4 == true)
	{
		//Bird3 purple


		if (isBird3Still == false)
		{


			switch (B2WingsRot)
			{
			case 0:
				Bird3WingsRotate -= BirdsWingsSpeed;
				if (Bird3WingsRotate <= WingSwingMin)
				{
					Bird3WingsRotate = WingSwingMin;
					B2WingsRot++;
				}
				break;

			case 1:
				Bird3WingsRotate += BirdsWingsSpeed;
				if (Bird3WingsRotate >= WingSwingMax)
				{
					Bird3WingsRotate = WingSwingMax;
					B2WingsRot--;
				}
				break;
			}


			Bird3RotateAngle += BirdsRotateSpeed + 4.0f;
			if (Bird3RotateAngle >= 360.0f)
			{
				Bird3RotateAngle = 360.0f;
			}


			Bird3Translate.X -= BirdsTranslateSpeed;
			if (Bird3Translate.X <= -13.0f)
			{
				Bird3Translate.X = -13.0f;
				isBird3Still = true;
				//BirdBlockings++;

			}

			Bird3Translate.Y += BirdsTranslateSpeed;
			if (Bird3Translate.Y >= 7.0f)
			{
				Bird3Translate.Y = 7.0f;
				//isBird3Still = true;
				//Scene++;
			}

			Bird3Translate.Z -= BirdsTranslateSpeed;
			if (Bird3Translate.Z <= 0.0f)
			{
				Bird3Translate.Z = 0.0f;
				//isBird3Still = true;
			}

		}

		if (isBird3Still == true)
		{
			Bird3WingsRotate = 165.0f;
		}





		//Bird4 orange



		if (isBird4Still == false)
		{


			switch (B2WingsRot)
			{
			case 0:
				Bird4WingsRotate -= BirdsWingsSpeed;
				if (Bird4WingsRotate <= WingSwingMin)
				{
					Bird4WingsRotate = WingSwingMin;
					B2WingsRot++;
				}
				break;

			case 1:
				Bird4WingsRotate += BirdsWingsSpeed;
				if (Bird4WingsRotate >= WingSwingMax)
				{
					Bird4WingsRotate = WingSwingMax;
					B2WingsRot--;
				}
				break;
			}



			Bird4RotateAngle -= BirdsRotateSpeed;
			if (Bird4RotateAngle <= 0.0f)
			{
				Bird4RotateAngle = 0.0f;
			}


			Bird4Translate.X -= BirdsTranslateSpeed + 0.0015f;
			if (Bird4Translate.X <= -13.0f)
			{
				Bird4Translate.X = -13.0f;
				//isBird4Still = true;

			}

			Bird4Translate.Y += BirdsTranslateSpeed;
			if (Bird4Translate.Y >= 7.0f)
			{
				Bird4Translate.Y = 7.0f;
				isBird4Still = true;
				isBird1Still = false;
				isGirl2Still = false;
				isGirl2Visible = true;
				isScene5 = true;
				//Scene++;

			}

			Bird4Translate.Z -= BirdsTranslateSpeed + 0.0015f;
			if (Bird4Translate.Z <= 0.0f)
			{
				Bird4Translate.Z = 0.0f;


			}

		}

		if (isBird4Still == true)
		{
			Bird4WingsRotate = 165.0f;


			Bird1RotateAngle += BirdsRotateSpeed + 0.8f;
			if (Bird1RotateAngle >= 300.0f)
			{
				Bird1RotateAngle = 300.0f;
			}
		}



	}




}




void Scene5(void)
{


	if (isScene5 == true)
	{
		//Bird1 blue

		if (isBird1Still == false)
		{


			switch (B2WingsRot)
			{
			case 0:
				Bird1WingsRotate -= BirdsWingsSpeed;
				if (Bird1WingsRotate <= WingSwingMin)
				{
					Bird1WingsRotate = WingSwingMin;
					B2WingsRot++;
				}
				break;

			case 1:
				Bird1WingsRotate += BirdsWingsSpeed;
				if (Bird1WingsRotate >= WingSwingMax)
				{
					Bird1WingsRotate = WingSwingMax;
					B2WingsRot--;
				}
				break;
			}





			Bird1Translate.X += BirdsTranslateSpeed + 0.006f;
			if (Bird1Translate.X >= 5.3f)
			{
				Bird1Translate.X = 5.3f;

			}

			Bird1Translate.Y += BirdsTranslateSpeed;
			if (Bird1Translate.Y >= 0.1f)
			{
				Bird1Translate.Y = 0.1f;
				isBird1Still = true;
			}

			Bird1Translate.Z -= BirdsTranslateSpeed + 0.004f;
			if (Bird1Translate.Z <= -3.1f)
			{
				Bird1Translate.Z = -3.1f;
				//isBird1Still = true;
			}

		}

		if (isBird1Still == true)
		{
			Bird1WingsRotate = 165.0f;

			Bird1RotateAngle -= BirdsRotateSpeed + 0.3f;
			if (Bird1RotateAngle <= 40.0f)
			{
				Bird1RotateAngle = 40.0f;
			}
		}




		//Bird2 red



		if (isBird2Still == false)
		{


			switch (B2WingsRot)
			{
			case 0:
				Bird2WingsRotate -= BirdsWingsSpeed;
				if (Bird2WingsRotate <= WingSwingMin)
				{
					Bird2WingsRotate = WingSwingMin;
					B2WingsRot++;
				}
				break;

			case 1:
				Bird2WingsRotate += BirdsWingsSpeed;
				if (Bird2WingsRotate >= WingSwingMax)
				{
					Bird2WingsRotate = WingSwingMax;
					B2WingsRot--;
				}
				break;
			}



			Bird2RotateAngle -= BirdsRotateSpeed;
			if (Bird2RotateAngle <= 210.0f)
			{
				Bird2RotateAngle = 210.0f;
			}


			Bird2Translate.X += BirdsTranslateSpeed;
			if (Bird2Translate.X >= 9.0f)
			{
				Bird2Translate.X = 9.0f;
				isBird2Still = true;

			}

			Bird2Translate.Y += BirdsTranslateSpeed;
			if (Bird2Translate.Y >= 5.5f)
			{
				Bird2Translate.Y = 5.5f;

			}

			Bird2Translate.Z += BirdsTranslateSpeed;
			if (Bird2Translate.Z >= 4.0f)
			{
				Bird2Translate.Z = 4.0f;

			}

		}

		if (isBird2Still == true)
		{
			Bird2WingsRotate = 165.0f;
			//BirdBlockings++;

			//Scene++;
		}

		RiceAlpha -= RiceAlphaSpeed;
		if (RiceAlpha >= 0.0f)
		{
			RiceAlpha = 0.0f;
			isRiceFeed = false;

		}






		switch (Girl2Movements)
		{
		case 3:

			Girl2Translate.X += GirlsTranslateSpeed + 0.002f;
			if (Girl2Translate.X >= -2.0f)
			{
				Girl2Translate.X = -2.0f;
				Girl2Movements++;
				//isGirl2Still = true;
			}


			Girl2Translate.Z += GirlsTranslateSpeed + 0.001f;
			if (Girl2Translate.Z >= 4.0f)
			{
				Girl2Translate.Z = 4.0f;

			}



			switch (RightHandLoop)
			{

			case 0:
				Girl2RtShoulderRotate -= GirlsHandsRotateSpeed;
				if (Girl2RtShoulderRotate <= HandSwingMin)
				{
					Girl2RtShoulderRotate = HandSwingMin;
					RightHandLoop++;
				}
				break;

			case 1:
				Girl2RtShoulderRotate += GirlsHandsRotateSpeed;
				if (Girl2RtShoulderRotate >= HandSwingMax)
				{
					Girl2RtShoulderRotate = HandSwingMax;
					RightHandLoop--;
				}
				break;
			}




			switch (LeftHandLoop)
			{

			case 0:
				Girl2LtShoulderRotate += GirlsHandsRotateSpeed;
				if (Girl2LtShoulderRotate >= HandSwingMax)
				{
					Girl2LtShoulderRotate = HandSwingMax;
					LeftHandLoop++;
				}
				break;

			case 1:
				Girl2LtShoulderRotate -= GirlsHandsRotateSpeed;
				if (Girl2LtShoulderRotate <= HandSwingMin)
				{
					Girl2LtShoulderRotate = HandSwingMin;
					LeftHandLoop--;
				}
				break;
			}


			break;





		case 4:

			Girl2Translate.Y -= GirlsTranslateSpeed; //- 0.001f;
			if (Girl2Translate.Y <= -0.3f)
			{
				Girl2Translate.Y = -0.3f;
				Girl2Movements++;
			}


			break;





		case 5:

			Girl2Translate.X += GirlsTranslateSpeed;
			if (Girl2Translate.X >= 2.0f)
			{
				Girl2Translate.X = 2.0f;
				//isGirl1Still = true;
				//isGirl1Visible = false;
			}

			Girl2Translate.Z += GirlsTranslateSpeed + 0.006;
			if (Girl2Translate.Z >= 15.0f)
			{
				Girl2Translate.Z = 15.0f;
			}


			switch (RightHandLoop)
			{

			case 0:
				Girl2RtShoulderRotate -= GirlsHandsRotateSpeed;
				if (Girl2RtShoulderRotate <= HandSwingMin)
				{
					Girl2RtShoulderRotate = HandSwingMin;
					RightHandLoop++;
				}
				break;

			case 1:
				Girl2RtShoulderRotate += GirlsHandsRotateSpeed;
				if (Girl2RtShoulderRotate >= HandSwingMax)
				{
					Girl2RtShoulderRotate = HandSwingMax;
					RightHandLoop--;
				}
				break;
			}




			switch (LeftHandLoop)
			{

			case 0:
				Girl2LtShoulderRotate += GirlsHandsRotateSpeed;
				if (Girl2LtShoulderRotate >= HandSwingMax)
				{
					Girl2LtShoulderRotate = HandSwingMax;
					LeftHandLoop++;
				}
				break;

			case 1:
				Girl2LtShoulderRotate -= GirlsHandsRotateSpeed;
				if (Girl2LtShoulderRotate <= HandSwingMin)
				{
					Girl2LtShoulderRotate = HandSwingMin;
					LeftHandLoop--;
				}
				break;
			}


		}
	}
}

