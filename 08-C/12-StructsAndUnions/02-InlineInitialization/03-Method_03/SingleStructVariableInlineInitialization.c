#include<stdio.h>

int main(void)
{
	struct MyData
	{
		int i;
		float f;
		double d;
		char c;
	}data = { 20, 3.1097f, 3.142857, 'S' };

	printf("\n\nMembers of 'struct MyData' are\n");
	printf("i is %d\n", data.i);
	printf("f is %f\n", data.f);
	printf("d is %lf\n", data.d);
	printf("c is %c\n", data.c);

	getch();
	return(0);

}
