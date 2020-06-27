#include <stdio.h>
int main(void)
{
	int sk_i, sk_j;
	
	printf("\n\n");
	printf("Printing Digits 1 to 10 and 10 to 100: \n\n");

	sk_i = 1;
	sk_j = 10;

	do
	{
		printf("\t %d \t %d\n", sk_i, sk_j);
		sk_i++;
		sk_j += 10;
	} while (sk_i <= 10, sk_j <= 100);

	printf("\n\n");

	getch();
	return(0);

}
