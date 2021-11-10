#include "vmath.h"
#include "inc/GlobalVariableDeclarations.h"
#pragma once

using namespace vmath;

//const double PI = 3.14159265358979323846;

//enum
//{
//	SPK_ATTRIBUTE_POSITION = 0,
//	SPK_ATTRIBUTE_COLOR,
//	SPK_ATTRIBUTE_NORMAL,
//	SPK_ATTRIBUTE_TEXCOORD,
//};

struct vec_int
{
	int* p;
	int size;
};

struct vec_float
{
	float* pf;
	int size;
};

//variables

struct vec_float* gp_vertices, * gp_texture, * gp_normal;

struct vec_int* gp_indices;

GLuint vao_torus;
GLuint vbo_position_torus;
GLuint vbo_texcoord_torus;
GLuint vbo_normal_torus;
GLuint vbo_indices;

int numIndices = 0;
int primitiveRestartIndex = 0;

float currentMainSegmentAngle;
float currentTubeSegmentAngle;

int torusNumVertices;

//prototypes
struct vec_int* create_vec_int();
struct vec_float* create_vec_float();
int push_back_vec_int(struct vec_int* p_vec_int, int data);
int push_back_vec_float(struct vec_float* p_vec_int, float data);
int destroy_vec_int(struct vec_int* p_vec_int);
int destroy_vec_float(struct vec_float* p_vec_float);

void makeTorus(float, float, int, int);
void prepareToDrawTorus(void);
void drawTorus(void);
void uninitTorus(void);

struct vec_int* create_vec_int(void)
{
	struct vec_int* p = (struct vec_int*)malloc(sizeof(struct vec_int));
	memset(p, 0, sizeof(struct vec_int));
	return(p);
}

struct vec_float* create_vec_float(void)
{
	struct vec_float* p = (struct vec_float*)malloc(sizeof(struct vec_float));
	memset(p, 0, sizeof(struct vec_float));
	return(p);
}

int push_back_vec_int(struct vec_int* p_vec_int, int data)
{
	p_vec_int->p = (int*)realloc(p_vec_int->p, (p_vec_int->size + 1) * sizeof(int));
	p_vec_int->size++;
	p_vec_int->p[p_vec_int->size - 1] = data;
	return(0);
}

int push_back_vec_float(struct vec_float* p_vec_float, float data)
{
	p_vec_float->pf = (float*)realloc(p_vec_float->pf, (p_vec_float->size + 1) * sizeof(float));
	p_vec_float->size++;
	p_vec_float->pf[p_vec_float->size - 1] = data;
	return(0);
}

int destroy_vec_int(struct vec_int* p_vec_int)
{
	free(p_vec_int->p);
	free(p_vec_int);

	return(0);
}

int destroy_vec_float(struct vec_float* p_vec_float)
{
	free(p_vec_float->pf);
	free(p_vec_float);

	return(0);
}

void makeTorus(float mainRadius, float tubeRadius, int mainSegments, int tubeSegments)
{
	gp_vertices = create_vec_float();
	gp_texture = create_vec_float();
	gp_normal = create_vec_float();
	gp_indices = create_vec_int();

	torusNumVertices = (mainSegments + 1) * (tubeSegments + 1);
	primitiveRestartIndex = torusNumVertices;
	numIndices = (mainSegments * 2 * (tubeSegments + 1)) + mainSegments - 1;


	float mainSegmentAngleStep = radians(360.0f / float(mainSegments));
	float tubeSegmentAngleStep = radians(360.0f / float(tubeSegments));

	//vertices
	currentMainSegmentAngle = 0.0f;

	for (int i = 0; i <= mainSegments; i++)
	{
		float sinMainSegment = sin(currentMainSegmentAngle);
		float cosMainSegment = cos(currentMainSegmentAngle);

		currentTubeSegmentAngle = 0.0f;

		for (int j = 0; j <= tubeSegments; j++)
		{
			float sinTubeSegment = sin(currentTubeSegmentAngle);
			float cosTubeSegment = cos(currentTubeSegmentAngle);

			GLfloat x_vert = (mainRadius + tubeRadius * cosTubeSegment) * cosMainSegment;
			GLfloat y_vert = (mainRadius + tubeRadius * cosTubeSegment) * sinMainSegment;
			GLfloat z_vert = tubeRadius * sinTubeSegment;

			push_back_vec_float(gp_vertices, x_vert);
			push_back_vec_float(gp_vertices, y_vert);
			push_back_vec_float(gp_vertices, z_vert);

			currentTubeSegmentAngle += tubeSegmentAngleStep;
		}

		currentMainSegmentAngle += mainSegmentAngleStep;
	}

	//normal
	currentMainSegmentAngle = 0.0f;
	for (int i = 0; i <= mainSegments; i++)
	{
		float sinMainSegment = sin(currentMainSegmentAngle);
		float cosMainSegment = cos(currentMainSegmentAngle);

		currentTubeSegmentAngle = 0.0f;

		for (int j = 0; j <= tubeSegments; j++)
		{
			float sinTubeSegment = sin(currentTubeSegmentAngle);
			float cosTubeSegment = cos(currentTubeSegmentAngle);

			GLfloat x_norm = cosMainSegment * cosTubeSegment;
			GLfloat y_norm = sinMainSegment * cosTubeSegment;
			GLfloat z_norm = sinTubeSegment;

			push_back_vec_float(gp_normal, x_norm);
			push_back_vec_float(gp_normal, y_norm);
			push_back_vec_float(gp_normal, z_norm);

			currentTubeSegmentAngle += tubeSegmentAngleStep;
		}

		currentMainSegmentAngle += mainSegmentAngleStep;
	}

	//texture
	float mainSegmentTextureStep = 2.0f / float(mainSegments);
	float tubeSegmentTextureStep = 1.0f / float(tubeSegments);

	float currentMainSegmentTexcoordV = 0.0f;
	for (int i = 0; i <= mainSegments; i++)
	{
		float currentTubeSegmentTexcoordU = 0.0f;

		for (int j = 0; j <= tubeSegments; j++)
		{
			GLfloat u_tex = currentTubeSegmentTexcoordU;
			GLfloat v_tex = currentMainSegmentTexcoordV;

			push_back_vec_float(gp_texture, u_tex);
			push_back_vec_float(gp_texture, v_tex);

			currentTubeSegmentTexcoordU += tubeSegmentTextureStep;
		}

		currentMainSegmentTexcoordV += mainSegmentTextureStep;
	}


	//indices
	GLuint currentVertexOffset = 0;
	for (int i = 0; i < mainSegments; i++)
	{
		for (int j = 0; j <= tubeSegments; j++)
		{
			GLuint vertexIndexA = currentVertexOffset;
			push_back_vec_int(gp_indices, vertexIndexA);

			GLuint vertexIndexB = currentVertexOffset + tubeSegments + 1;
			push_back_vec_int(gp_indices, vertexIndexB);

			currentVertexOffset++;
		}

		if (i != mainSegments - 1)
		{
			push_back_vec_int(gp_indices, primitiveRestartIndex);
		}
	}

	prepareToDrawTorus();

}

void prepareToDrawTorus(void)
{
	
	//vao
	glGenVertexArrays(1, &vao_torus);
	glBindVertexArray(vao_torus);

	glGenBuffers(1, &vbo_position_torus);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_torus);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * torusNumVertices, gp_vertices->pf, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_normal_torus);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal_torus);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * torusNumVertices, gp_normal->pf, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_texcoord_torus);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord_torus);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * torusNumVertices, gp_texture->pf, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_TEXCOORD);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * numIndices, gp_indices->p, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

}

void drawTorus(void)
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBindVertexArray(vao_torus);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);

	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(primitiveRestartIndex);

	glPointSize(5.0f);
	glDrawElements(GL_TRIANGLE_STRIP, numIndices, GL_UNSIGNED_INT, 0);

	glDisable(GL_PRIMITIVE_RESTART);

	//unbind vao
	glBindVertexArray(0);
}

void uninitTorus(void)
{
	destroy_vec_float(gp_normal);
	destroy_vec_float(gp_texture);
	destroy_vec_float(gp_vertices);

	destroy_vec_int(gp_indices);

	if (vao_torus)
	{
		glDeleteVertexArrays(1, &vao_torus);
		vao_torus = 0;
	}

	if (vbo_indices)
	{
		glDeleteVertexArrays(1, &vbo_indices);
		vbo_indices = 0;
	}

	if (vbo_texcoord_torus)
	{
		glDeleteVertexArrays(1, &vbo_texcoord_torus);
		vbo_texcoord_torus = 0;
	}

	if (vbo_normal_torus)
	{
		glDeleteVertexArrays(1, &vbo_normal_torus);
		vbo_normal_torus = 0;
	}

	if (vbo_position_torus)
	{
		glDeleteVertexArrays(1, &vbo_position_torus);
		vbo_position_torus = 0;
	}


}


