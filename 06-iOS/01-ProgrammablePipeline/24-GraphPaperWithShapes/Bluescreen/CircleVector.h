#include<stdlib.h>
#pragma once

struct vector 
{
	float* vertArray;
	int size;
};
//struct vector* circle;

struct vector* createVector(void);
//void createVector(struct vector* circle);
void pushBack(struct vector* circle, float element);
void destroyVector(struct vector* circle);

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



