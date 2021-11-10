#include "GlobalVariableDeclarations.h"
#include "stack.h"
#include "SphereMesh.h"
#include "CircleVector.h"
#include "shaders.h"
#include "vmath.h"
#pragma once

//function prototypes
void RenderText(GLuint, std::string, GLfloat, GLfloat, GLfloat, vec4);

void drawSolarSystem(void);
void drawFocusedPlanets(void);
void drawSunGodrays(void);
void drawSkybox(void);
void drawCubeForCubemap(void);
void drawStarfield(void);


void renderTextMercury(void);
void renderTextVenus(void);
void renderTextEarth(void);
void renderTextMars(void);
void renderTextJupiter(void);
void renderTextSaturn(void);
void renderTextUranus(void);
void renderTextNeptune(void);
void renderTextPluto(void);

//functions

void drawSolarSystem(void)
{
	/**********************************DRAW PLANETS*********************************/

	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projectionMatrix;
	mat4 translateMatrix;
	mat4 scaleMatrix;
	mat4 rotateXMatrix;
	mat4 rotateYMatrix;
	mat4 rotateZMatrix;

	//start using OpenGL program object
	glUseProgram(sceneShaderProgramObject);

	glUniform3f(lAUniformS, 0.2f, 0.2f, 0.2f);
	glUniform3f(lDUniformS, 1.0f, 1.0f, 1.0f);
	glUniform3f(lSUniformS, 0.0f, 0.0f, 0.0f);
	glUniform4f(positionalLightPositionUniformS, 0.0f, 0.0f, 0.0f, 1.0f);

	glUniform1f(kShininessUniformS, 50.0);

	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	rotateXMatrix = mat4::identity();
	rotateYMatrix = mat4::identity();
	rotateZMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();

	viewMatrix = camera;
	Push(pStack, viewMatrix);
	Push(pStack, modelMatrix);

	//SUN

	glUniform1i(passUniformS, 2);     //pass 2 (scene lighting)
	glUniform1i(lightUniformS, 1);     //positional light
	scaleMatrix = scale(0.4f, 0.4f, 0.4f);
	rotateXMatrix = rotate(90.0f, 1.0f, 0.0f, 0.0f);             //axis change

	modelMatrix = modelMatrix * scaleMatrix * rotateXMatrix;

	projectionMatrix = perspectiveProjectionMatrix;
	glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sun_texture);
	glUniform1i(textureSamplerUniformS, 0);

	drawSphere();

	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = Pop(pStack);
	//modelMatrix = Pop(pStack);

	Push(pStack, modelMatrix);

	//PLANETS

	glUniform3f(lAUniformS, 0.5f, 0.5f, 0.5f);
	glUniform3f(lDUniformS, 1.0f, 1.0f, 1.0f);
	glUniform3f(lSUniformS, 0.0f, 0.0f, 0.0f);

	glUniform4f(pointLightPositionUniformS, 0.0f, 1.0f, 0.0f, 1.0f);
	glUniform1f(lightConstantUniformS, 1.0f);
	glUniform1f(lightLinearUniformS, 0.12f);
	glUniform1f(lightQuadraticUniformS, 0.032f);

	//MERCURY

	if (isMercuryInOrbit == true)
	{
		//orbit
		glUniform1i(passUniformS, 1);     //pass 1 (color)
		rotateZMatrix = rotate(orbitsZRot, 0.0f, 0.0f, 1.0f);                    //slightly tilting orbits
		rotateXMatrix = rotate(orbitsXRot, 1.0f, 0.0f, 0.0f);					//slightly tilting orbits
		modelMatrix = modelMatrix * rotateZMatrix * rotateXMatrix;
		Push(pStack, modelMatrix);
		modelMatrix = Pop(pStack);

		projectionMatrix = perspectiveProjectionMatrix;  //pre-multiplication of matrices
		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

		//bind vao_circle
		glBindVertexArray(vao_mercury_ellipse);
		glVertexAttrib4f(SPK_ATTRIBUTE_COLOR, 0.92f, 0.79f, 0.61f, 0.5f);
		glDrawArrays(GL_LINE_LOOP, 0, mercury_ellipse_num_vertices);
		glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);
		//unbind vao_circle
		glBindVertexArray(0);
	}

	//planet	
	//position after transforming from sun is the radius of next transformation... so use the transformed x variable in place of bracket in 1st parameter
	glUniform1i(passUniformS, 2);     //pass 2 (scene lighting)
	glUniform1i(lightUniformS, 0);     //point light

	if (isMercuryInOrbit == true)
	{
		mercuryXPos = (mercuryXLengthConstant + mercuryOrbitRadius) * cosf(mercuryRevAngle) + 0.0f;
		mercuryZPos = mercuryOrbitRadius * sinf(mercuryRevAngle) + 0.0f;
	}

	translateMatrix = translate(mercuryXPos, mercuryYPos, mercuryZPos);    //to revolve in ellipse
	scaleMatrix = scale(0.04879f, 0.04879f, 0.04879f);
	rotateXMatrix = rotate(90.0f, 1.0f, 0.0f, 0.0f);             //axis change

	modelMatrix = modelMatrix * translateMatrix * scaleMatrix * rotateXMatrix;

	Push(pStack, modelMatrix);

	rotateZMatrix = rotate(mercuryRotAngle, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix * rotateZMatrix;

	projectionMatrix = perspectiveProjectionMatrix;
	glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mercury_texture);
	glUniform1i(textureSamplerUniformS, 0);

	drawSphere();

	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = Pop(pStack);
	modelMatrix = Pop(pStack);

	Push(pStack, modelMatrix);

	//VENUS

	if (isVenusInOrbit == true)
	{
		//orbit
		glUniform1i(passUniformS, 1);     //pass 1 (color)
		rotateZMatrix = rotate(orbitsZRot, 0.0f, 0.0f, 1.0f);                    //slightly tilting orbits
		rotateXMatrix = rotate(orbitsXRot, 1.0f, 0.0f, 0.0f);					//slightly tilting orbits
		modelMatrix = modelMatrix * rotateZMatrix * rotateXMatrix;
		Push(pStack, modelMatrix);
		modelMatrix = Pop(pStack);

		projectionMatrix = perspectiveProjectionMatrix;  //pre-multiplication of matrices
		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

		//bind vao_circle
		glBindVertexArray(vao_venus_ellipse);
		glVertexAttrib4f(SPK_ATTRIBUTE_COLOR, 0.91f, 0.69f, 0.32f, 0.5f);
		glDrawArrays(GL_LINE_LOOP, 0, venus_ellipse_num_vertices);
		glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);
		//unbind vao_circle
		glBindVertexArray(0);
	}

	//planet
	//position after transforming from sun is the radius of next transformation... so use the transformed x variable in place of bracket in 1st parameter
	glUniform1i(passUniformS, 2);     //pass 2 (scene lighting)
	glUniform1i(lightUniformS, 0);     //point light

	if (isVenusInOrbit == true)
	{
		venusXPos = (venusXLengthConstant + venusOrbitRadius) * cosf(venusRevAngle) + 0.0f;
		venusZPos = venusOrbitRadius * sinf(venusRevAngle) + 0.0f;
	}

	translateMatrix = translate(venusXPos, venusYPos, venusZPos);    //to revolve in ellipse
	scaleMatrix = scale(0.11588f, 0.11588f, 0.11588f);
	rotateXMatrix = rotate(90.0f, 1.0f, 0.0f, 0.0f);             //axis change
	modelMatrix = modelMatrix * translateMatrix * scaleMatrix * rotateXMatrix;

	Push(pStack, modelMatrix);

	rotateZMatrix = rotate(venusRotAngle, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix * rotateZMatrix;

	projectionMatrix = perspectiveProjectionMatrix;
	glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, venus_texture);
	glUniform1i(textureSamplerUniformS, 0);

	drawSphere();

	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = Pop(pStack);
	modelMatrix = Pop(pStack);

	Push(pStack, modelMatrix);

	//EARTH

	if (isEarthInOrbit)
	{
		//orbit
		glUniform1i(passUniformS, 1);     //pass 1 (color)
		rotateZMatrix = rotate(orbitsZRot, 0.0f, 0.0f, 1.0f);                    //slightly tilting orbits
		rotateXMatrix = rotate(orbitsXRot, 1.0f, 0.0f, 0.0f);					//slightly tilting orbits
		modelMatrix = modelMatrix * rotateZMatrix * rotateXMatrix;
		Push(pStack, modelMatrix);
		modelMatrix = Pop(pStack);

		projectionMatrix = perspectiveProjectionMatrix;  //pre-multiplication of matrices
		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

		//bind vao_circle
		glBindVertexArray(vao_earth_ellipse);
		glVertexAttrib4f(SPK_ATTRIBUTE_COLOR, 0.11f, 0.23f, 0.45f, 0.5f);
		glDrawArrays(GL_LINE_LOOP, 0, earth_ellipse_num_vertices);
		glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);
		//unbind vao_circle
		glBindVertexArray(0);
	}

	//planet
	//position after transforming from sun is the radius of next transformation... so use the transformed x variable in place of bracket in 1st parameter
	glUniform1i(passUniformS, 2);     //pass 2 (scene lighting)
	glUniform1i(lightUniformS, 0);     //point light

	if (isEarthInOrbit == true)
	{
		earthXPos = (earthXLengthConstant + earthOrbitRadius) * cosf(earthRevAngle) + 0.0f;
		earthZPos = earthOrbitRadius * sinf(earthRevAngle) + 0.0f;
	}

	translateMatrix = translate(earthXPos, earthYPos, earthZPos);    //to revolve in ellipse
	scaleMatrix = scale(0.12747f, 0.12747f, 0.12747f);
	rotateXMatrix = rotate(90.0f, 1.0f, 0.0f, 0.0f);             //axis change
	modelMatrix = modelMatrix * translateMatrix * scaleMatrix * rotateXMatrix;

	Push(pStack, modelMatrix);

	rotateZMatrix = rotate(earthRotAngle, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix * rotateZMatrix;

	projectionMatrix = perspectiveProjectionMatrix;
	glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earth_texture);
	glUniform1i(textureSamplerUniformS, 0);

	drawSphere();

	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = Pop(pStack);

	//Push(pStack, modelMatrix);

	////moon
	//glUniform1i(passUniformS, 2);     //pass 2 (scene lighting)
	//rotateZMatrix = rotate(earthMoonRevAngle, 0.0f, 0.0f, 1.0f);
	//translateMatrix = translate(0.5f, 0.0f, 0.0f);
	//scaleMatrix = scale(0.5f, 0.5f, 0.5f);

	//modelMatrix = modelMatrix * rotateZMatrix * translateMatrix * scaleMatrix;

	//rotateZMatrix = rotate(earthMoonRotAngle, 0.0f, 0.0f, 1.0f);
	//modelMatrix = modelMatrix * rotateZMatrix;

	//projectionMatrix = perspectiveProjectionMatrix; 
	//glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
	//glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
	//glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, earthMoon_texture);
	//glUniform1i(textureSamplerUniformS, 0);

	//drawSphere();

	//glBindTexture(GL_TEXTURE_2D, 0);

	//modelMatrix = Pop(pStack);
	modelMatrix = Pop(pStack);

	Push(pStack, modelMatrix);

	//MARS

	if (isMarsInOrbit == true)
	{
		//orbit
		glUniform1i(passUniformS, 1);     //pass 1 (color)
		rotateZMatrix = rotate(orbitsZRot, 0.0f, 0.0f, 1.0f);                    //slightly tilting orbits
		rotateXMatrix = rotate(orbitsXRot, 1.0f, 0.0f, 0.0f);					//slightly tilting orbits
		modelMatrix = modelMatrix * rotateZMatrix * rotateXMatrix;
		Push(pStack, modelMatrix);
		modelMatrix = Pop(pStack);

		projectionMatrix = perspectiveProjectionMatrix;  //pre-multiplication of matrices
		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

		//bind vao_circle
		glBindVertexArray(vao_mars_ellipse);
		glVertexAttrib4f(SPK_ATTRIBUTE_COLOR, 0.98f, 0.66f, 0.48f, 0.5f);
		glDrawArrays(GL_LINE_LOOP, 0, mars_ellipse_num_vertices);
		glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);
		//unbind vao_circle
		glBindVertexArray(0);
	}

	//planet
	//position after transforming from sun is the radius of next transformation... so use the transformed x variable in place of bracket in 1st parameter
	glUniform1i(passUniformS, 2);     //pass 2 (scene lighting)
	glUniform1i(lightUniformS, 0);     //point light

	if (isMarsInOrbit == true)
	{
		marsXPos = (marsXLengthConstant + marsOrbitRadius) * cosf(marsRevAngle) + 0.0f;
		marsZPos = marsOrbitRadius * sinf(marsRevAngle) + 0.0f;
	}

	translateMatrix = translate(marsXPos, marsYPos, marsZPos);    //to revolve in ellipse
	scaleMatrix = scale(0.06779f, 0.06779f, 0.06779f);
	rotateXMatrix = rotate(90.0f, 1.0f, 0.0f, 0.0f);             //axis change
	modelMatrix = modelMatrix * translateMatrix * scaleMatrix * rotateXMatrix;

	Push(pStack, modelMatrix);

	rotateZMatrix = rotate(marsRotAngle, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix * rotateZMatrix;

	projectionMatrix = perspectiveProjectionMatrix;
	glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mars_texture);
	glUniform1i(textureSamplerUniformS, 0);

	drawSphere();

	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = Pop(pStack);
	modelMatrix = Pop(pStack);

	Push(pStack, modelMatrix);

	//JUPITER

	if (isJupiterInOrbit == true)
	{
		//orbit
		glUniform1i(passUniformS, 1);     //pass 1 (color)
		rotateZMatrix = rotate(orbitsZRot, 0.0f, 0.0f, 1.0f);                    //slightly tilting orbits
		rotateXMatrix = rotate(orbitsXRot, 1.0f, 0.0f, 0.0f);					//slightly tilting orbits
		modelMatrix = modelMatrix * rotateZMatrix * rotateXMatrix;
		Push(pStack, modelMatrix);
		modelMatrix = Pop(pStack);

		projectionMatrix = perspectiveProjectionMatrix;  //pre-multiplication of matrices
		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

		//bind vao_circle
		glBindVertexArray(vao_jupiter_ellipse);
		glVertexAttrib4f(SPK_ATTRIBUTE_COLOR, 0.67f, 0.58f, 0.49f, 0.5f);
		glDrawArrays(GL_LINE_LOOP, 0, jupiter_ellipse_num_vertices);
		glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);
		//unbind vao_circle
		glBindVertexArray(0);
	}

	//planet
	//position after transforming from sun is the radius of next transformation... so use the transformed x variable in place of bracket in 1st parameter
	glUniform1i(passUniformS, 2);     //pass 2 (scene lighting)
	glUniform1i(lightUniformS, 0);     //point light

	if (isJupiterInOrbit == true)
	{
		jupiterXPos = (jupiterXLengthConstant + jupiterOrbitRadius) * cosf(jupiterRevAngle) + 0.0f;
		jupiterZPos = jupiterOrbitRadius * sinf(jupiterRevAngle) + 0.0f;
	}

	translateMatrix = translate(jupiterXPos, jupiterYPos, jupiterZPos);    //to revolve in ellipse
	scaleMatrix = scale(0.25f, 0.25f, 0.25f);
	rotateXMatrix = rotate(90.0f, 1.0f, 0.0f, 0.0f);             //axis change
	modelMatrix = modelMatrix * translateMatrix * scaleMatrix * rotateXMatrix;

	Push(pStack, modelMatrix);

	rotateZMatrix = rotate(jupiterRotAngle, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix * rotateZMatrix;

	projectionMatrix = perspectiveProjectionMatrix;
	glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, jupiter_texture);
	glUniform1i(textureSamplerUniformS, 0);

	drawSphere();

	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = Pop(pStack);
	modelMatrix = Pop(pStack);

	Push(pStack, modelMatrix);

	//SATURN

	if (isSaturnInOrbit == true)
	{
		//orbit
		glUniform1i(passUniformS, 1);     //pass 1 (color)
		rotateZMatrix = rotate(orbitsZRot, 0.0f, 0.0f, 1.0f);                    //slightly tilting orbits
		rotateXMatrix = rotate(orbitsXRot, 1.0f, 0.0f, 0.0f);					//slightly tilting orbits
		modelMatrix = modelMatrix * rotateZMatrix * rotateXMatrix;
		Push(pStack, modelMatrix);
		modelMatrix = Pop(pStack);

		projectionMatrix = perspectiveProjectionMatrix;  //pre-multiplication of matrices
		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

		//bind vao_circle
		glBindVertexArray(vao_saturn_ellipse);
		glVertexAttrib4f(SPK_ATTRIBUTE_COLOR, 0.89f, 0.8f, 0.58f, 0.5f);
		glDrawArrays(GL_LINE_LOOP, 0, saturn_ellipse_num_vertices);
		glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);
		//unbind vao_circle
		glBindVertexArray(0);
	}

	//planet
	//position after transforming from sun is the radius of next transformation... so use the transformed x variable in place of bracket in 1st parameter
	glUniform1i(passUniformS, 2);     //pass 2 (scene lighting)
	glUniform1i(lightUniformS, 0);     //point light

	if (isSaturnInOrbit == true)
	{
		saturnXPos = (saturnXLengthConstant + saturnOrbitRadius) * cosf(saturnRevAngle) + 0.0f;
		saturnZPos = saturnOrbitRadius * sinf(saturnRevAngle) + 0.0f;

		//glLineWidth(50.0f);
	}

	translateMatrix = translate(saturnXPos, saturnYPos, saturnZPos);    //to revolve in ellipse
	scaleMatrix = scale(0.17f, 0.17f, 0.17f);
	rotateXMatrix = rotate(90.0f, 1.0f, 0.0f, 0.0f);             //axis change
	modelMatrix = modelMatrix * translateMatrix * scaleMatrix * rotateXMatrix;

	Push(pStack, modelMatrix);

	rotateZMatrix = rotate(saturnRotAngle, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix * rotateZMatrix;

	projectionMatrix = perspectiveProjectionMatrix;
	glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, saturn_texture);
	glUniform1i(textureSamplerUniformS, 0);

	drawSphere();

	glBindTexture(GL_TEXTURE_2D, 0);

	//ring
	rotateXMatrix = rotate(0.0f, 1.0f, 0.0f, 0.0f);
	rotateZMatrix = rotate(0.0f, 0.0f, 0.0f, 1.0f);

	scaleMatrix = scale(1.0f, 1.0f, 0.1f);
	modelMatrix = modelMatrix * scaleMatrix * rotateXMatrix * rotateZMatrix;


	projectionMatrix = perspectiveProjectionMatrix;
	glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, saturn_ring_texture);
	glUniform1i(textureSamplerUniformS, 0);

	drawTorus();

	glBindTexture(GL_TEXTURE_2D, 0);

	//glUniform1i(passUniformS, 1);     //pass 1 (color)

	////bind vao_circle
	//glBindVertexArray(vao_saturnRing_ellipse);
	//glVertexAttrib4f(SPK_ATTRIBUTE_COLOR, 0.72f, 0.67f, 0.54f, 1.0f);
	//glDrawArrays(GL_LINE_LOOP, 0, saturnRing_ellipse_num_vertices);
	//glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);
	////unbind vao_circle
	//glBindVertexArray(0);

	//glLineWidth(1.0f);

	modelMatrix = Pop(pStack);
	modelMatrix = Pop(pStack);

	Push(pStack, modelMatrix);

	//URANUS

	if (isUranusInOrbit == true)
	{
		//orbit
		glUniform1i(passUniformS, 1);     //pass 1 (color)
		rotateZMatrix = rotate(orbitsZRot, 0.0f, 0.0f, 1.0f);                    //slightly tilting orbits
		rotateXMatrix = rotate(orbitsXRot, 1.0f, 0.0f, 0.0f);					//slightly tilting orbits
		modelMatrix = modelMatrix * rotateZMatrix * rotateXMatrix;
		Push(pStack, modelMatrix);
		modelMatrix = Pop(pStack);

		projectionMatrix = perspectiveProjectionMatrix;  //pre-multiplication of matrices
		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

		//bind vao_circle
		glBindVertexArray(vao_uranus_ellipse);
		glVertexAttrib4f(SPK_ATTRIBUTE_COLOR, 0.57f, 0.76f, 0.79f, 0.5f);
		glDrawArrays(GL_LINE_LOOP, 0, uranus_ellipse_num_vertices);
		glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);
		//unbind vao_circle
		glBindVertexArray(0);
	}

	//planet
	//position after transforming from sun is the radius of next transformation... so use the transformed x variable in place of bracket in 1st parameter
	glUniform1i(passUniformS, 2);     //pass 2 (scene lighting)
	glUniform1i(lightUniformS, 0);     //point light

	if (isUranusInOrbit == true)
	{
		uranusXPos = (uranusXLengthConstant + uranusOrbitRadius) * cosf(uranusRevAngle) + 0.0f;
		uranusZPos = uranusOrbitRadius * sinf(uranusRevAngle) + 0.0f;
	}

	translateMatrix = translate(uranusXPos, uranusYPos, uranusZPos);    //to revolve in ellipse
	scaleMatrix = scale(0.13f, 0.13f, 0.13f);
	rotateXMatrix = rotate(90.0f, 1.0f, 0.0f, 0.0f);             //axis change
	modelMatrix = modelMatrix * translateMatrix * scaleMatrix * rotateXMatrix;

	Push(pStack, modelMatrix);

	rotateZMatrix = rotate(uranusRotAngle, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix * rotateZMatrix;

	projectionMatrix = perspectiveProjectionMatrix;
	glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, uranus_texture);
	glUniform1i(textureSamplerUniformS, 0);

	drawSphere();

	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = Pop(pStack);
	modelMatrix = Pop(pStack);

	Push(pStack, modelMatrix);


	//NEPTUNE

	if (isNeptuneInOrbit == true)
	{
		//orbit
		glUniform1i(passUniformS, 1);     //pass 1 (color)
		rotateZMatrix = rotate(orbitsZRot, 0.0f, 0.0f, 1.0f);                    //slightly tilting orbits
		rotateXMatrix = rotate(orbitsXRot, 1.0f, 0.0f, 0.0f);					//slightly tilting orbits
		modelMatrix = modelMatrix * rotateZMatrix * rotateXMatrix;
		Push(pStack, modelMatrix);
		modelMatrix = Pop(pStack);

		projectionMatrix = perspectiveProjectionMatrix;  //pre-multiplication of matrices
		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

		//bind vao_circle
		glBindVertexArray(vao_neptune_ellipse);
		glVertexAttrib4f(SPK_ATTRIBUTE_COLOR, 0.25f, 0.47f, 0.82f, 0.5f);
		glDrawArrays(GL_LINE_LOOP, 0, neptune_ellipse_num_vertices);
		glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);
		//unbind vao_circle
		glBindVertexArray(0);
	}

	//planet
	//position after transforming from sun is the radius of next transformation... so use the transformed x variable in place of bracket in 1st parameter
	glUniform1i(passUniformS, 2);     //pass 2 (scene lighting)
	glUniform1i(lightUniformS, 0);     //point light

	if (isNeptuneInOrbit == true)
	{
		neptuneXPos = (neptuneXLengthConstant + neptuneOrbitRadius) * cosf(neptuneRevAngle) + 0.0f;
		neptuneZPos = neptuneOrbitRadius * sinf(neptuneRevAngle) + 0.0f;
	}

	translateMatrix = translate(neptuneXPos, neptuneYPos, neptuneZPos);    //to revolve in ellipse
	scaleMatrix = scale(0.12f, 0.12f, 0.12f);
	rotateXMatrix = rotate(90.0f, 1.0f, 0.0f, 0.0f);             //axis change
	modelMatrix = modelMatrix * translateMatrix * scaleMatrix * rotateXMatrix;

	Push(pStack, modelMatrix);

	rotateZMatrix = rotate(neptuneRotAngle, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix * rotateZMatrix;

	projectionMatrix = perspectiveProjectionMatrix;
	glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, neptune_texture);
	glUniform1i(textureSamplerUniformS, 0);

	drawSphere();

	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = Pop(pStack);
	modelMatrix = Pop(pStack);

	Push(pStack, modelMatrix);


	//PLUTO

	if (isPlutoInOrbit == true)
	{
		//orbit
		glUniform1i(passUniformS, 1);     //pass 1 (color)
		rotateZMatrix = rotate(orbitsZRot, 0.0f, 0.0f, 1.0f);                    //slightly tilting orbits
		rotateXMatrix = rotate(orbitsXRot, 1.0f, 0.0f, 0.0f);					//slightly tilting orbits
		modelMatrix = modelMatrix * rotateZMatrix * rotateXMatrix;
		Push(pStack, modelMatrix);
		modelMatrix = Pop(pStack);

		projectionMatrix = perspectiveProjectionMatrix;  //pre-multiplication of matrices
		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

		//bind vao_circle
		glBindVertexArray(vao_pluto_ellipse);
		glVertexAttrib4f(SPK_ATTRIBUTE_COLOR, 0.47f, 0.51f, 0.55f, 0.5f);
		glDrawArrays(GL_LINE_LOOP, 0, pluto_ellipse_num_vertices);
		glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);
		//unbind vao_circle
		glBindVertexArray(0);
	}

	//planet
	//position after transforming from sun is the radius of next transformation... so use the transformed x variable in place of bracket in 1st parameter
	glUniform1i(passUniformS, 2);     //pass 2 (scene lighting)
	glUniform1i(lightUniformS, 0);     //point light

	if (isPlutoInOrbit == true)
	{
		plutoXPos = (plutoXLengthConstant + plutoOrbitRadius) * cosf(plutoRevAngle) + 0.0f;
		plutoZPos = plutoOrbitRadius * sinf(plutoRevAngle) + 0.0f;
	}

	translateMatrix = translate(plutoXPos, plutoYPos, plutoZPos);    //to revolve in ellipse
	scaleMatrix = scale(0.025f, 0.025f, 0.025f);
	rotateXMatrix = rotate(90.0f, 1.0f, 0.0f, 0.0f);             //axis change
	modelMatrix = modelMatrix * translateMatrix * scaleMatrix * rotateXMatrix;

	Push(pStack, modelMatrix);

	rotateZMatrix = rotate(plutoRotAngle, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix * rotateZMatrix;

	projectionMatrix = perspectiveProjectionMatrix;
	glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, pluto_texture);
	glUniform1i(textureSamplerUniformS, 0);

	drawSphere();

	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = Pop(pStack);
	modelMatrix = Pop(pStack);

	Push(pStack, modelMatrix);

	//last pop of 1st push
	viewMatrix = Pop(pStack);
	modelMatrix = Pop(pStack);

	//stop using OpenGL program object
	glUseProgram(0);
}

void drawFocusedPlanets(void)
{
	//start using OpenGL program object
	glUseProgram(sceneShaderProgramObject);

	glUniform1i(passUniformS, 2);     //pass 2 (scene lighting)
	glUniform1i(lightUniformS, 1);     //pass 2 (scene lighting)

	glUniform3f(lAUniformS, 0.2f, 0.2f, 0.2f);
	glUniform3f(lDUniformS, 1.0f, 1.0f, 1.0f);
	glUniform3f(lSUniformS, 0.0f, 0.0f, 0.0f);
	glUniform4f(positionalLightPositionUniformS, 100.0f, 100.0f, 0.0f, 1.0f);

	glUniform1f(kShininessUniformS, 50.0);

	mat4 modelMatrix1;
	mat4 viewMatrix1;
	mat4 projectionMatrix1;
	mat4 translateMatrix1;
	mat4 scaleMatrix1;
	mat4 rotateXMatrix1;
	mat4 rotateYMatrix1;
	mat4 rotateZMatrix1;

	translateMatrix1 = mat4::identity();
	scaleMatrix1 = mat4::identity();
	rotateXMatrix1 = mat4::identity();
	rotateYMatrix1 = mat4::identity();
	rotateZMatrix1 = mat4::identity();
	modelMatrix1 = mat4::identity();
	viewMatrix1 = mat4::identity();
	projectionMatrix1 = mat4::identity();

	Push(pStack, modelMatrix1);  //main push()

	//MERCURY

	if (isMercuryClicked == true)
	{
		translateMatrix1 = translate(0.0f, 0.0f, 2.5f);    //to revolve in ellipse
		scaleMatrix1 = scale(0.5f, 0.5f, 0.5f);
		rotateXMatrix1 = rotate(90.0f, 1.0f, 0.0f, 0.0f);             //axis change
		modelMatrix1 = modelMatrix1 * translateMatrix1 * scaleMatrix1 * rotateXMatrix1;

		Push(pStack, modelMatrix1);

		rotateZMatrix1 = rotate(mercuryRotAngle, 0.0f, 0.0f, 1.0f);
		modelMatrix1 = modelMatrix1 * rotateZMatrix1;
		viewMatrix1 = lookat(vec3(0.0f, 0.0f, 4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

		projectionMatrix1 = perspectiveProjectionMatrix;  //pre-multiplication of matrices
		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix1);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix1);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mercury_texture);
		glUniform1i(textureSamplerUniformS, 0);

		drawSphere();

		modelMatrix1 = Pop(pStack);
		modelMatrix1 = Pop(pStack);
		
		
	}

	if (isVenusClicked == true)
	{
		translateMatrix1 = translate(0.0f, 0.0f, 2.5f);    //to revolve in ellipse
		scaleMatrix1 = scale(0.5f, 0.5f, 0.5f);
		rotateXMatrix1 = rotate(90.0f, 1.0f, 0.0f, 0.0f);             //axis change
		modelMatrix1 = modelMatrix1 * translateMatrix1 * scaleMatrix1 * rotateXMatrix1;

		Push(pStack, modelMatrix1);

		rotateZMatrix1 = rotate(venusRotAngle, 0.0f, 0.0f, 1.0f);
		modelMatrix1 = modelMatrix1 * rotateZMatrix1;
		viewMatrix1 = lookat(vec3(0.0f, 0.0f, 4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		projectionMatrix1 = perspectiveProjectionMatrix;
		
		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix1);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix1);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, venus_texture);
		glUniform1i(textureSamplerUniformS, 0);

		drawSphere();

		glBindTexture(GL_TEXTURE_2D, 0);

		modelMatrix1 = Pop(pStack);
		modelMatrix1 = Pop(pStack);
	}

	if (isEarthClicked == true)
	{
		translateMatrix1 = translate(0.0f, 0.0f, 2.5f);    //to revolve in ellipse
		scaleMatrix1 = scale(0.5f, 0.5f, 0.5f);
		rotateXMatrix1 = rotate(90.0f, 1.0f, 0.0f, 0.0f);             //axis change
		modelMatrix1 = modelMatrix1 * translateMatrix1 * scaleMatrix1 * rotateXMatrix1;

		Push(pStack, modelMatrix1);

		rotateZMatrix1 = rotate(earthRotAngle, 0.0f, 0.0f, 1.0f);
		modelMatrix1 = modelMatrix1 * rotateZMatrix1;
		viewMatrix1 = lookat(vec3(0.0f, 0.0f, 4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		projectionMatrix1 = perspectiveProjectionMatrix;
		
		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix1);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix1);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, earth_texture);
		glUniform1i(textureSamplerUniformS, 0);

		drawSphere();

		glBindTexture(GL_TEXTURE_2D, 0);

		//moon
		glUniform1i(passUniformS, 0);     //pass 2 (scene lighting)
		rotateZMatrix1 = rotate(earthMoonRevAngle, 0.0f, 0.0f, 1.0f);
		translateMatrix1 = translate(0.8f, 0.0f, 0.0f);
		scaleMatrix1 = scale(0.3f, 0.3f, 0.3f);

		modelMatrix1 = modelMatrix1 * rotateZMatrix1 * translateMatrix1 * scaleMatrix1;

		rotateZMatrix1 = rotate(earthMoonRotAngle, 0.0f, 0.0f, 1.0f);
		modelMatrix1 = modelMatrix1 * rotateZMatrix1;
		viewMatrix1 = lookat(vec3(0.0f, 0.0f, 4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		projectionMatrix1 = perspectiveProjectionMatrix; 
		
		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix1);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix1);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, earthMoon_texture);
		glUniform1i(textureSamplerUniformS, 0);

		drawSphere();

		glBindTexture(GL_TEXTURE_2D, 0);

		modelMatrix1 = Pop(pStack);
		modelMatrix1 = Pop(pStack);
		modelMatrix1 = Pop(pStack);

	}
	
	if (isMarsClicked == true)
	{
		translateMatrix1 = translate(0.0f, 0.0f, 2.5f);    //to revolve in ellipse
		scaleMatrix1 = scale(0.5f, 0.5f, 0.5f);
		rotateXMatrix1 = rotate(90.0f, 1.0f, 0.0f, 0.0f);             //axis change
		modelMatrix1 = modelMatrix1 * translateMatrix1 * scaleMatrix1 * rotateXMatrix1;

		Push(pStack, modelMatrix1);

		rotateZMatrix1 = rotate(marsRotAngle, 0.0f, 0.0f, 1.0f);
		modelMatrix1 = modelMatrix1 * rotateZMatrix1;
		viewMatrix1 = lookat(vec3(0.0f, 0.0f, 4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		projectionMatrix1 = perspectiveProjectionMatrix;

		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix1);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix1);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mars_texture);
		glUniform1i(textureSamplerUniformS, 0);

		drawSphere();

		glBindTexture(GL_TEXTURE_2D, 0);


		//moon
		glUniform1i(passUniformS, 0);     //pass 2 (scene lighting)
		rotateZMatrix1 = rotate(marsMoonRevAngle, 0.0f, 0.0f, 1.0f);
		translateMatrix1 = translate(0.8f, 0.0f, 0.0f);
		scaleMatrix1 = scale(0.3f, 0.3f, 0.3f);

		modelMatrix1 = modelMatrix1 * rotateZMatrix1 * translateMatrix1 * scaleMatrix1;

		rotateZMatrix1 = rotate(marsMoonRotAngle, 0.0f, 0.0f, 1.0f);
		modelMatrix1 = modelMatrix1 * rotateZMatrix1;
		viewMatrix1 = lookat(vec3(0.0f, 0.0f, 4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		projectionMatrix1 = perspectiveProjectionMatrix;

		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix1);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix1);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, marsMoon_texture);
		glUniform1i(textureSamplerUniformS, 0);

		drawSphere();

		glBindTexture(GL_TEXTURE_2D, 0);

		modelMatrix1 = Pop(pStack);
		modelMatrix1 = Pop(pStack);
		modelMatrix1 = Pop(pStack);

	}

	if (isJupiterClicked == true)
	{
		translateMatrix1 = translate(0.0f, 0.0f, 2.5f);    //to revolve in ellipse
		scaleMatrix1 = scale(0.5f, 0.5f, 0.5f);
		rotateXMatrix1 = rotate(90.0f, 1.0f, 0.0f, 0.0f);             //axis change
		modelMatrix1 = modelMatrix1 * translateMatrix1 * scaleMatrix1 * rotateXMatrix1;

		Push(pStack, modelMatrix1);

		rotateZMatrix1 = rotate(jupiterRotAngle, 0.0f, 0.0f, 1.0f);
		modelMatrix1 = modelMatrix1 * rotateZMatrix1;
		viewMatrix1 = lookat(vec3(0.0f, 0.0f, 4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		projectionMatrix1 = perspectiveProjectionMatrix;

		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix1);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix1);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, jupiter_texture);
		glUniform1i(textureSamplerUniformS, 0);

		drawSphere();

		glBindTexture(GL_TEXTURE_2D, 0);


		//moon
		glUniform1i(passUniformS, 0);     //pass 2 (scene lighting)
		rotateZMatrix1 = rotate(jupiterMoonRevAngle, 0.0f, 0.0f, 1.0f);
		translateMatrix1 = translate(0.8f, 0.0f, 0.0f);
		scaleMatrix1 = scale(0.3f, 0.3f, 0.3f);

		modelMatrix1 = modelMatrix1 * rotateZMatrix1 * translateMatrix1 * scaleMatrix1;

		rotateZMatrix1 = rotate(jupiterMoonRotAngle, 0.0f, 0.0f, 1.0f);
		modelMatrix1 = modelMatrix1 * rotateZMatrix1;
		viewMatrix1 = lookat(vec3(0.0f, 0.0f, 4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		projectionMatrix1 = perspectiveProjectionMatrix;

		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix1);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix1);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, jupiterMoon_texture);
		glUniform1i(textureSamplerUniformS, 0);

		drawSphere();

		glBindTexture(GL_TEXTURE_2D, 0);

		modelMatrix1 = Pop(pStack);
		modelMatrix1 = Pop(pStack);
		modelMatrix1 = Pop(pStack);

	}

	if (isSaturnClicked == true)
	{
		/*focusedSaturnXPos = saturnXPos;
		focusedSaturnYPos = saturnYPos;
		focusedSaturnZPos = saturnZPos;*/

		translateMatrix1 = translate(0.0f, 0.0f, 2.5f);    //to revolve in ellipse
		scaleMatrix1 = scale(0.5f, 0.5f, 0.5f);
		rotateZMatrix1 = rotate(-10.0f, 0.0f, 0.0f, 1.0f);             //axis change
		rotateXMatrix1 = rotate(115.0f, 1.0f, 0.0f, 0.0f);             //axis change
		modelMatrix1 = modelMatrix1 * translateMatrix1 * scaleMatrix1 * rotateZMatrix1 * rotateXMatrix1;

		Push(pStack, modelMatrix1);

		rotateZMatrix1 = rotate(saturnRotAngle, 0.0f, 0.0f, 1.0f);
		modelMatrix1 = modelMatrix1 * rotateZMatrix1;
		viewMatrix1 = lookat(vec3(0.0f, 0.0f, 4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		projectionMatrix1 = perspectiveProjectionMatrix;

		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix1);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix1);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, saturn_texture);
		glUniform1i(textureSamplerUniformS, 0);

		drawSphere();

		glBindTexture(GL_TEXTURE_2D, 0);

		Push(pStack, modelMatrix1);

		//ring
		scaleMatrix1 = scale(1.0f, 1.0f, 0.1f);
		modelMatrix1 = modelMatrix1 * scaleMatrix1;


		projectionMatrix1 = perspectiveProjectionMatrix;
		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix1);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix1);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, saturn_ring_texture);
		glUniform1i(textureSamplerUniformS, 0);

		drawTorus();

		glBindTexture(GL_TEXTURE_2D, 0);

		modelMatrix1 = Pop(pStack);

		//moon
		glUniform1i(passUniformS, 0);     //pass 2 (scene lighting)
		rotateZMatrix1 = rotate(saturnMoonRevAngle, 0.0f, 0.0f, 1.0f);
		translateMatrix1 = translate(0.7f, 0.0f, -0.15f);
		scaleMatrix1 = scale(0.25f, 0.25f, 0.25f);

		modelMatrix1 = modelMatrix1 * rotateZMatrix1 * translateMatrix1 * scaleMatrix1;

		rotateZMatrix1 = rotate(saturnMoonRotAngle, 0.0f, 0.0f, 1.0f);
		modelMatrix1 = modelMatrix1 * rotateZMatrix1;
		viewMatrix1 = lookat(vec3(0.0f, 0.0f, 4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		projectionMatrix1 = perspectiveProjectionMatrix;

		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix1);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix1);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, saturnMoon_texture);
		glUniform1i(textureSamplerUniformS, 0);

		drawSphere();

		glBindTexture(GL_TEXTURE_2D, 0);

		modelMatrix1 = Pop(pStack);
		modelMatrix1 = Pop(pStack);
		modelMatrix1 = Pop(pStack);

	}

	if (isUranusClicked == true)
	{
		translateMatrix1 = translate(0.0f, 0.0f, 2.5f);    //to revolve in ellipse
		scaleMatrix1 = scale(0.5f, 0.5f, 0.5f);
		rotateXMatrix1 = rotate(90.0f, 1.0f, 0.0f, 0.0f);             //axis change
		modelMatrix1 = modelMatrix1 * translateMatrix1 * scaleMatrix1 * rotateXMatrix1;

		Push(pStack, modelMatrix1);

		rotateZMatrix1 = rotate(uranusRotAngle, 0.0f, 0.0f, 1.0f);
		modelMatrix1 = modelMatrix1 * rotateZMatrix1;
		viewMatrix1 = lookat(vec3(0.0f, 0.0f, 4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		projectionMatrix1 = perspectiveProjectionMatrix;

		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix1);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix1);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, uranus_texture);
		glUniform1i(textureSamplerUniformS, 0);

		drawSphere();

		glBindTexture(GL_TEXTURE_2D, 0);


		//moon
		glUniform1i(passUniformS, 0);     //pass 2 (scene lighting)
		rotateZMatrix1 = rotate(uranusMoonRevAngle, 0.0f, 0.0f, 1.0f);
		translateMatrix1 = translate(0.8f, 0.0f, 0.0f);
		scaleMatrix1 = scale(0.3f, 0.3f, 0.3f);

		modelMatrix1 = modelMatrix1 * rotateZMatrix1 * translateMatrix1 * scaleMatrix1;

		rotateZMatrix1 = rotate(uranusMoonRotAngle, 0.0f, 0.0f, 1.0f);
		modelMatrix1 = modelMatrix1 * rotateZMatrix1;
		viewMatrix1 = lookat(vec3(0.0f, 0.0f, 4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		projectionMatrix1 = perspectiveProjectionMatrix;

		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix1);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix1);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, uranusMoon_texture);
		glUniform1i(textureSamplerUniformS, 0);

		drawSphere();

		glBindTexture(GL_TEXTURE_2D, 0);

		modelMatrix1 = Pop(pStack);
		modelMatrix1 = Pop(pStack);
		modelMatrix1 = Pop(pStack);

	}

	if (isNeptuneClicked == true)
	{
		translateMatrix1 = translate(0.0f, 0.0f, 2.5f);    //to revolve in ellipse
		scaleMatrix1 = scale(0.5f, 0.5f, 0.5f);
		rotateXMatrix1 = rotate(90.0f, 1.0f, 0.0f, 0.0f);             //axis change
		modelMatrix1 = modelMatrix1 * translateMatrix1 * scaleMatrix1 * rotateXMatrix1;

		Push(pStack, modelMatrix1);

		rotateZMatrix1 = rotate(neptuneRotAngle, 0.0f, 0.0f, 1.0f);
		modelMatrix1 = modelMatrix1 * rotateZMatrix1;
		viewMatrix1 = lookat(vec3(0.0f, 0.0f, 4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		projectionMatrix1 = perspectiveProjectionMatrix;

		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix1);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix1);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, neptune_texture);
		glUniform1i(textureSamplerUniformS, 0);

		drawSphere();

		glBindTexture(GL_TEXTURE_2D, 0);


		//moon
		glUniform1i(passUniformS, 0);     //pass 2 (scene lighting)
		rotateZMatrix1 = rotate(neptuneMoonRevAngle, 0.0f, 0.0f, 1.0f);
		translateMatrix1 = translate(0.8f, 0.0f, 0.0f);
		scaleMatrix1 = scale(0.3f, 0.3f, 0.3f);

		modelMatrix1 = modelMatrix1 * rotateZMatrix1 * translateMatrix1 * scaleMatrix1;

		rotateZMatrix1 = rotate(neptuneMoonRotAngle, 0.0f, 0.0f, 1.0f);
		modelMatrix1 = modelMatrix1 * rotateZMatrix1;
		viewMatrix1 = lookat(vec3(0.0f, 0.0f, 4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		projectionMatrix1 = perspectiveProjectionMatrix;

		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix1);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix1);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, neptuneMoon_texture);
		glUniform1i(textureSamplerUniformS, 0);

		drawSphere();

		glBindTexture(GL_TEXTURE_2D, 0);

		modelMatrix1 = Pop(pStack);
		modelMatrix1 = Pop(pStack);
		modelMatrix1 = Pop(pStack);

	}

	if (isPlutoClicked == true)
	{
		translateMatrix1 = translate(0.0f, 0.0f, 2.5f);    //to revolve in ellipse
		scaleMatrix1 = scale(0.5f, 0.5f, 0.5f);
		rotateXMatrix1 = rotate(90.0f, 1.0f, 0.0f, 0.0f);             //axis change
		modelMatrix1 = modelMatrix1 * translateMatrix1 * scaleMatrix1 * rotateXMatrix1;

		Push(pStack, modelMatrix1);

		rotateZMatrix1 = rotate(plutoRotAngle, 0.0f, 0.0f, 1.0f);
		modelMatrix1 = modelMatrix1 * rotateZMatrix1;
		viewMatrix1 = lookat(vec3(0.0f, 0.0f, 4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		projectionMatrix1 = perspectiveProjectionMatrix;

		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix1);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix1);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, pluto_texture);
		glUniform1i(textureSamplerUniformS, 0);

		drawSphere();

		glBindTexture(GL_TEXTURE_2D, 0);

		modelMatrix1 = Pop(pStack);
		modelMatrix1 = Pop(pStack);
	}

	modelMatrix1 = Pop(pStack);   //main pop()
	Push(pStack, modelMatrix1);   //identity push()
	
	glUniform1i(passUniformS, 1);

	translateMatrix1 = translate(0.0f, 0.0f, 2.0f);
	scaleMatrix1 = scale(0.9f, 0.5f, 0.0f);
	modelMatrix1 = translateMatrix1 * scaleMatrix1;
	projectionMatrix1 = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix1);
	glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix1);
	glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix1);

	glVertexAttrib4f(SPK_ATTRIBUTE_COLOR, 0.3f, 0.3f, 0.3f, 0.6f);
	//bind vao_rectangle
	glBindVertexArray(vao_square);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	//unbind vao_rectangle
	glBindVertexArray(0);
	//glVertexAttrib4f(SPK_ATTRIBUTE_COLOR, 0.0f, 0.0f, 0.0f, 1.0f);

	glVertexAttrib4f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f, 1.0f);
	glLineWidth(5.0f);
	//bind vao_rectangle
	glBindVertexArray(vao_square);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
	//unbind vao_rectangle
	glBindVertexArray(0);
	glLineWidth(1.0f);
	glVertexAttrib4f(SPK_ATTRIBUTE_COLOR, 0.0f, 0.0f, 0.0f, 1.0f);

	//stop using OpenGL program object
	glUseProgram(0);

	if (isMercuryClicked == true)
		RenderText(textShaderProgramObject, "MERCURY", -0.09f, 0.21f, 0.0008f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	if (isVenusClicked == true)
		RenderText(textShaderProgramObject, "VENUS", -0.06f, 0.21f, 0.0008f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	if (isEarthClicked == true)
		RenderText(textShaderProgramObject, "EARTH & MOON", -0.15f, 0.21f, 0.0008f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	if (isMarsClicked == true)
		RenderText(textShaderProgramObject, "MARS & PHOBOS", -0.15f, 0.21f, 0.0008f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	if (isJupiterClicked == true)
		RenderText(textShaderProgramObject, "JUPITER & EUROPA", -0.18f, 0.21f, 0.0008f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	if (isSaturnClicked == true)
		RenderText(textShaderProgramObject, "SATURN & TITAN", -0.15f, 0.21f, 0.0008f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	if (isUranusClicked == true)
		RenderText(textShaderProgramObject, "URANUS & ARIEL", -0.15f, 0.21f, 0.0008f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	if (isNeptuneClicked == true)
		RenderText(textShaderProgramObject, "NEPTUNE & TRITON", -0.18f, 0.21f, 0.0008f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	if (isPlutoClicked == true)
		RenderText(textShaderProgramObject, "PLUTO", -0.06f, 0.21f, 0.0008f, vec4(1.0f, 1.0f, 1.0f, 1.0f));


}

void drawSunGodrays(void)
{
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projectionMatrix;
	mat4 translateMatrix;
	mat4 scaleMatrix;
	mat4 rotateXMatrix;
	mat4 rotateYMatrix;
	mat4 rotateZMatrix;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//pass1
	//light source colored with black scene objects (silhouette pass)

	//bind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_pass1);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, (GLsizei)WIN_WIDTH, (GLsizei)WIN_HEIGHT);
	//glViewport(0, 0, gWidth, gHeight);

	//start using OpenGL program object
	glUseProgram(sceneShaderProgramObject);

	//OpenGL Drawing

	glUniform1i(passUniformS, 1);     //pass 1

	//SUN TRANSFORMATION	
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	rotateXMatrix = mat4::identity();
	rotateYMatrix = mat4::identity();
	rotateZMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();

	translateMatrix = translate(sunPosX, sunPosY, 0.0f);
	scaleMatrix = scale(0.3f, 0.3f, 0.3f);

	modelMatrix = translateMatrix * scaleMatrix;
	viewMatrix = camera;
	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

	glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 0.96f, 0.89f, 0.54f);

	drawSphere();

	//draw scene in black color 


	//stop using OpenGL program object
	glUseProgram(0);

	//reset color
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//unbind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//pass2
	//no light source with colored scene objects

	//bind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_pass2);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, (GLsizei)WIN_WIDTH, (GLsizei)WIN_HEIGHT);

	//draw scene in color


	glBindTexture(GL_TEXTURE_2D, 0);

	//stop using OpenGL program object
	glUseProgram(0);

	//reset color
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//unbind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//pass3
	//apply godrays

	//bind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_godrays);

	glDisable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, (GLsizei)WIN_WIDTH, (GLsizei)WIN_HEIGHT);

	//start using OpenGL program object
	glUseProgram(godraysShaderProgramObject);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	translateMatrix = translate(0.0f, 0.0f, 0.0f);
	modelMatrix = translateMatrix;
	viewMatrix = camera;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, silhouetteTexture_pass1);

	glUniform4f(lightPositionUniformG, sunPosX, sunPosY, 0.0f, 1.0f);
	glUniform1f(densityUniformG, 0.5f);
	glUniform1f(weightUniformG, 0.02f);
	glUniform1f(decayUniformG, 1.0f);
	glUniform1f(exposureUniformG, 1.0f);
	glUniform1i(numSamplesUniformG, 10);

	glUniform1i(occlusionTextureSamplerUniformG, 0);

	glUniformMatrix4fv(modelMatrixUniformG, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniformG, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniformG, 1, GL_FALSE, projectionMatrix);

	//bind vao_rectangle
	glBindVertexArray(vao_square);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	//unbind vao_rectangle
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//stop using OpenGL program object
	glUseProgram(0);

	//unbind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//DEFAULT FBO

	//last pass for godrays
	//blend textures

	//start using OpenGL program object
	glUseProgram(finalPassShaderProgramObject);

	glEnable(GL_DEPTH_TEST);
	//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, (GLsizei)gWidth, (GLsizei)gHeight);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorTexture_pass2);
	glUniform1i(pass1TextureSamplerUniformF, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glUniform1i(pass2TextureSamplerUniformF, 1);

	//bind vao_rectangle
	glBindVertexArray(vao_square);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	//unbind vao_rectangle
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//stop using OpenGL program object
	glUseProgram(0);
}

void drawSkybox(void)
{
	glUseProgram(skyboxShaderProgramObject);

	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projectionMatrix;
	mat4 translateMatrix;
	mat4 scaleMatrix;
	mat4 rotateXMatrix;
	mat4 rotateYMatrix;
	mat4 rotateZMatrix;

	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	rotateXMatrix = mat4::identity();
	rotateYMatrix = mat4::identity();
	rotateZMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();

	camPos = vec3(0.0f, 0.0f, 0.0f);   //z : -3.0
	camTarget = vec3(0.0f, 0.0f, -1.0f);
	camUpAxis = vec3(0.0f, 1.0f, 0.0f);

	viewMatrix = lookat(camPos, camTarget, camUpAxis);
	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);
	glUniform1i(textureSamplerUniform, 0);

	//bind vao_rectangle
	glBindVertexArray(vao_skybox);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	//unbind vao_rectangle
	glBindVertexArray(0);

	//stop using OpenGL program object
	glUseProgram(0);
}

void drawCubeForCubemap(void)
{
	//start using OpenGL program object
	glUseProgram(cubeShaderProgramObject);

	mat4 modelViewProjectionMatrixC = mat4::identity();
	mat4 translateMatrixC = mat4::identity();
	mat4 modelMatrixC = mat4::identity();
	//mat4 viewMatrixC = mat4::identity();
	//
	translateMatrixC = translate(0.0f, 0.0f, 0.0f);
	modelMatrixC = translateMatrixC;

	//camPos = vec3(0.0f, 0.0f, 0.0f);   //z : -3.0
	//camTarget = vec3(0.0f, 0.0f, -1.0f);
	//camUpAxis = vec3(0.0f, 1.0f, 0.0f);

	//viewMatrixC = lookat(camPos, camTarget, camUpAxis);

	//modelViewProjectionMatrixC = perspectiveProjectionMatrix * modelMatrixC * viewMatrixC;
	modelViewProjectionMatrixC = perspectiveProjectionMatrix * modelMatrixC;

	glUniformMatrix4fv(mvpMatrixUniformC, 1, GL_FALSE, modelViewProjectionMatrixC);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, starfieldTextureColorBuffer);
	glUniform1i(textureSamplerUniformC, 0);

	//bind vao_rectangle
	glBindVertexArray(vao_cube);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	//unbind vao_rectangle
	glBindVertexArray(0);

	//stop using OpenGL program object
	glUseProgram(0);
}

void drawStarfield(void)
{
	float t = (float)currentTime;

	t *= 0.0001f;
	t -= floor(t);

	//start using OpenGL program object
	glUseProgram(starfieldShaderProgramObject);

	glUniform1f(timeUniformSF, t);

	//OpenGL Drawing

	mat4 projectionMatrixSF;

	projectionMatrixSF = mat4::identity();
	projectionMatrixSF = perspectiveProjectionMatrix;

	glUniformMatrix4fv(projectionMatrixUniformSF, 1, GL_FALSE, projectionMatrixSF);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, star_texture);
	glUniform1i(textureSamplerUniformSF, 0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	//bind vao_rectangle
	glBindVertexArray(vao_star);
	glEnable(GL_PROGRAM_POINT_SIZE);

	glDrawArrays(GL_POINTS, 0, NUM_STARS);

	glDisable(GL_BLEND);

	//unbind vao_rectangle
	glBindVertexArray(0);

	//stop using OpenGL program object
	glUseProgram(0);

}

void renderTextMercury(void)
{
	RenderText(textShaderProgramObject, "MERCURY", -0.1f, 0.35f, 0.00055f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "It is the smallest planet", -0.1f, 0.32f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "in our solar system", -0.1f, 0.3f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "and closest to the Sun.", -0.1f, 0.28f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "It is the fastest planet,", -0.1f, 0.26f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "zipping around the Sun.", -0.1f, 0.24f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Distance(km)", -0.1f, 0.18f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "57,909,227", -0.1f, 0.16f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Equatorial Radius(km)", -0.1f, 0.12f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "2,439.7", -0.1f, 0.1f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Volume(km  )", -0.1f, 0.06f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "3", -0.011f, 0.065f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript Km^3
	RenderText(textShaderProgramObject, "60,827,208,742", -0.1f, 0.04f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Density(g/cm  )", -0.1f, 0.0f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "3", 0.004f, 0.005f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript cm^3
	RenderText(textShaderProgramObject, "5.427", -0.1f, -0.02f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Surface Gravity(m/s  )", -0.1f, -0.06f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "2", 0.055f, -0.055f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript s^2
	RenderText(textShaderProgramObject, "3.7", -0.1f, -0.08f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Rotation Period(Earth Days)", -0.1f, -0.12f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "58.646", -0.1f, -0.14f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Period(Earth Years)", -0.1f, -0.18f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "0.2408467", -0.1f, -0.2f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Inclination(deg)", -0.1f, -0.24f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "7.0", -0.1f, -0.26f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Equatorial Inclination(deg)", -0.1f, -0.3f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "0", -0.1f, -0.32f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

}

void renderTextVenus(void)
{
	RenderText(textShaderProgramObject, "VENUS", -0.1f, 0.35f, 0.00055f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "It is the smallest planet.", -0.1f, 0.32f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "It spins slowly in the opposite", -0.1f, 0.3f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "direction from most planets.", -0.1f, 0.28f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "It is the hottest planet", -0.1f, 0.26f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "in our solar system.", -0.1f, 0.24f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Distance(km)", -0.1f, 0.18f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "108,209,475", -0.1f, 0.16f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Equatorial Radius(km)", -0.1f, 0.12f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "6,051.8", -0.1f, 0.1f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Volume(km  )", -0.1f, 0.06f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "3", -0.011f, 0.065f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript Km^3
	RenderText(textShaderProgramObject, "928,415,345,893", -0.1f, 0.04f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Density(g/cm  )", -0.1f, 0.0f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "3", 0.004f, 0.005f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript cm^3
	RenderText(textShaderProgramObject, "5.243", -0.1f, -0.02f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Surface Gravity(m/s  )", -0.1f, -0.06f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "2", 0.055f, -0.055f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript s^2
	RenderText(textShaderProgramObject, "8.87", -0.1f, -0.08f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Rotation Period(Earth Days)", -0.1f, -0.12f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "-243.018", -0.1f, -0.14f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Period(Earth Years)", -0.1f, -0.18f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "0.61519726", -0.1f, -0.2f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Inclination(deg)", -0.1f, -0.24f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "3.39", -0.1f, -0.26f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Equatorial Inclination(deg)", -0.1f, -0.3f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "177.3", -0.1f, -0.32f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

}

void renderTextEarth(void)
{
	RenderText(textShaderProgramObject, "EARTH", -0.1f, 0.35f, 0.00055f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "It is the only place we know", -0.1f, 0.32f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "that's inhabited by living things.", -0.1f, 0.3f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "It's also the only planet", -0.1f, 0.28f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "in our solar system with", -0.1f, 0.26f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "liquid water on the surface.", -0.1f, 0.24f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Distance(km)", -0.1f, 0.18f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "149,598,262", -0.1f, 0.16f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Equatorial Radius(km)", -0.1f, 0.12f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "6,371.00", -0.1f, 0.1f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Volume(km  )", -0.1f, 0.06f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "3", -0.011f, 0.065f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript Km^3
	RenderText(textShaderProgramObject, "1,083,206,916,846", -0.1f, 0.04f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Density(g/cm  )", -0.1f, 0.0f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "3", 0.004f, 0.005f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript cm^3
	RenderText(textShaderProgramObject, "5.513", -0.1f, -0.02f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Surface Gravity(m/s  )", -0.1f, -0.06f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "2", 0.055f, -0.055f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript s^2
	RenderText(textShaderProgramObject, "9.80665", -0.1f, -0.08f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Rotation Period(Earth Days)", -0.1f, -0.12f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "0.99726968", -0.1f, -0.14f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Period(Earth Years)", -0.1f, -0.18f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "1.0000174", -0.1f, -0.2f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Inclination(deg)", -0.1f, -0.24f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "0.00005", -0.1f, -0.26f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Equatorial Inclination(deg)", -0.1f, -0.3f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "23.4393", -0.1f, -0.32f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

}

void renderTextMars(void)
{
	RenderText(textShaderProgramObject, "MARS", -0.1f, 0.35f, 0.00055f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "It is a dusty, cold, desert world", -0.1f, 0.32f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "with a very thin atmosphere.", -0.1f, 0.3f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "There is an evidence that it was", -0.1f, 0.28f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "billions of years ago, wetter &", -0.1f, 0.26f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "warmer, with thick atmosphere.", -0.1f, 0.24f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Distance(km)", -0.1f, 0.18f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "227,943,824", -0.1f, 0.16f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Equatorial Radius(km)", -0.1f, 0.12f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "3,389.5", -0.1f, 0.1f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Volume(km  )", -0.1f, 0.06f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "3", -0.011f, 0.065f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript Km^3
	RenderText(textShaderProgramObject, "163,115,609,799", -0.1f, 0.04f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Density(g/cm  )", -0.1f, 0.0f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "3", 0.004f, 0.005f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript cm^3
	RenderText(textShaderProgramObject, "3.934", -0.1f, -0.02f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Surface Gravity(m/s  )", -0.1f, -0.06f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "2", 0.055f, -0.055f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript s^2
	RenderText(textShaderProgramObject, "3.71", -0.1f, -0.08f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Rotation Period(Earth Days)", -0.1f, -0.12f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "1.026", -0.1f, -0.14f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Period(Earth Years)", -0.1f, -0.18f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "1.8808476", -0.1f, -0.2f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Inclination(deg)", -0.1f, -0.24f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "1.85", -0.1f, -0.26f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Equatorial Inclination(deg)", -0.1f, -0.3f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "25.2", -0.1f, -0.32f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

}

void renderTextJupiter(void)
{
	RenderText(textShaderProgramObject, "JUPITER", -0.1f, 0.35f, 0.00055f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "It is more than twice as massive", -0.1f, 0.32f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "than the other planets of our", -0.1f, 0.3f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "solar system combined. The", -0.1f, 0.28f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "giant planet's Great Red spot is", -0.1f, 0.26f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "an old storm bigger than Earth.", -0.1f, 0.24f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Distance(km)", -0.1f, 0.18f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "778,340,821", -0.1f, 0.16f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Equatorial Radius(km)", -0.1f, 0.12f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "69,911", -0.1f, 0.1f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Volume(km  )", -0.1f, 0.06f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "3", -0.011f, 0.065f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript Km^3
	RenderText(textShaderProgramObject, "1,431,281,810,739,360", -0.1f, 0.04f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Density(g/cm  )", -0.1f, 0.0f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "3", 0.004f, 0.005f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript cm^3
	RenderText(textShaderProgramObject, "1.326", -0.1f, -0.02f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Surface Gravity(m/s  )", -0.1f, -0.06f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "2", 0.055f, -0.055f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript s^2
	RenderText(textShaderProgramObject, "24.79", -0.1f, -0.08f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Rotation Period(Earth Days)", -0.1f, -0.12f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "0.41354", -0.1f, -0.14f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Period(Earth Years)", -0.1f, -0.18f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "11.862615", -0.1f, -0.2f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Inclination(deg)", -0.1f, -0.24f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "1.304", -0.1f, -0.26f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Equatorial Inclination(deg)", -0.1f, -0.3f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "3.1", -0.1f, -0.32f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

}

void renderTextSaturn(void)
{
	RenderText(textShaderProgramObject, "SATURN", -0.1f, 0.35f, 0.00055f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "Adorned with a dazzling,", -0.1f, 0.32f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "complex system of icy rings,", -0.1f, 0.3f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "it is unique in our solar system.", -0.1f, 0.28f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "Some planets have rings, but", -0.1f, 0.26f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "not as spectacular as Saturn's.", -0.1f, 0.24f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Distance(km)", -0.1f, 0.18f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "1,426,666,422", -0.1f, 0.16f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Equatorial Radius(km)", -0.1f, 0.12f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "58,232", -0.1f, 0.1f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Volume(km  )", -0.1f, 0.06f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "3", -0.011f, 0.065f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript Km^3
	RenderText(textShaderProgramObject, "827,129,915,150,897", -0.1f, 0.04f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Density(g/cm  )", -0.1f, 0.0f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "3", 0.004f, 0.005f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript cm^3
	RenderText(textShaderProgramObject, "0.687", -0.1f, -0.02f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Surface Gravity(m/s  )", -0.1f, -0.06f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "2", 0.055f, -0.055f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript s^2
	RenderText(textShaderProgramObject, "10.4*", -0.1f, -0.08f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Rotation Period(Earth Days)", -0.1f, -0.12f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "0.444", -0.1f, -0.14f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Period(Earth Years)", -0.1f, -0.18f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "29.447498", -0.1f, -0.2f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Inclination(deg)", -0.1f, -0.24f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "2.49", -0.1f, -0.26f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Equatorial Inclination(deg)", -0.1f, -0.3f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "26.7", -0.1f, -0.32f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

}

void renderTextUranus(void)
{
	RenderText(textShaderProgramObject, "URANUS", -0.1f, 0.35f, 0.00055f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "Seventh planet from the Sun,", -0.1f, 0.32f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "rotates at a nearly 90-deg angle", -0.1f, 0.3f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "from the plane of it's orbit.", -0.1f, 0.28f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "It's unique tilt makes Uranus", -0.1f, 0.26f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "appear to spin on it's side.", -0.1f, 0.24f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Distance(km)", -0.1f, 0.18f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "2,870,658,186", -0.1f, 0.16f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Equatorial Radius(km)", -0.1f, 0.12f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "25,362", -0.1f, 0.1f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Volume(km  )", -0.1f, 0.06f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "3", -0.011f, 0.065f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript Km^3
	RenderText(textShaderProgramObject, "68,334,355,695,584", -0.1f, 0.04f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Density(g/cm  )", -0.1f, 0.0f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "3", 0.004f, 0.005f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript cm^3
	RenderText(textShaderProgramObject, "1.270", -0.1f, -0.02f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Surface Gravity(m/s  )", -0.1f, -0.06f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "2", 0.055f, -0.055f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript s^2
	RenderText(textShaderProgramObject, "8.87", -0.1f, -0.08f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Rotation Period(Earth Days)", -0.1f, -0.12f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "-0.718", -0.1f, -0.14f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Period(Earth Years)", -0.1f, -0.18f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "84.016846", -0.1f, -0.2f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Inclination(deg)", -0.1f, -0.24f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "0.77", -0.1f, -0.26f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Equatorial Inclination(deg)", -0.1f, -0.3f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "97.8", -0.1f, -0.32f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

}

void renderTextNeptune(void)
{
	RenderText(textShaderProgramObject, "NEPTUNE", -0.1f, 0.35f, 0.00055f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "It is dark, cold and whipped", -0.1f, 0.32f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "by supersonic winds. It was", -0.1f, 0.3f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "the first planet located through", -0.1f, 0.28f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "mathematical calculations,", -0.1f, 0.26f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "rather than by telescope.", -0.1f, 0.24f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Distance(km)", -0.1f, 0.18f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "57,909,227", -0.1f, 0.16f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Equatorial Radius(km)", -0.1f, 0.12f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "2,439.7", -0.1f, 0.1f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Volume(km  )", -0.1f, 0.06f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "3", -0.011f, 0.065f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript Km^3
	RenderText(textShaderProgramObject, "60,827,208,742", -0.1f, 0.04f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Density(g/cm  )", -0.1f, 0.0f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "3", 0.004f, 0.005f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript cm^3
	RenderText(textShaderProgramObject, "5.427", -0.1f, -0.02f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Surface Gravity(m/s  )", -0.1f, -0.06f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "2", 0.055f, -0.055f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript s^2
	RenderText(textShaderProgramObject, "3.7", -0.1f, -0.08f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Rotation Period(Earth Days)", -0.1f, -0.12f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "58.646", -0.1f, -0.14f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Period(Earth Years)", -0.1f, -0.18f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "0.2408467", -0.1f, -0.2f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Inclination(deg)", -0.1f, -0.24f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "7.0", -0.1f, -0.26f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Equatorial Inclination(deg)", -0.1f, -0.3f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "0", -0.1f, -0.32f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

}

void renderTextPluto(void)
{
	RenderText(textShaderProgramObject, "PLUTO", -0.1f, 0.35f, 0.00055f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "It is a complex world of", -0.1f, 0.32f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "ice mountains and frozen plains.", -0.1f, 0.3f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "Once considered the 9th planet", -0.1f, 0.28f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "and is best known of a new class", -0.1f, 0.26f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "of worlds called dwarf planets.", -0.1f, 0.24f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Distance(km)", -0.1f, 0.18f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "5,906,440,628", -0.1f, 0.16f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Equatorial Radius(km)", -0.1f, 0.12f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "1,151", -0.1f, 0.1f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Volume(km  )", -0.1f, 0.06f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "3", -0.011f, 0.065f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript Km^3
	RenderText(textShaderProgramObject, "6,387,259,783", -0.1f, 0.04f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Density(g/cm  )", -0.1f, 0.0f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "3", 0.004f, 0.005f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript cm^3
	RenderText(textShaderProgramObject, "2.050", -0.1f, -0.02f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Surface Gravity(m/s  )", -0.1f, -0.06f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "2", 0.055f, -0.055f, 0.0002f, vec4(1.0f, 1.0f, 1.0f, 1.0f));   //superscript s^2
	RenderText(textShaderProgramObject, "0.66", -0.1f, -0.08f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Rotation Period(Earth Days)", -0.1f, -0.12f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "6.375", -0.1f, -0.14f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Period(Earth Years)", -0.1f, -0.18f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "248", -0.1f, -0.2f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Orbit Inclination(deg)", -0.1f, -0.24f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "57", -0.1f, -0.26f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderText(textShaderProgramObject, "Equatorial Inclination(deg)", -0.1f, -0.3f, 0.00035f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderText(textShaderProgramObject, "122.5", -0.1f, -0.32f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

}


