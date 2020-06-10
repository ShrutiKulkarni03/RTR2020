#include <stdio.h>
int main(void)
{
	
	int sk_i, sk_j, sk_k;
	
	printf("\n\n");

	sk_i = 1;
	while (sk_i <= 5)
	{
		printf("i = %d\n", sk_i);
		sk_j = 1;
		while (sk_j <= 3)
		{
			printf("\tj = %d\n", sk_j);
			sk_k = 1;
			while (sk_k <= 2)
			{
				printf("\t\tk = %d\n", sk_k);
				sk_k++;
			}
			sk_j++;
		}
		printf("\n\n");
		sk_i++;
	}

	getch();
	return(0);
}

