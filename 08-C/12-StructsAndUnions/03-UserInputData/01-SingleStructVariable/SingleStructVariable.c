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
	struct MyData data;

	printf("\n\nEnter Integer Value for member 'i' of the struct : ");
	scanf("%d", &data.i);

	printf("\nEnter Floating Point Value for member 'f' of the struct : ");
	scanf("%f", &data.f);

	printf("\nEnter Double Value for member 'd' of the struct : ");
	scanf("%lf", &data.d);

	printf("\nEnter Character for member 'c' of the struct : ");
	data.c = getch();

	printf("\n\nMembers of 'struct MyData data' are\n");
	printf("i is %d\n", data.i);
	printf("f is %f\n", data.f);
	printf("d is %lf\n", data.d);
	printf("c is %c\n", data.c);

	getch();
	return(0);

}
