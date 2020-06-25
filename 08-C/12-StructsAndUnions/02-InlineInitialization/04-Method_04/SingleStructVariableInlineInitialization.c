#include<stdio.h>

struct MyData
{
	int i;
	float f;
	double d;
	char c;
};

int main(void)
{
	struct MyData data1 = { 55, 37.568f, 4.287904, 'S' };

	struct MyData data2 = { 4.879f, 19, 'A', 9.875321 };

	struct MyData data3 = {'P', 471 };

	struct MyData data4 = { 'K' };

	printf("\n\nMembers of 'struct MyData data1' are\n");
	printf("i is %d\n", data1.i);
	printf("f is %f\n", data1.f);
	printf("d is %lf\n", data1.d);
	printf("c is %c\n", data1.c);

	printf("\n\nMembers of 'struct MyData data2' are\n");
	printf("i is %d\n", data2.i);
	printf("f is %f\n", data2.f);
	printf("d is %lf\n", data2.d);
	printf("c is %c\n", data2.c);

	printf("\n\nMembers of 'struct MyData data3' are\n");
	printf("i is %d\n", data3.i);
	printf("f is %f\n", data3.f);
	printf("d is %lf\n", data3.d);
	printf("c is %c\n", data3.c);

	getch();
	return(0);

}
