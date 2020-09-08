#include<stdio.h>
#include<stdlib.h>

#define PIE 3.1415

int main(int argc, char* argv[])
{
	float area;
	float r = atof(argv[1]);

	area = PIE * r * r;

	printf("\n%f\n", area);

	return(0);
	getch();

}
