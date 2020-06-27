#include <stdio.h>
int main(void)
{
	
	int sk_i, sk_j, sk_k;
	
	printf("\n\n");
	sk_i = 0;
	while (sk_i < 64)
	{
		sk_j = 0;
		while (sk_j < 64)
		{
			sk_k = ((sk_i & 0x8) == 0) ^ ((sk_j & 0x8) == 0);
			if (sk_k == 0)
				printf(" ");
			if (sk_k == 1)
			printf("* ");
			sk_j++;
		}
		printf("\n\n"); 
		sk_i++;
	}

	getch();
	return(0);
}

