#include<stdio.h>
int main(void)
{
	int sk_i, sk_j;

	printf("\n\nNumber 10-1 and 100 - 10\n\n");

	for (sk_i = 10, sk_j = 100; sk_i >= 1, sk_j >=10; sk_i--, sk_j -= 10)
	{
		printf("\t%d\t   %d\n", sk_i, sk_j);
	}

	getch();
	return(0);
}

