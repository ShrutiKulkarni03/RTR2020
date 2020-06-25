#include<stdio.h>

int main(void)
{
	struct MyPoint
	{
		int x, y;

	}point;

	struct MyPointProperties
	{
		int quad;
		char axis_loc[10];
	}PointProps;


	printf("\n\nEnter Co-ordinates of point 'A' : ");
	scanf("%d", &point.x);
	printf("Enter Co-ordinates of point 'B' : ");
	scanf("%d", &point.y);
	printf("\nCo-ordinates of point 'A' are (%d, %d)\n", point.x, point.y);

	if (point.x == 0 && point.y == 0)
		printf("The point is the Origin (%d, %d)\n", point.x, point.y);

	else
	{
		if (point.x == 0)
		{
			if (point.y < 0)
				strcpy(PointProps.axis_loc, "Negative Y");

			if (point.y > 0)
				strcpy(PointProps.axis_loc, "Positive Y");

			PointProps.quad = 0;

			printf("The point lies on the %s Axis\n", PointProps.axis_loc);
		}
		else if (point.y == 0)
		{
			if (point.x < 0)
				strcpy(PointProps.axis_loc, "Negative X");

			if (point.x > 0)
				strcpy(PointProps.axis_loc, "Positive X");

			PointProps.quad = 0;

			printf("The point lies on the %s Axis\n", PointProps.axis_loc);
		}
		else
		{
			PointProps.axis_loc[0] = '\0';

			if (point.x > 0 && point.y > 0)
				PointProps.quad = 1;

			else if (point.x < 0 && point.y > 0)
				PointProps.quad = 2;

			else if (point.x > 0 && point.y < 0)
				PointProps.quad = 3;

			else
				PointProps.quad = 4;

			printf("The point lies in Quadrant %d\n", PointProps.quad);
		}
	}


	getch();
	return(0);

}
