#pragma once

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

/*void D(void)
{
	//D

	//left

	glBegin(GL_QUADS);

	//glColor3f(1.0f, 0.6f, 0.2f);   //saffron

	glVertex3f(-0.1f, 0.25f, 0.0f);
	glVertex3f(-0.15f, 0.25f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);   //green

	glVertex3f(-0.15f, -0.25f, 0.0f);
	glVertex3f(-0.1f, -0.25f, 0.0f);


	//slant

	glColor3f(1.0f, 0.6f, 0.2f);

	glVertex3f(0.09f, 0.25f, 0.0f);
	glVertex3f(0.07f, 0.2f, 0.0f);
	glVertex3f(0.1f, 0.15f, 0.0f);
	glVertex3f(0.15f, 0.15f, 0.0f);


	//top

	glColor3f(1.0f, 0.6f, 0.2f);

	glVertex3f(0.09f, 0.25f, 0.0f);
	glVertex3f(-0.08f, 0.25f, 0.0f);
	glVertex3f(-0.08f, 0.2f, 0.0f);
	glVertex3f(0.07f, 0.2f, 0.0f);


	//right

	glColor3f(1.0f, 0.6f, 0.2f);

	glVertex3f(0.15f, 0.15f, 0.0f);
	glVertex3f(0.1f, 0.15f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);

	glVertex3f(0.1f, -0.25f, 0.0f);
	glVertex3f(0.15f, -0.25f, 0.0f);


	//bottom

	glColor3f(0.07f, 0.53f, 0.03f);

	glVertex3f(0.1f, -0.2f, 0.0f);
	glVertex3f(-0.08f, -0.2f, 0.0f);
	glVertex3f(-0.08f, -0.25f, 0.0f);
	glVertex3f(0.1f, -0.25f, 0.0f);

	glEnd();

}*/

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

