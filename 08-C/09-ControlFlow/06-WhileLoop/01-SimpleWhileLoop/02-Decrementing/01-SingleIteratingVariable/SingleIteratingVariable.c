#include <stdio.h>
int main(void)
{
	
	int sk_i;
	
	printf("\n\n");
	printf("Printing Digits 10 to 1 : \n\n");

	sk_i = 10;
	while (sk_i >= 1)
	{
		printf("\t%d\n", sk_i);
		sk_i--;
	}

	printf("\n\n");

	getch();
	return(0);

}
