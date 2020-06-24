#include<stdio.h>

struct MyData
{
	int i;
	float f;
	double d;

};

int main(void)
{
	struct MyData data;

	int iSize, fSize, dSize, struct_MyDataSize;

	data.i = 20;
	data.f = 3.1097f;
	data.d = 3.142857;

	printf("\n\nMembers of 'struct MyData' are\n");
	printf("i is %d\n", data.i);
	printf("f is %f\n", data.f);
	printf("d is %lf\n", data.d);

	iSize = sizeof(data.i);
	fSize = sizeof(data.f);
	dSize = sizeof(data.d);
	

	printf("\n\nSizes of members od 'struct MyData' are\n");
	printf("Size of 'i' is %d\n", iSize);
	printf("Size of 'f' is %d\n", fSize);
	printf("Size of 'd' is %d\n", dSize);
	

	struct_MyDataSize = sizeof(struct MyData);
	printf("Size of 'struct MyData' is %d\n", struct_MyDataSize);

	getch();
	return(0);

}
