#include<stdio.h>

struct MyData
{
	int i;
	float f;
	double d;
	char c;

}data;

int main(void)
{
	int iSize, fSize, dSize, cSize, struct_MyDataSize;

	data.i = 20;
	data.f = 3.1097f;
	data.d = 3.142857;
	data.c = 'S';

	printf("\n\nMembers of 'struct MyData' are\n");
	printf("i is %d\n", data.i);
	printf("f is %f\n", data.f);
	printf("d is %lf\n", data.d);
	printf("c is %c\n", data.c);

	iSize = sizeof(data.i);
	fSize = sizeof(data.f);
	dSize = sizeof(data.d);
	cSize = sizeof(data.c);

	printf("\n\nSizes of members od 'struct MyData' are\n");
	printf("Size of 'i' is %d\n", iSize);
	printf("Size of 'f' is %d\n", fSize);
	printf("Size of 'd' is %d\n", dSize);
	printf("Size of 'c' is %d\n", cSize);

	struct_MyDataSize = sizeof(struct MyData);
	printf("Size of 'struct MyData' is %d\n", struct_MyDataSize);

	getch();
	return(0);

}
