#include <stdio.h>
int main(void)
{
	
	int sk_i, sk_j, sk_k;
	
	printf("\n\n");
	sk_i = 1;
	do
	{
		printf("i = %d\n", sk_i);
		
		sk_j = 1;
		do
		{
			printf("\tj = %d\n", sk_j);
			
			sk_k = 1;
			do
			{
				printf("\t\tk = %d\n", sk_k);
				sk_k++;
			} while (sk_k <= 1);
			printf("\n\n");
			sk_j++;
		} while (sk_j <= 2);
		printf("\n\n");
		sk_i++;
	} while (sk_i <= 3);

	getch();
	return(0);
}
