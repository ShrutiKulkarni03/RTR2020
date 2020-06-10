#include<stdio.h>

int main(void)
{
	int sk_1;

	printf("\n\n");
	printf("Enter an Integer 'A'\n");
	scanf("%d", &sk_1);
	printf("\n");

	if (sk_1 < 0)
	{
		printf("'A' exists and has non-zero negative value %d\n", sk_1);
	}
	if (sk_1 > 0)
	{
		printf("'A' exists and has non-zero positive value %d\n", sk_1);
	}
	if (sk_1 == 0)
	{
		printf("'A' exists and has zero value %d\n", sk_1);
	}

	return(0);

}
