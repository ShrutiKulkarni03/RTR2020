#include <stdio.h>
int main(void)
{
	
	int sk_i, sk_j, sk_k;
	
	printf("\n\n");

	for (sk_i = 1; sk_i <= 4; sk_i++)
	{
		printf("i = %d\n", sk_i);
		printf("\n\n");

		for (sk_j = 1; sk_j <= 3; sk_j++)
		{
			printf("\tj = %d\n", sk_j);

			for (sk_k = 1; sk_k <= 2; sk_k++)
			{
				printf("\t\tk = %d\n", sk_k);
			}
		}
	}

	getch();
	return(0);
}

