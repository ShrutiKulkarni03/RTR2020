#include <stdio.h>
int main(void)
{
	
	int sk_i, sk_j;

	printf("\n\n");
	sk_i = 1;
	do
	{
		printf("i = %d\n", sk_i);
		
		sk_j = 1;
		do
		{
			printf("\tj = %d\n", sk_j);
			sk_j++;
		} while (sk_j <= 3);
		sk_i++;
		printf("\n\n");
	} while (sk_i <= 10);

	getch();
	return(0);
}
