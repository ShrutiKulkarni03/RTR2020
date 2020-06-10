#include <stdio.h>
int main(void)
{

	int sk_i, sk_j;

	printf("\n\n");

	sk_i = 1;
	while (sk_i <= 5)
	{
		printf("i = %d\n", sk_i);
		printf("--------\n\n");
		sk_j = 1;
		while (sk_j <= 3)
		{
			printf("\tj = %d\n", sk_j);
			sk_j++;
		}
		sk_i++;
		printf("\n\n");
	}

	getch();
	return(0);
}

