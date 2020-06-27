#include <stdio.h>
int main(void)
{
	
	int sk_i, sk_j;
	
	printf("\n\n");

	for (sk_i = 1; sk_i <= 5; sk_i++)
	{
		printf("i = %d\n", sk_i);
		printf("\n\n");

		for (sk_j = 1; sk_j <= 5; sk_j++)
		{
			printf("\tj = %d\n", sk_j);
		}

		printf("\n\n");
	}

	getch();
	return(0);
}

