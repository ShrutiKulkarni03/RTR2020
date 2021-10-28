#include<stdlib.h>
#include "GlobalVariableDeclarations.h"
#pragma once

struct vector 
{
	float* vertArray;
	int size;
};
//struct vector* circle;

//function prototypes
struct vector* createVector(void);
//void createVector(struct vector* circle);
void pushBack(struct vector* circle, float element);
void destroyVector(struct vector* circle);


void initMercuryEllipse(void);
void initVenusEllipse(void);
void initEarthEllipse(void);
void initMarsEllipse(void);
void initJupiterEllipse(void);
void initSaturnEllipse(void);
void initUranusEllipse(void);
void initNeptuneEllipse(void);
void initPlutoEllipse(void);

struct vector* createVector(void)
{
	struct vector* circle = NULL;
	
	circle = (struct vector*)malloc(sizeof(struct vector));

	circle->vertArray = NULL;
	circle->size = 0;

	return(circle);

}

void pushBack(struct vector* circle, float element)
{
	circle->vertArray = (float*)realloc(circle->vertArray, (circle->size + 1) * sizeof(int));
	circle->size += 1;

	circle->vertArray[circle->size - 1] = element;

	//return(1);
}

void destroyVector(struct vector* circle)
{
	free(circle->vertArray);
	free(circle);
}

void initMercuryEllipse(void)
{
	mercuryEllipse = createVector();

	for (GLfloat angle = 0.0f; angle <= (2 * PI); angle += 0.1f)
	{
		GLfloat x_coord = (mercuryXLengthConstant + mercuryOrbitRadius) * cos(angle) + 0.0f;
		GLfloat y_coord = 0.0f;
		GLfloat z_coord = mercuryOrbitRadius * sin(angle) + 0.0f;

		pushBack(mercuryEllipse, x_coord);
		pushBack(mercuryEllipse, y_coord);
		pushBack(mercuryEllipse, z_coord);

		mercury_ellipse_num_vertices++;
	}

	/*****ELLIPSE VAO*****/

	glGenVertexArrays(1, &vao_mercury_ellipse);
	glBindVertexArray(vao_mercury_ellipse);

	glGenBuffers(1, &vbo_position_mercury_ellipse);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_mercury_ellipse);
	glBufferData(GL_ARRAY_BUFFER, mercury_ellipse_num_vertices * sizeof(float) * 3, mercuryEllipse->vertArray, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

	glBindVertexArray(0);
}

void initVenusEllipse(void)
{
	venusEllipse = createVector();

	for (GLfloat angle = 0.0f; angle <= (2 * PI); angle += 0.1f)
	{
		GLfloat x_coord = (venusXLengthConstant + venusOrbitRadius) * cos(angle) + 0.0f;
		GLfloat y_coord = 0.0f;
		GLfloat z_coord = venusOrbitRadius * sin(angle) + 0.0f;

		pushBack(venusEllipse, x_coord);
		pushBack(venusEllipse, y_coord);
		pushBack(venusEllipse, z_coord);

		venus_ellipse_num_vertices++;
	}

	/*****ELLIPSE VAO*****/

	glGenVertexArrays(1, &vao_venus_ellipse);
	glBindVertexArray(vao_venus_ellipse);

	glGenBuffers(1, &vbo_position_venus_ellipse);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_venus_ellipse);
	glBufferData(GL_ARRAY_BUFFER, venus_ellipse_num_vertices * sizeof(float) * 3, venusEllipse->vertArray, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

	glBindVertexArray(0);
}

void initEarthEllipse(void)
{
	earthEllipse = createVector();

	for (GLfloat angle = 0.0f; angle <= (2 * PI); angle += 0.1f)
	{
		GLfloat x_coord = (earthXLengthConstant + earthOrbitRadius) * cos(angle) + 0.0f;
		GLfloat y_coord = 0.0f;
		GLfloat z_coord = earthOrbitRadius * sin(angle) + 0.0f;

		pushBack(earthEllipse, x_coord);
		pushBack(earthEllipse, y_coord);
		pushBack(earthEllipse, z_coord);

		earth_ellipse_num_vertices++;
	}

	/*****ELLIPSE VAO*****/

	glGenVertexArrays(1, &vao_earth_ellipse);
	glBindVertexArray(vao_earth_ellipse);

	glGenBuffers(1, &vbo_position_earth_ellipse);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_earth_ellipse);
	glBufferData(GL_ARRAY_BUFFER, earth_ellipse_num_vertices * sizeof(float) * 3, earthEllipse->vertArray, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

	glBindVertexArray(0);
}

void initMarsEllipse(void)
{
	marsEllipse = createVector();

	for (GLfloat angle = 0.0f; angle <= (2 * PI); angle += 0.1f)
	{
		GLfloat x_coord = (marsXLengthConstant + marsOrbitRadius) * cos(angle) + 0.0f;
		GLfloat y_coord = 0.0f;
		GLfloat z_coord = marsOrbitRadius * sin(angle) + 0.0f;

		pushBack(marsEllipse, x_coord);
		pushBack(marsEllipse, y_coord);
		pushBack(marsEllipse, z_coord);

		mars_ellipse_num_vertices++;
	}

	/*****ELLIPSE VAO*****/

	glGenVertexArrays(1, &vao_mars_ellipse);
	glBindVertexArray(vao_mars_ellipse);

	glGenBuffers(1, &vbo_position_mars_ellipse);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_mars_ellipse);
	glBufferData(GL_ARRAY_BUFFER, mars_ellipse_num_vertices * sizeof(float) * 3, marsEllipse->vertArray, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

	glBindVertexArray(0);
}

void initJupiterEllipse(void)
{
	jupiterEllipse = createVector();

	for (GLfloat angle = 0.0f; angle <= (2 * PI); angle += 0.1f)
	{
		GLfloat x_coord = (jupiterXLengthConstant + jupiterOrbitRadius) * cos(angle) + 0.0f;
		GLfloat y_coord = 0.0f;
		GLfloat z_coord = jupiterOrbitRadius * sin(angle) + 0.0f;

		pushBack(jupiterEllipse, x_coord);
		pushBack(jupiterEllipse, y_coord);
		pushBack(jupiterEllipse, z_coord);

		jupiter_ellipse_num_vertices++;
	}

	/*****ELLIPSE VAO*****/

	glGenVertexArrays(1, &vao_jupiter_ellipse);
	glBindVertexArray(vao_jupiter_ellipse);

	glGenBuffers(1, &vbo_position_jupiter_ellipse);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_jupiter_ellipse);
	glBufferData(GL_ARRAY_BUFFER, jupiter_ellipse_num_vertices * sizeof(float) * 3, jupiterEllipse->vertArray, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

	glBindVertexArray(0);
}

void initSaturnEllipse(void)
{
	saturnEllipse = createVector();

	for (GLfloat angle = 0.0f; angle <= (2 * PI); angle += 0.1f)
	{
		GLfloat x_coord = (saturnXLengthConstant + saturnOrbitRadius) * cos(angle) + 0.0f;
		GLfloat y_coord = 0.0f;
		GLfloat z_coord = saturnOrbitRadius * sin(angle) + 0.0f;

		pushBack(saturnEllipse, x_coord);
		pushBack(saturnEllipse, y_coord);
		pushBack(saturnEllipse, z_coord);

		saturn_ellipse_num_vertices++;
	}

	/*****ELLIPSE VAO*****/

	glGenVertexArrays(1, &vao_saturn_ellipse);
	glBindVertexArray(vao_saturn_ellipse);

	glGenBuffers(1, &vbo_position_saturn_ellipse);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_saturn_ellipse);
	glBufferData(GL_ARRAY_BUFFER, saturn_ellipse_num_vertices * sizeof(float) * 3, saturnEllipse->vertArray, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

	glBindVertexArray(0);
}

void initSaturnRingEllipse(void)
{
	saturnRingEllipse = createVector();

	for (GLfloat angle = 0.0f; angle <= (2 * PI); angle += 0.01f)
	{
		GLfloat x_coord = (saturnRingXLengthConstant + saturnRingOrbitRadius) * cos(angle) + 0.0f;
		GLfloat y_coord = saturnRingOrbitRadius * sin(angle) + 0.0f;
		GLfloat z_coord = 0.0f;

		pushBack(saturnRingEllipse, x_coord);
		pushBack(saturnRingEllipse, y_coord);
		pushBack(saturnRingEllipse, z_coord);

		saturnRing_ellipse_num_vertices++;
	}

	/*****ELLIPSE VAO*****/

	glGenVertexArrays(1, &vao_saturnRing_ellipse);
	glBindVertexArray(vao_saturnRing_ellipse);

	glGenBuffers(1, &vbo_position_saturnRing_ellipse);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_saturnRing_ellipse);
	glBufferData(GL_ARRAY_BUFFER, saturnRing_ellipse_num_vertices * sizeof(float) * 3, saturnRingEllipse->vertArray, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

	glBindVertexArray(0);
}

void initUranusEllipse(void)
{
	uranusEllipse = createVector();

	for (GLfloat angle = 0.0f; angle <= (2 * PI); angle += 0.1f)
	{
		GLfloat x_coord = (uranusXLengthConstant + uranusOrbitRadius) * cos(angle) + 0.0f;
		GLfloat y_coord = 0.0f;
		GLfloat z_coord = uranusOrbitRadius * sin(angle) + 0.0f;

		pushBack(uranusEllipse, x_coord);
		pushBack(uranusEllipse, y_coord);
		pushBack(uranusEllipse, z_coord);

		uranus_ellipse_num_vertices++;
	}

	/*****ELLIPSE VAO*****/

	glGenVertexArrays(1, &vao_uranus_ellipse);
	glBindVertexArray(vao_uranus_ellipse);

	glGenBuffers(1, &vbo_position_uranus_ellipse);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_uranus_ellipse);
	glBufferData(GL_ARRAY_BUFFER, uranus_ellipse_num_vertices * sizeof(float) * 3, uranusEllipse->vertArray, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

	glBindVertexArray(0);
}

void initNeptuneEllipse(void)
{
	neptuneEllipse = createVector();

	for (GLfloat angle = 0.0f; angle <= (2 * PI); angle += 0.1f)
	{
		GLfloat x_coord = (neptuneXLengthConstant + neptuneOrbitRadius) * cos(angle) + 0.0f;
		GLfloat y_coord = 0.0f;
		GLfloat z_coord = neptuneOrbitRadius * sin(angle) + 0.0f;

		pushBack(neptuneEllipse, x_coord);
		pushBack(neptuneEllipse, y_coord);
		pushBack(neptuneEllipse, z_coord);

		neptune_ellipse_num_vertices++;
	}

	/*****ELLIPSE VAO*****/

	glGenVertexArrays(1, &vao_neptune_ellipse);
	glBindVertexArray(vao_neptune_ellipse);

	glGenBuffers(1, &vbo_position_neptune_ellipse);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_neptune_ellipse);
	glBufferData(GL_ARRAY_BUFFER, neptune_ellipse_num_vertices * sizeof(float) * 3, neptuneEllipse->vertArray, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

	glBindVertexArray(0);
}

void initPlutoEllipse(void)
{
	plutoEllipse = createVector();

	for (GLfloat angle = 0.0f; angle <= (2 * PI); angle += 0.1f)
	{
		GLfloat x_coord = (plutoXLengthConstant + plutoOrbitRadius) * cos(angle) + 0.0f;
		GLfloat y_coord = 0.0f;
		GLfloat z_coord = plutoOrbitRadius * sin(angle) + 0.0f;

		pushBack(plutoEllipse, x_coord);
		pushBack(plutoEllipse, y_coord);
		pushBack(plutoEllipse, z_coord);

		pluto_ellipse_num_vertices++;
	}

	/*****ELLIPSE VAO*****/

	glGenVertexArrays(1, &vao_pluto_ellipse);
	glBindVertexArray(vao_pluto_ellipse);

	glGenBuffers(1, &vbo_position_pluto_ellipse);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_pluto_ellipse);
	glBufferData(GL_ARRAY_BUFFER, pluto_ellipse_num_vertices * sizeof(float) * 3, plutoEllipse->vertArray, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

	glBindVertexArray(0);
}
