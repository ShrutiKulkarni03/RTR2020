#include <stdio.h>
int main(void)
{
	
	int sk_i, sk_j;
	
	printf("\n\n");
	printf("Printing Digits 10 to 1 and 100 to 10 : \n\n");

	sk_i = 10;
	sk_j = 100;

	while (sk_i >= 1, sk_j >= 10)
	{
		printf("\t %d \t %d\n", sk_i, sk_j);
		sk_i--;
		sk_j -= 10;
	}

	printf("\n\n");

	getch();
	return(0);

}
