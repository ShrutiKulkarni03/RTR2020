#include <stdio.h>
int main(void)
{
	
	int sk_i;
	
	printf("\n\n");
	printf("Printing Digits 1 to 10 : \n\n");

	sk_i = 1;
	while (sk_i <= 10)
	{
		printf("\t%d\n", sk_i);
		sk_i++;
	}

	printf("\n\n");

	getch();
	return(0);

}
