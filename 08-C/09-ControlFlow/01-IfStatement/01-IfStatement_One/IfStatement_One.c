#include<stdio.h>

int main(void)
{
	int sk_a, sk_b;

	printf("\n\n");

	printf("Enter an Integer 'A'\n");
	scanf("%d", &sk_a);

	printf("Enter an Integer 'B'\n");
	scanf("%d", &sk_b);

	if (sk_a < sk_b)
	{
		printf("A is less than B\n");
	}
	if (sk_a == sk_b)
	{
		printf("A is equal to B\n");
	}

	printf("Two Comparisons are done\n\n");

	return(0);

}		
