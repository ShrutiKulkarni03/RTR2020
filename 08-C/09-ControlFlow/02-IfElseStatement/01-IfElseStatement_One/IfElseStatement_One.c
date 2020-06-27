#include<stdio.h>

int main(void)
{
	int sk_a, sk_b, sk_c;

	printf("\n\nEnter an Integer 'A'\n");
	scanf("%d", &sk_a);

	printf("\nEnter an Integer 'B'\n");
	scanf("%d", &sk_b);

	if (sk_a < sk_b)
	{
		printf("\n'A' is less than 'B'\n");
	}
	else
	{
		printf("\n'A' is not less than 'B'\n");
	}

	printf("\n\nEnter an Integer 'C'\n");
	scanf("%d", &sk_c);

	if (sk_b != sk_c)
	{
		printf("\n'B' is not equal to 'C'\n");
	}
	else
	{
		printf("\n'B' is equal to 'C'\n");
	}

	return(0);

}
