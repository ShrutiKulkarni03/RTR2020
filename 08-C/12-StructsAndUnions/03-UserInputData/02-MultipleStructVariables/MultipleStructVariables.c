#include<stdio.h>

struct MyPoint
{
	float x, y;

};

int main(void)
{
	struct MyPoint pointA, pointB, pointC, pointD, pointE;

	printf("\n\nEnter X coordinate for point 'A' : ");
	scanf("%f", &pointA.x);
	printf("Enter Y coordinate for point 'A' : ");
	scanf("%f", &pointA.y);

	printf("\nEnter X coordinate for point 'B' : ");
	scanf("%f", &pointB.x);
	printf("Enter Y coordinate for point 'B' : ");
	scanf("%f", &pointB.y);

	printf("\nEnter X coordinate for point 'C' : ");
	scanf("%f", &pointC.x);
	printf("Enter Y coordinate for point 'C' : ");
	scanf("%f", &pointC.y);

	printf("\nEnter X coordinate for point 'D' : ");
	scanf("%f", &pointD.x);
	printf("Enter Y coordinate for point 'D' : ");
	scanf("%f", &pointD.y);

	printf("\nEnter X coordinate for point 'E' : ");
	scanf("%f", &pointE.x);
	printf("Enter Y coordinate for point 'E' : ");
	scanf("%f", &pointE.y);


	printf("\n\nCo-ordinates of point 'A' are (%f, %f)\n", pointA.x, pointA.y);
	printf("Co-ordinates of point 'B' are (%f, %f)\n", pointB.x, pointB.y);
	printf("Co-ordinates of point 'C' are (%f, %f)\n", pointC.x, pointC.y);
	printf("Co-ordinates of point 'D' are (%f, %f)\n", pointD.x, pointD.y);
	printf("Co-ordinates of point 'E' are (%f, %f)\n", pointE.x, pointE.y);


	getch();
	return(0);

}
