#include <stdio.h>
int main(void)
{
	
	int sk_i, sk_j, sk_c;

	printf("\n\n");
	sk_i = 0;
	do
	{
		sk_j = 0;
		do
		{
			sk_c = ((sk_i & 0x8) == 0) ^ ((sk_j & 0x8) == 0);
			if (sk_c == 0)
				printf(" ");
			if (sk_c == 1)
				printf("* ");
			sk_j++;
		} while (sk_j < 64);
		printf("\n\n");
		sk_i++;
	} while (sk_i < 64);

	getch();
	return(0);
}
