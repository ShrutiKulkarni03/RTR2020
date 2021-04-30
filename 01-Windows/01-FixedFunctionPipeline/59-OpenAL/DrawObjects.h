#pragma once

#define PI 3.14

//Global Update() variables

GLfloat BG_blue = 0.0f;

//IANI Translate

GLfloat I1_x = -1.85f;
GLfloat A_x = 2.0f;
GLfloat N_y = 1.5f;
GLfloat I2_y = -1.5f;

//D color

GLfloat D_SaffronR = 0.0f;
GLfloat D_SaffronG = 0.0f;
GLfloat D_SaffronB = 0.0f;

GLfloat D_GreenR = 0.0f;
GLfloat D_GreenG = 0.0f;
GLfloat D_GreenB = 0.0f;

GLfloat India_scale_x = 0.8f;
GLfloat India_scale_y = 0.8f;

//A Flag

GLfloat Flag_A_Saffron_R = 0.0f;
GLfloat Flag_A_Saffron_G = 0.0f;
GLfloat Flag_A_Saffron_B = 0.0f;

GLfloat Flag_A_White_R = 0.0f;
GLfloat Flag_A_White_G = 0.0f;
GLfloat Flag_A_White_B = 0.0f;

GLfloat Flag_A_Green_R = 0.0f;
GLfloat Flag_A_Green_G = 0.0f;
GLfloat Flag_A_Green_B = 0.0f;

//Smoke Tricolor

GLfloat Smoke_light_Saffron_R = 1.0f;
GLfloat Smoke_light_Saffron_G = 1.0f;
GLfloat Smoke_light_Saffron_B = 1.0f;

GLfloat Smoke_light_White_R = 1.0f;
GLfloat Smoke_light_White_G = 1.0f;
GLfloat Smoke_light_White_B = 1.0f;

GLfloat Smoke_light_Green_R = 1.0f;
GLfloat Smoke_light_Green_G = 1.0f;
GLfloat Smoke_light_Green_B = 1.0f;


GLfloat Smoke_dark_Saffron_R = 0.0f;
GLfloat Smoke_dark_Saffron_G = 0.0f;
GLfloat Smoke_dark_Saffron_B = 0.082f;

GLfloat Smoke_dark_White_R = 0.0f;
GLfloat Smoke_dark_White_G = 0.0f;
GLfloat Smoke_dark_White_B = 0.082f;

GLfloat Smoke_dark_Green_R = 0.0f;
GLfloat Smoke_dark_Green_G = 0.0f;
GLfloat Smoke_dark_Green_B = 0.082f;

//Jets straight translate

GLfloat Jet1_x = 0.0f;
GLfloat Jet2_x = -2.5f;
GLfloat Jet3_x = 0.0f;

GLfloat Jet1_curve_x = 0.0f;
GLfloat Jet1_curve_y = 0.0f;
GLfloat Jet3_curve_x = 0.0f;
GLfloat Jet3_curve_y = 0.0f;

//Jets curve translate

GLfloat Jet1_angle1 = 0.94 * PI;
GLfloat Jet3_angle1 = 1.08 * PI;

GLfloat r1_x = -1.0f;
GLfloat r1_y = 1.25f;
GLfloat r3_x = -1.0f;
GLfloat r3_y = -1.25;

//Jets rotate

GLfloat Jet1_rot = 270.0f;
GLfloat Jet3_rot = 90.0f;

//India

void I1(void)
{
	//I

	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.6f, 0.2f);

	glVertex3f(-0.75f, 0.25f, 0.0f);
	glVertex3f(-0.8f, 0.25f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);

	glVertex3f(-0.8f, -0.25f, 0.0f);
	glVertex3f(-0.75f, -0.25f, 0.0f);

	glEnd();
}

void N(void)
{
	//N

	//left

	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.6f, 0.2f);

	glVertex3f(-0.55f, 0.25f, 0.0f);
	glVertex3f(-0.6f, 0.25f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);

	glVertex3f(-0.6f, -0.25f, 0.0f);
	glVertex3f(-0.55f, -0.25f, 0.0f);

	//slant

	glColor3f(1.0f, 0.6f, 0.2f);

	glVertex3f(-0.53f, 0.25f, 0.0f);
	glVertex3f(-0.53f, 0.13f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);

	glVertex3f(-0.37f, -0.25f, 0.0f);
	glVertex3f(-0.37f, -0.13f, 0.0f);

	//right

	glColor3f(1.0f, 0.6f, 0.2f);

	glVertex3f(-0.3f, 0.25f, 0.0f);
	glVertex3f(-0.35f, 0.25f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);

	glVertex3f(-0.35f, -0.25f, 0.0f);
	glVertex3f(-0.3f, -0.25f, 0.0f);

	glEnd();

}

void D(void)
{

	//D

	//left

	glBegin(GL_QUADS);

	glColor3f(D_SaffronR, D_SaffronG, D_SaffronB);   //saffron

	glVertex3f(-0.1f, 0.25f, 0.0f);
	glVertex3f(-0.15f, 0.25f, 0.0f);

	glColor3f(D_GreenR, D_GreenG, D_GreenB);   //green

	glVertex3f(-0.15f, -0.25f, 0.0f);
	glVertex3f(-0.1f, -0.25f, 0.0f);


	//slant

	glColor3f(D_SaffronR, D_SaffronG, D_SaffronB);   //saffron

	glVertex3f(0.09f, 0.25f, 0.0f);
	glVertex3f(0.07f, 0.2f, 0.0f);
	glVertex3f(0.1f, 0.15f, 0.0f);
	glVertex3f(0.15f, 0.15f, 0.0f);


	//top

	glColor3f(D_SaffronR, D_SaffronG, D_SaffronB);   //saffron

	glVertex3f(0.09f, 0.25f, 0.0f);
	glVertex3f(-0.08f, 0.25f, 0.0f);
	glVertex3f(-0.08f, 0.2f, 0.0f);
	glVertex3f(0.07f, 0.2f, 0.0f);


	//right

	glColor3f(D_SaffronR, D_SaffronG, D_SaffronB);    //saffron

	glVertex3f(0.15f, 0.15f, 0.0f);
	glVertex3f(0.1f, 0.15f, 0.0f);

	glColor3f(D_GreenR, D_GreenG, D_GreenB);   //green

	glVertex3f(0.1f, -0.25f, 0.0f);
	glVertex3f(0.15f, -0.25f, 0.0f);


	//bottom

	glColor3f(D_GreenR, D_GreenG, D_GreenB);   //green

	glVertex3f(0.1f, -0.2f, 0.0f);
	glVertex3f(-0.08f, -0.2f, 0.0f);
	glVertex3f(-0.08f, -0.25f, 0.0f);
	glVertex3f(0.1f, -0.25f, 0.0f);

	glEnd();

}

void I2(void)
{
	//I

	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.6f, 0.2f);

	glVertex3f(0.35f, 0.25f, 0.0f);
	glVertex3f(0.3f, 0.25f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);

	glVertex3f(0.3f, -0.25f, 0.0f);
	glVertex3f(0.35f, -0.25f, 0.0f);

	glEnd();

}

void A(void)
{
	//A

	//top

	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.6f, 0.2f);

	glVertex3f(0.73f, 0.25f, 0.0f);
	glVertex3f(0.57f, 0.25f, 0.0f);
	glVertex3f(0.57f, 0.2f, 0.0f);
	glVertex3f(0.73f, 0.2f, 0.0f);


	//left

	glColor3f(1.0f, 0.6f, 0.2f);

	glVertex3f(0.62f, 0.2f, 0.0f);
	glVertex3f(0.57f, 0.25f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);

	glVertex3f(0.5f, -0.25f, 0.0f);
	glVertex3f(0.55f, -0.25f, 0.0f);


	//right

	glColor3f(1.0f, 0.6f, 0.2f);

	glVertex3f(0.73f, 0.25f, 0.0f);
	glVertex3f(0.68f, 0.2f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);

	glVertex3f(0.75f, -0.25f, 0.0f);
	glVertex3f(0.8f, -0.25f, 0.0f);


	/*//mid bar

	glColor3f(1.0f, 0.6f, 0.2f);

	glVertex3f(0.687f, 0.025f, 0.0f);
	glVertex3f(0.613f, 0.025f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);

	glVertex3f(0.608f, -0.025f, 0.0f);
	glVertex3f(0.69f, -0.025f, 0.0f);*/

	glEnd();

}


void Flag_A(void)
{
	glBegin(GL_QUADS);

	glColor3f(Flag_A_Saffron_R, Flag_A_Saffron_G, Flag_A_Saffron_B);

	glVertex3f(0.75f, 0.017f, 0.0f);
	glVertex3f(0.58f, 0.017f, 0.0f);
	glVertex3f(0.58f, 0.0067f, 0.0f);
	glVertex3f(0.75f, 0.0067f, 0.0f);

	glColor3f(Flag_A_White_R, Flag_A_White_G, Flag_A_White_B);

	glVertex3f(0.75f, 0.0067f, 0.0f);
	glVertex3f(0.58f, 0.0067f, 0.0f);
	glVertex3f(0.58f, -0.0067f, 0.0f);
	glVertex3f(0.75f, -0.0067f, 0.0f);

	glColor3f(Flag_A_Green_R, Flag_A_Green_G, Flag_A_Green_B);

	glVertex3f(0.75f, -0.0067f, 0.0f);
	glVertex3f(0.58f, -0.0067f, 0.0f);
	glVertex3f(0.58f, -0.017f, 0.0f);
	glVertex3f(0.75f, -0.017f, 0.0f);

	glEnd();
}


//Jet1

void FighterJet1(void)
{
	glBegin(GL_POLYGON);

	glColor3f(0.37f, 0.4f, 0.37f);

	glVertex3f(0.23f, 0.0f, 0.0f);
	glVertex3f(0.22f, 0.003f, 0.0f);
	glVertex3f(0.205f, 0.003f, 0.0f);
	glVertex3f(0.205f, -0.003f, 0.0f);
	glVertex3f(0.22f, -0.003f, 0.0f);

	glEnd();


	glBegin(GL_QUADS);

	glColor3f(0.37f, 0.4f, 0.37f);

	glVertex3f(0.205f, -0.003f, 0.0f);
	glVertex3f(0.205f, 0.003f, 0.0f);

	glColor3f(0.5f, 0.54f, 0.5f);

	glVertex3f(0.155f, 0.02f, 0.0f);
	glVertex3f(0.155f, -0.02f, 0.0f);



	glColor3f(0.71f, 0.77f, 0.75f);

	glVertex3f(0.15f, -0.02f, 0.0f);
	glVertex3f(0.15f, 0.02f, 0.0f);

	glColor3f(0.61f, 0.68f, 0.66f);

	glVertex3f(0.1f, 0.035f, 0.0f);
	glVertex3f(0.1f, -0.035f, 0.0f);



	glColor3f(0.38f, 0.48f, 0.54f);

	glVertex3f(0.1f, -0.035f, 0.0f);
	glVertex3f(0.1f, 0.035f, 0.0f);

	glColor3f(0.3f, 0.39f, 0.45f);

	glVertex3f(0.04f, 0.06f, 0.0f);
	glVertex3f(0.04f, -0.06f, 0.0f);



	glColor3f(0.3f, 0.39f, 0.45f);

	glVertex3f(0.04f, -0.06f, 0.0f);
	glVertex3f(0.04f, 0.06f, 0.0f);

	glColor3f(0.35f, 0.45f, 0.51f);

	glVertex3f(-0.1f, 0.06f, 0.0f);
	glVertex3f(-0.1f, -0.06f, 0.0f);



	glColor3f(0.35f, 0.45f, 0.51f);

	glVertex3f(-0.1f, 0.03f, 0.0f);
	glVertex3f(-0.1f, 0.05f, 0.0f);

	glColor3f(0.27f, 0.34f, 0.40f);

	glVertex3f(-0.21f, 0.05f, 0.0f);
	glVertex3f(-0.21f, 0.03f, 0.0f);



	glColor3f(0.35f, 0.45f, 0.51f);

	glVertex3f(-0.1f, -0.05f, 0.0f);
	glVertex3f(-0.1f, -0.03f, 0.0f);

	glColor3f(0.27f, 0.34f, 0.40f);

	glVertex3f(-0.21f, -0.03f, 0.0f);
	glVertex3f(-0.21f, -0.05f, 0.0f);

	glEnd();


	glBegin(GL_POLYGON);

	glColor3f(0.35f, 0.45f, 0.51f);


	glVertex3f(-0.1f, -0.03f, 0.0f);
	glVertex3f(-0.1f, 0.03f, 0.0f);

	glColor3f(0.27f, 0.34f, 0.40f);

	glVertex3f(-0.175f, 0.03f, 0.0f);
	glVertex3f(-0.185f, 0.025f, 0.0f);
	glVertex3f(-0.185f, -0.025f, 0.0f);
	glVertex3f(-0.175f, -0.03f, 0.0f);



	glEnd();


	glBegin(GL_QUADS);

	glColor3f(0.21f, 0.21f, 0.21f);

	glVertex3f(-0.185f, -0.025f, 0.0f);
	glVertex3f(-0.185f, 0.025f, 0.0f);

	glColor3f(0.3f, 0.3f, 0.3f);

	glVertex3f(-0.2f, 0.025f, 0.0f);
	glVertex3f(-0.2f, -0.025f, 0.0f);



	glColor3f(0.64f, 0.64f, 0.64f);

	glVertex3f(-0.2f, -0.025f, 0.0f);
	glVertex3f(-0.2f, 0.025f, 0.0f);

	glColor3f(0.74f, 0.74f, 0.74f);

	glVertex3f(-0.225f, 0.02f, 0.0f);
	glVertex3f(-0.225f, -0.02f, 0.0f);


	//Smoke	

	glColor3f(1.0f, 0.6f, 0.2f);

	glVertex3f(-0.225f, -0.02f, 0.0f);
	glVertex3f(-0.225f, 0.02f, 0.0f);

	glColor3f(0.0f, 0.0f, 0.082f);

	glVertex3f(-0.35f, 0.02f, 0.0f);
	glVertex3f(-0.35f, -0.02f, 0.0f);



	glColor3f(0.33f, 0.42f, 0.47f);

	glVertex3f(-0.06f, 0.195f, 0.0f);
	glVertex3f(-0.1f, 0.195f, 0.0f);

	glColor3f(0.38f, 0.48f, 0.54f);

	glVertex3f(-0.1f, 0.065f, 0.0f);
	glVertex3f(0.025f, 0.065f, 0.0f);



	glColor3f(0.38f, 0.48f, 0.54f);

	glVertex3f(0.025f, -0.065f, 0.0f);
	glVertex3f(-0.1f, -0.065f, 0.0f);

	glColor3f(0.33f, 0.42f, 0.47f);

	glVertex3f(-0.1f, -0.195f, 0.0f);
	glVertex3f(-0.06f, -0.195f, 0.0f);


	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(0.62f, 0.62f, 0.62f);

	glVertex3f(-0.02f, 0.205f, 0.0f);
	glVertex3f(-0.025f, 0.21f, 0.0f);
	glVertex3f(-0.11f, 0.21f, 0.0f);
	glVertex3f(-0.11f, 0.2f, 0.0f);
	glVertex3f(-0.025f, 0.2f, 0.0f);

	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(0.62f, 0.62f, 0.62f);

	glVertex3f(-0.02f, -0.205f, 0.0f);
	glVertex3f(-0.025f, -0.2f, 0.0f);
	glVertex3f(-0.11f, -0.2f, 0.0f);
	glVertex3f(-0.11f, -0.21f, 0.0f);
	glVertex3f(-0.025f, -0.21f, 0.0f);

	glEnd();


	glBegin(GL_POLYGON);

	glColor3f(0.33f, 0.42f, 0.47f);

	glVertex3f(-0.19f, 0.125f, 0.0f);
	glVertex3f(-0.21f, 0.125f, 0.0f);
	glVertex3f(-0.22f, 0.12f, 0.0f);

	glColor3f(0.38f, 0.48f, 0.54f);

	glVertex3f(-0.22f, 0.055f, 0.0f);
	glVertex3f(-0.135f, 0.055f, 0.0f);

	glEnd();


	glBegin(GL_POLYGON);

	glColor3f(0.38f, 0.48f, 0.54f);

	glVertex3f(-0.135f, -0.055f, 0.0f);
	glVertex3f(-0.22f, -0.055f, 0.0f);

	glColor3f(0.33f, 0.42f, 0.47f);

	glVertex3f(-0.22f, -0.12f, 0.0f);
	glVertex3f(-0.21f, -0.125f, 0.0f);
	glVertex3f(-0.19f, -0.125f, 0.0f);

	glEnd();



	glBegin(GL_POLYGON);

	glColor3f(0.3f, 0.3f, 0.3f);

	glVertex3f(0.13f, -0.01f, 0.0f);
	glVertex3f(0.14f, -0.005f, 0.0f);
	glVertex3f(0.14f, 0.005f, 0.0f);
	glVertex3f(0.13f, 0.01f, 0.0f);

	glColor3f(0.25f, 0.25f, 0.25f);

	glVertex3f(0.1f, 0.01f, 0.0f);
	glVertex3f(0.07f, 0.005f, 0.0f);
	glVertex3f(0.07f, -0.005f, 0.0f);
	glVertex3f(0.1f, -0.01f, 0.0f);

	glEnd();



	glBegin(GL_POLYGON);

	glColor3f(0.7f, 0.7f, 0.7f);

	glVertex3f(-0.075f, -0.01f, 0.0f);
	glVertex3f(-0.05f, 0.0f, 0.0f);
	glVertex3f(-0.075f, 0.01f, 0.0f);

	glColor3f(0.51f, 0.51f, 0.51f);

	glVertex3f(-0.16f, 0.01f, 0.0f);
	glVertex3f(-0.16f, 0.007f, 0.0f);
	glVertex3f(-0.2f, 0.007f, 0.0f);
	glVertex3f(-0.2f, -0.007f, 0.0f);
	glVertex3f(-0.16f, -0.007f, 0.0f);
	glVertex3f(-0.16f, -0.01f, 0.0f);

	glEnd();


	glBegin(GL_POLYGON);

	glColor3f(0.8f, 0.8f, 0.8f);

	glVertex3f(-0.127f, -0.002f, 0.0f);
	glVertex3f(-0.122f, 0.0f, 0.0f);
	glVertex3f(-0.127f, 0.002f, 0.0f);

	glColor3f(0.9f, 0.9f, 0.9f);

	glVertex3f(-0.22f, 0.002f, 0.0f);
	glVertex3f(-0.22f, -0.002f, 0.0f);

	glEnd();
}


//Jet2

void FighterJet2(void)
{
	glBegin(GL_POLYGON);

	glColor3f(0.37f, 0.4f, 0.37f);

	glVertex3f(0.23f, 0.0f, 0.0f);
	glVertex3f(0.22f, 0.003f, 0.0f);
	glVertex3f(0.205f, 0.003f, 0.0f);
	glVertex3f(0.205f, -0.003f, 0.0f);
	glVertex3f(0.22f, -0.003f, 0.0f);

	glEnd();


	glBegin(GL_QUADS);

	glColor3f(0.37f, 0.4f, 0.37f);

	glVertex3f(0.205f, -0.003f, 0.0f);
	glVertex3f(0.205f, 0.003f, 0.0f);

	glColor3f(0.5f, 0.54f, 0.5f);

	glVertex3f(0.155f, 0.02f, 0.0f);
	glVertex3f(0.155f, -0.02f, 0.0f);



	glColor3f(0.71f, 0.77f, 0.75f);

	glVertex3f(0.15f, -0.02f, 0.0f);
	glVertex3f(0.15f, 0.02f, 0.0f);

	glColor3f(0.61f, 0.68f, 0.66f);

	glVertex3f(0.1f, 0.035f, 0.0f);
	glVertex3f(0.1f, -0.035f, 0.0f);



	glColor3f(0.38f, 0.48f, 0.54f);

	glVertex3f(0.1f, -0.035f, 0.0f);
	glVertex3f(0.1f, 0.035f, 0.0f);

	glColor3f(0.3f, 0.39f, 0.45f);

	glVertex3f(0.04f, 0.06f, 0.0f);
	glVertex3f(0.04f, -0.06f, 0.0f);



	glColor3f(0.3f, 0.39f, 0.45f);

	glVertex3f(0.04f, -0.06f, 0.0f);
	glVertex3f(0.04f, 0.06f, 0.0f);

	glColor3f(0.35f, 0.45f, 0.51f);

	glVertex3f(-0.1f, 0.06f, 0.0f);
	glVertex3f(-0.1f, -0.06f, 0.0f);



	glColor3f(0.35f, 0.45f, 0.51f);

	glVertex3f(-0.1f, 0.03f, 0.0f);
	glVertex3f(-0.1f, 0.05f, 0.0f);

	glColor3f(0.27f, 0.34f, 0.40f);

	glVertex3f(-0.21f, 0.05f, 0.0f);
	glVertex3f(-0.21f, 0.03f, 0.0f);



	glColor3f(0.35f, 0.45f, 0.51f);

	glVertex3f(-0.1f, -0.05f, 0.0f);
	glVertex3f(-0.1f, -0.03f, 0.0f);

	glColor3f(0.27f, 0.34f, 0.40f);

	glVertex3f(-0.21f, -0.03f, 0.0f);
	glVertex3f(-0.21f, -0.05f, 0.0f);

	glEnd();


	glBegin(GL_POLYGON);

	glColor3f(0.35f, 0.45f, 0.51f);


	glVertex3f(-0.1f, -0.03f, 0.0f);
	glVertex3f(-0.1f, 0.03f, 0.0f);

	glColor3f(0.27f, 0.34f, 0.40f);

	glVertex3f(-0.175f, 0.03f, 0.0f);
	glVertex3f(-0.185f, 0.025f, 0.0f);
	glVertex3f(-0.185f, -0.025f, 0.0f);
	glVertex3f(-0.175f, -0.03f, 0.0f);



	glEnd();


	glBegin(GL_QUADS);

	glColor3f(0.21f, 0.21f, 0.21f);

	glVertex3f(-0.185f, -0.025f, 0.0f);
	glVertex3f(-0.185f, 0.025f, 0.0f);

	glColor3f(0.3f, 0.3f, 0.3f);

	glVertex3f(-0.2f, 0.025f, 0.0f);
	glVertex3f(-0.2f, -0.025f, 0.0f);



	glColor3f(0.64f, 0.64f, 0.64f);

	glVertex3f(-0.2f, -0.025f, 0.0f);
	glVertex3f(-0.2f, 0.025f, 0.0f);

	glColor3f(0.74f, 0.74f, 0.74f);

	glVertex3f(-0.225f, 0.02f, 0.0f);
	glVertex3f(-0.225f, -0.02f, 0.0f);



	//Smoke	

	glColor3f(1.0f, 1.0f, 1.0f);

	glVertex3f(-0.225f, -0.02f, 0.0f);
	glVertex3f(-0.225f, 0.02f, 0.0f);

	glColor3f(0.0f, 0.0f, 0.082f);

	glVertex3f(-0.35f, 0.02f, 0.0f);
	glVertex3f(-0.35f, -0.02f, 0.0f);

	


	glColor3f(0.33f, 0.42f, 0.47f);

	glVertex3f(-0.06f, 0.195f, 0.0f);
	glVertex3f(-0.1f, 0.195f, 0.0f);

	glColor3f(0.38f, 0.48f, 0.54f);

	glVertex3f(-0.1f, 0.065f, 0.0f);
	glVertex3f(0.025f, 0.065f, 0.0f);



	glColor3f(0.38f, 0.48f, 0.54f);

	glVertex3f(0.025f, -0.065f, 0.0f);
	glVertex3f(-0.1f, -0.065f, 0.0f);

	glColor3f(0.33f, 0.42f, 0.47f);

	glVertex3f(-0.1f, -0.195f, 0.0f);
	glVertex3f(-0.06f, -0.195f, 0.0f);


	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(0.62f, 0.62f, 0.62f);

	glVertex3f(-0.02f, 0.205f, 0.0f);
	glVertex3f(-0.025f, 0.21f, 0.0f);
	glVertex3f(-0.11f, 0.21f, 0.0f);
	glVertex3f(-0.11f, 0.2f, 0.0f);
	glVertex3f(-0.025f, 0.2f, 0.0f);

	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(0.62f, 0.62f, 0.62f);

	glVertex3f(-0.02f, -0.205f, 0.0f);
	glVertex3f(-0.025f, -0.2f, 0.0f);
	glVertex3f(-0.11f, -0.2f, 0.0f);
	glVertex3f(-0.11f, -0.21f, 0.0f);
	glVertex3f(-0.025f, -0.21f, 0.0f);

	glEnd();


	glBegin(GL_POLYGON);

	glColor3f(0.33f, 0.42f, 0.47f);

	glVertex3f(-0.19f, 0.125f, 0.0f);
	glVertex3f(-0.21f, 0.125f, 0.0f);
	glVertex3f(-0.22f, 0.12f, 0.0f);

	glColor3f(0.38f, 0.48f, 0.54f);

	glVertex3f(-0.22f, 0.055f, 0.0f);
	glVertex3f(-0.135f, 0.055f, 0.0f);

	glEnd();


	glBegin(GL_POLYGON);

	glColor3f(0.38f, 0.48f, 0.54f);

	glVertex3f(-0.135f, -0.055f, 0.0f);
	glVertex3f(-0.22f, -0.055f, 0.0f);

	glColor3f(0.33f, 0.42f, 0.47f);

	glVertex3f(-0.22f, -0.12f, 0.0f);
	glVertex3f(-0.21f, -0.125f, 0.0f);
	glVertex3f(-0.19f, -0.125f, 0.0f);

	glEnd();



	glBegin(GL_POLYGON);

	glColor3f(0.3f, 0.3f, 0.3f);

	glVertex3f(0.13f, -0.01f, 0.0f);
	glVertex3f(0.14f, -0.005f, 0.0f);
	glVertex3f(0.14f, 0.005f, 0.0f);
	glVertex3f(0.13f, 0.01f, 0.0f);

	glColor3f(0.25f, 0.25f, 0.25f);

	glVertex3f(0.1f, 0.01f, 0.0f);
	glVertex3f(0.07f, 0.005f, 0.0f);
	glVertex3f(0.07f, -0.005f, 0.0f);
	glVertex3f(0.1f, -0.01f, 0.0f);

	glEnd();



	glBegin(GL_POLYGON);

	glColor3f(0.7f, 0.7f, 0.7f);

	glVertex3f(-0.075f, -0.01f, 0.0f);
	glVertex3f(-0.05f, 0.0f, 0.0f);
	glVertex3f(-0.075f, 0.01f, 0.0f);

	glColor3f(0.51f, 0.51f, 0.51f);

	glVertex3f(-0.16f, 0.01f, 0.0f);
	glVertex3f(-0.16f, 0.007f, 0.0f);
	glVertex3f(-0.2f, 0.007f, 0.0f);
	glVertex3f(-0.2f, -0.007f, 0.0f);
	glVertex3f(-0.16f, -0.007f, 0.0f);
	glVertex3f(-0.16f, -0.01f, 0.0f);

	glEnd();


	glBegin(GL_POLYGON);

	glColor3f(0.8f, 0.8f, 0.8f);

	glVertex3f(-0.127f, -0.002f, 0.0f);
	glVertex3f(-0.122f, 0.0f, 0.0f);
	glVertex3f(-0.127f, 0.002f, 0.0f);

	glColor3f(0.9f, 0.9f, 0.9f);

	glVertex3f(-0.22f, 0.002f, 0.0f);
	glVertex3f(-0.22f, -0.002f, 0.0f);

	glEnd();

}


//Jet3

void FighterJet3(void)
{
	glBegin(GL_POLYGON);

	glColor3f(0.37f, 0.4f, 0.37f);

	glVertex3f(0.23f, 0.0f, 0.0f);
	glVertex3f(0.22f, 0.003f, 0.0f);
	glVertex3f(0.205f, 0.003f, 0.0f);
	glVertex3f(0.205f, -0.003f, 0.0f);
	glVertex3f(0.22f, -0.003f, 0.0f);

	glEnd();


	glBegin(GL_QUADS);

	glColor3f(0.37f, 0.4f, 0.37f);

	glVertex3f(0.205f, -0.003f, 0.0f);
	glVertex3f(0.205f, 0.003f, 0.0f);

	glColor3f(0.5f, 0.54f, 0.5f);

	glVertex3f(0.155f, 0.02f, 0.0f);
	glVertex3f(0.155f, -0.02f, 0.0f);



	glColor3f(0.71f, 0.77f, 0.75f);

	glVertex3f(0.15f, -0.02f, 0.0f);
	glVertex3f(0.15f, 0.02f, 0.0f);

	glColor3f(0.61f, 0.68f, 0.66f);

	glVertex3f(0.1f, 0.035f, 0.0f);
	glVertex3f(0.1f, -0.035f, 0.0f);



	glColor3f(0.38f, 0.48f, 0.54f);

	glVertex3f(0.1f, -0.035f, 0.0f);
	glVertex3f(0.1f, 0.035f, 0.0f);

	glColor3f(0.3f, 0.39f, 0.45f);

	glVertex3f(0.04f, 0.06f, 0.0f);
	glVertex3f(0.04f, -0.06f, 0.0f);



	glColor3f(0.3f, 0.39f, 0.45f);

	glVertex3f(0.04f, -0.06f, 0.0f);
	glVertex3f(0.04f, 0.06f, 0.0f);

	glColor3f(0.35f, 0.45f, 0.51f);

	glVertex3f(-0.1f, 0.06f, 0.0f);
	glVertex3f(-0.1f, -0.06f, 0.0f);



	glColor3f(0.35f, 0.45f, 0.51f);

	glVertex3f(-0.1f, 0.03f, 0.0f);
	glVertex3f(-0.1f, 0.05f, 0.0f);

	glColor3f(0.27f, 0.34f, 0.40f);

	glVertex3f(-0.21f, 0.05f, 0.0f);
	glVertex3f(-0.21f, 0.03f, 0.0f);



	glColor3f(0.35f, 0.45f, 0.51f);

	glVertex3f(-0.1f, -0.05f, 0.0f);
	glVertex3f(-0.1f, -0.03f, 0.0f);

	glColor3f(0.27f, 0.34f, 0.40f);

	glVertex3f(-0.21f, -0.03f, 0.0f);
	glVertex3f(-0.21f, -0.05f, 0.0f);

	glEnd();


	glBegin(GL_POLYGON);

	glColor3f(0.35f, 0.45f, 0.51f);


	glVertex3f(-0.1f, -0.03f, 0.0f);
	glVertex3f(-0.1f, 0.03f, 0.0f);

	glColor3f(0.27f, 0.34f, 0.40f);

	glVertex3f(-0.175f, 0.03f, 0.0f);
	glVertex3f(-0.185f, 0.025f, 0.0f);
	glVertex3f(-0.185f, -0.025f, 0.0f);
	glVertex3f(-0.175f, -0.03f, 0.0f);



	glEnd();


	glBegin(GL_QUADS);

	glColor3f(0.21f, 0.21f, 0.21f);

	glVertex3f(-0.185f, -0.025f, 0.0f);
	glVertex3f(-0.185f, 0.025f, 0.0f);

	glColor3f(0.3f, 0.3f, 0.3f);

	glVertex3f(-0.2f, 0.025f, 0.0f);
	glVertex3f(-0.2f, -0.025f, 0.0f);



	glColor3f(0.64f, 0.64f, 0.64f);

	glVertex3f(-0.2f, -0.025f, 0.0f);
	glVertex3f(-0.2f, 0.025f, 0.0f);

	glColor3f(0.74f, 0.74f, 0.74f);

	glVertex3f(-0.225f, 0.02f, 0.0f);
	glVertex3f(-0.225f, -0.02f, 0.0f);


	//Smoke	

	glColor3f(0.07f, 0.53f, 0.03f);

	glVertex3f(-0.225f, -0.02f, 0.0f);
	glVertex3f(-0.225f, 0.02f, 0.0f);

	glColor3f(0.0f, 0.0f, 0.082f);

	glVertex3f(-0.35f, 0.02f, 0.0f);
	glVertex3f(-0.35f, -0.02f, 0.0f);



	glColor3f(0.33f, 0.42f, 0.47f);

	glVertex3f(-0.06f, 0.195f, 0.0f);
	glVertex3f(-0.1f, 0.195f, 0.0f);

	glColor3f(0.38f, 0.48f, 0.54f);

	glVertex3f(-0.1f, 0.065f, 0.0f);
	glVertex3f(0.025f, 0.065f, 0.0f);



	glColor3f(0.38f, 0.48f, 0.54f);

	glVertex3f(0.025f, -0.065f, 0.0f);
	glVertex3f(-0.1f, -0.065f, 0.0f);

	glColor3f(0.33f, 0.42f, 0.47f);

	glVertex3f(-0.1f, -0.195f, 0.0f);
	glVertex3f(-0.06f, -0.195f, 0.0f);


	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(0.62f, 0.62f, 0.62f);

	glVertex3f(-0.02f, 0.205f, 0.0f);
	glVertex3f(-0.025f, 0.21f, 0.0f);
	glVertex3f(-0.11f, 0.21f, 0.0f);
	glVertex3f(-0.11f, 0.2f, 0.0f);
	glVertex3f(-0.025f, 0.2f, 0.0f);

	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(0.62f, 0.62f, 0.62f);

	glVertex3f(-0.02f, -0.205f, 0.0f);
	glVertex3f(-0.025f, -0.2f, 0.0f);
	glVertex3f(-0.11f, -0.2f, 0.0f);
	glVertex3f(-0.11f, -0.21f, 0.0f);
	glVertex3f(-0.025f, -0.21f, 0.0f);

	glEnd();


	glBegin(GL_POLYGON);

	glColor3f(0.33f, 0.42f, 0.47f);

	glVertex3f(-0.19f, 0.125f, 0.0f);
	glVertex3f(-0.21f, 0.125f, 0.0f);
	glVertex3f(-0.22f, 0.12f, 0.0f);

	glColor3f(0.38f, 0.48f, 0.54f);

	glVertex3f(-0.22f, 0.055f, 0.0f);
	glVertex3f(-0.135f, 0.055f, 0.0f);

	glEnd();


	glBegin(GL_POLYGON);

	glColor3f(0.38f, 0.48f, 0.54f);

	glVertex3f(-0.135f, -0.055f, 0.0f);
	glVertex3f(-0.22f, -0.055f, 0.0f);

	glColor3f(0.33f, 0.42f, 0.47f);

	glVertex3f(-0.22f, -0.12f, 0.0f);
	glVertex3f(-0.21f, -0.125f, 0.0f);
	glVertex3f(-0.19f, -0.125f, 0.0f);

	glEnd();



	glBegin(GL_POLYGON);

	glColor3f(0.3f, 0.3f, 0.3f);

	glVertex3f(0.13f, -0.01f, 0.0f);
	glVertex3f(0.14f, -0.005f, 0.0f);
	glVertex3f(0.14f, 0.005f, 0.0f);
	glVertex3f(0.13f, 0.01f, 0.0f);

	glColor3f(0.25f, 0.25f, 0.25f);

	glVertex3f(0.1f, 0.01f, 0.0f);
	glVertex3f(0.07f, 0.005f, 0.0f);
	glVertex3f(0.07f, -0.005f, 0.0f);
	glVertex3f(0.1f, -0.01f, 0.0f);

	glEnd();



	glBegin(GL_POLYGON);

	glColor3f(0.7f, 0.7f, 0.7f);

	glVertex3f(-0.075f, -0.01f, 0.0f);
	glVertex3f(-0.05f, 0.0f, 0.0f);
	glVertex3f(-0.075f, 0.01f, 0.0f);

	glColor3f(0.51f, 0.51f, 0.51f);

	glVertex3f(-0.16f, 0.01f, 0.0f);
	glVertex3f(-0.16f, 0.007f, 0.0f);
	glVertex3f(-0.2f, 0.007f, 0.0f);
	glVertex3f(-0.2f, -0.007f, 0.0f);
	glVertex3f(-0.16f, -0.007f, 0.0f);
	glVertex3f(-0.16f, -0.01f, 0.0f);

	glEnd();


	glBegin(GL_POLYGON);

	glColor3f(0.8f, 0.8f, 0.8f);

	glVertex3f(-0.127f, -0.002f, 0.0f);
	glVertex3f(-0.122f, 0.0f, 0.0f);
	glVertex3f(-0.127f, 0.002f, 0.0f);

	glColor3f(0.9f, 0.9f, 0.9f);

	glVertex3f(-0.22f, 0.002f, 0.0f);
	glVertex3f(-0.22f, -0.002f, 0.0f);

	glEnd();
}


//BG

void bgRect(void)
{
	//BG

	glBegin(GL_QUADS);

	glColor3f(0.0f, 0.0f, BG_blue);
	glVertex3f(2.5f, 1.5f, 0.0f);
	glVertex3f(-2.5f, 1.5f, 0.0f);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-2.5f, -1.5f, 0.0f);
	glVertex3f(2.5f, -1.5f, 0.0f);


	glEnd();

}
