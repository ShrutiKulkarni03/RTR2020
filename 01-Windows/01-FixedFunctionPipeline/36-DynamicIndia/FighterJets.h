#pragma once

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

