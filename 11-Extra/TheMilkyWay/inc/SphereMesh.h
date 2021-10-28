#include<math.h>
#include "GlobalVariableDeclarations.h"
#pragma once

//const double PI = 3.14159265358979323846;

//global variables
short *elements;
GLfloat *verts;
GLfloat *norms;
GLfloat *texCoords;

GLuint numElements = 0;
GLuint maxElements = 0;
GLuint numVertices = 0;

GLuint vbo_position = 0;
GLuint vbo_normal = 0;
GLuint vbo_texture = 0;
GLuint vbo_index = 0;
GLuint vao = 0;

//function prototypes
void allocate(GLuint);
void addTriangle(GLfloat**, GLfloat**, GLfloat**);
void prepareToDraw(void);
void drawSphere(void);
GLuint getIndexCount(void);
GLuint getVertexCount(void);
void normalizeVector(GLfloat[]);
bool isFoundIdentical(const float, const float, const float);
void cleanupMeshData(void);
void deallocate(void);


void allocate(GLuint numIndices)
{
	//code
	cleanupMeshData();

	maxElements = numIndices;
	numElements = 0;
	numVertices = 0;

	int iNumIndices = numIndices / 3;

	verts = (GLfloat*)malloc(sizeof(GLfloat) * (iNumIndices * 3));
	norms = (GLfloat*)malloc(sizeof(GLfloat) * (iNumIndices * 3));
	texCoords = (GLfloat*)malloc(sizeof(GLfloat) * (iNumIndices * 2));
	elements = (short*)malloc(sizeof(short) * (iNumIndices * 3));

}

void addTriangle(GLfloat** single_vertex, GLfloat** single_normal, GLfloat** single_texture)
{
	//code
	const float diff = 0.00001;

	int i, j;
	
	normalizeVector(single_normal[0]);
	normalizeVector(single_normal[1]);
	normalizeVector(single_normal[2]);

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < numVertices; j++)
		{
			if (isFoundIdentical(verts[j * 3], single_vertex[i][0], diff) &&
				isFoundIdentical(verts[(j * 3) + 1], single_vertex[i][1], diff) &&
				isFoundIdentical(verts[(j * 3) + 2], single_vertex[i][2], diff) &&

				isFoundIdentical(norms[j * 3], single_normal[i][0], diff) &&
				isFoundIdentical(norms[(j * 3) + 1], single_normal[i][1], diff) &&
				isFoundIdentical(norms[(j * 3) + 2], single_normal[i][2], diff) &&

				isFoundIdentical(texCoords[j * 2], single_texture[i][0], diff) &&
				isFoundIdentical(texCoords[(j * 2) + 1], single_texture[i][1], diff))
			{
				elements[numElements] = j;
				numElements++;
				break;
			}

		}

		if (j == numVertices && numVertices < maxElements && numElements < maxElements)
		{
			verts[numVertices * 3] = single_vertex[i][0];
			verts[(numVertices * 3) + 1] = single_vertex[i][1];
			verts[(numVertices * 3) + 2] = single_vertex[i][2];

			norms[numVertices * 3] = single_normal[i][0];
			norms[(numVertices * 3) + 1] = single_normal[i][1];
			norms[(numVertices * 3) + 2] = single_normal[i][2];

			texCoords[numVertices * 2] = single_texture[i][0];
			texCoords[(numVertices * 2) + 1] = single_texture[i][1];

			elements[numElements] = numVertices; 
			numElements++; 
			numVertices++;
		}
					
	}

}

void prepareToDraw(void)
{
	//code
	//VAO

	/*for (int i = 0; i < numVertices; i++)
	{
		fprintf(gpFile, "verts = %f %f %f\n", verts[i * 3 + 0], verts[i * 3 + 1], verts[i * 3 + 2]);
	}

	for (int i = 0; i < numElements; i++)
	{
		fprintf(gpFile, "elements = %d\n", elements[i]);
	}*/

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * numVertices, verts, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &vbo_normal);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * numVertices, norms, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_texture);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * numVertices, texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_TEXCOORD);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &vbo_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * numElements, elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



	glBindVertexArray(0);

	cleanupMeshData();

}

void drawSphere(void)
{
	//code
	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);
}

GLuint getIndexCount(void)
{
	//code
	return(numElements);
}

GLuint getVertexCount(void)
{
	//code
	return(numVertices);
}

void normalizeVector(GLfloat v[])
{
	//code
	GLfloat squaredVectorLength = (v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]);

	GLfloat squareRootOfSquaredVectorLength = sqrt(squaredVectorLength);

	v[0] = v[0] * 1.0 / squareRootOfSquaredVectorLength;
	v[1] = v[1] * 1.0 / squareRootOfSquaredVectorLength;
	v[2] = v[2] * 1.0 / squareRootOfSquaredVectorLength;

}

bool isFoundIdentical(const float val1, const float val2, const float diff)
{
	//code
	float temp = fabs(val1 - val2);
	if (temp < diff)
		return(true);
	else
		return(false);


}

void cleanupMeshData(void)
{
	//code
	if (elements != NULL)
	{
		free(elements);
		elements = NULL;
	}

	if (verts != NULL)
	{
		free(verts);
		verts = NULL;
	}

	if (norms != NULL)
	{
		free(norms);
		norms = NULL;
	}

	if (texCoords != NULL)
	{
		free(texCoords);
		texCoords = NULL;
	}
}

void deallocate(void)
{
	//code
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	if (vbo_position)
	{
		glDeleteBuffers(1, &vbo_position);
		vbo_position = 0;
	}

	if (vbo_normal)
	{
		glDeleteBuffers(1, &vbo_normal);
		vbo_normal = 0;
	}

	if (vbo_texture)
	{
		glDeleteBuffers(1, &vbo_texture);
		vbo_texture = 0;
	}

	if (vbo_index)
	{
		glDeleteBuffers(1, &vbo_index);
		vbo_index = 0;
	}

}



//sphere code

void makeSphere(GLfloat fRadius, GLuint iSlices, GLuint iStacks)
{
	//code

	float pi = (float)PI;

	//variables
	float drho = pi / iStacks;
	float dtheta = 2.0 * (float)PI / (float)iSlices;
	float ds = 1.0 / (float)iSlices;
	float dt = 1.0 / (float)iStacks;
	float t = 1.0;
	float s = 0.0;

	allocate(iSlices * iStacks * 6);

	for (int i = 0; i < iStacks; i++)
	{
		float rho = (float)i * drho;
		float srho = (float)sin(rho);
		float crho = (float)cos(rho);
		float srhodrho = (float)sin(rho + drho);
		float crhodrho = (float)cos(rho + drho);

		s = 0.0;

		/*REF:
		GLfloat vertex[4][3];
		GLfloat normal[4][3];
		GLfloat texture[4][2];*/

		GLfloat** vertex = (GLfloat**)malloc(sizeof(GLfloat*) * 4);
		for (int a = 0; a < 4; a++)
			vertex[a] = (GLfloat*)malloc(sizeof(GLfloat) * 3);

		GLfloat** normal = (GLfloat**)malloc(sizeof(GLfloat*) * 4);
		for (int a = 0; a < 4; a++)
			normal[a] = (GLfloat*)malloc(sizeof(GLfloat) * 3);

		GLfloat** texture = (GLfloat**)malloc(sizeof(GLfloat*) * 4);
		for (int a = 0; a < 4; a++)
			texture[a] = (GLfloat*)malloc(sizeof(GLfloat) * 2);



		for (int j = 0; j < iSlices; j++)
		{
			//fprintf(gpFile, "iSlices = %d,  %d\n", i, j);

			float theta = (j == iSlices) ? 0.0 : j * dtheta;
			float stheta = (float)-sin(theta);
			float ctheta = (float)cos(theta);

			float x = stheta * srho;
			float y = ctheta * srho;
			float z = crho;

			texture[0][0] = s;
			texture[0][1] = t;
			normal[0][0] = x;
			normal[0][1] = y;
			normal[0][2] = z;
			vertex[0][0] = x * fRadius;
			vertex[0][1] = y * fRadius;
			vertex[0][2] = z * fRadius;

			x = stheta * srhodrho;
			y = ctheta * srhodrho;
			z = crhodrho;

			texture[1][0] = s;
			texture[1][1] = t - dt;
			normal[1][0] = x;
			normal[1][1] = y;
			normal[1][2] = z;
			vertex[1][0] = x * fRadius;
			vertex[1][1] = y * fRadius;
			vertex[1][2] = z * fRadius;

			theta = ((j + 1) == iSlices) ? 0.0 : (j + 1) * dtheta;
			stheta = (float)-sin(theta);
			ctheta = (float)cos(theta);

			x = stheta * srho;
			y = ctheta * srho;
			z = crho;

			s += ds;
			texture[2][0] = s;
			texture[2][1] = t;
			normal[2][0] = x;
			normal[2][1] = y;
			normal[2][2] = z;
			vertex[2][0] = x * fRadius;
			vertex[2][1] = y * fRadius;
			vertex[2][2] = z * fRadius;

			x = stheta * srhodrho;
			y = ctheta * srhodrho;
			z = crhodrho;

			texture[3][0] = s;
			texture[3][1] = t - dt;
			normal[3][0] = x;
			normal[3][1] = y;
			normal[3][2] = z;
			vertex[3][0] = x * fRadius;
			vertex[3][1] = y * fRadius;
			vertex[3][2] = z * fRadius;

			addTriangle(vertex, normal, texture);

			vertex[0][0] = vertex[1][0];
			vertex[0][1] = vertex[1][1];
			vertex[0][2] = vertex[1][2];
			normal[0][0] = normal[1][0];
			normal[0][1] = normal[1][1];
			normal[0][2] = normal[1][2];
			texture[0][0] = texture[1][0];
			texture[0][1] = texture[1][1];

			vertex[1][0] = vertex[3][0];
			vertex[1][1] = vertex[3][1];
			vertex[1][2] = vertex[3][2];
			normal[1][0] = normal[3][0];
			normal[1][1] = normal[3][1];
			normal[1][2] = normal[3][2];
			texture[1][0] = texture[3][0];
			texture[1][1] = texture[3][1];

			addTriangle(vertex, normal, texture);
		}

		t -= dt;
	}

	prepareToDraw();
}








