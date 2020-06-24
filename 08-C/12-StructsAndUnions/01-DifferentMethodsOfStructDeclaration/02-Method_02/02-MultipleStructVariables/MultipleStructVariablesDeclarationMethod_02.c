#include<stdio.h>

struct MyPoint
{
	int x, y;

};

struct MyPoint pointA, pointB, pointC, pointD, pointE;

int main(void)
{
	pointA.x = 1;
	pointA.y = 2;

	pointB.x = 3;
	pointB.y = 4;

	pointC.x = 5;
	pointC.y = 6;

	pointD.x = 7;
	pointD.y = 8;

	pointE.x = 9;
	pointE.y = 10;

	printf("\n\nCo-ordinates of point 'A' are (%d, %d)\n", pointA.x, pointA.y);
	printf("Co-ordinates of point 'B' are (%d, %d)\n", pointB.x, pointB.y);
	printf("Co-ordinates of point 'C' are (%d, %d)\n", pointC.x, pointC.y);
	printf("Co-ordinates of point 'D' are (%d, %d)\n", pointD.x, pointD.y);
	printf("Co-ordinates of point 'E' are (%d, %d)\n", pointE.x, pointE.y);


	getch();
	return(0);

}
