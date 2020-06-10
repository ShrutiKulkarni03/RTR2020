#include <stdio.h>
int main(void)
{
	//variable declarations
	int sk_i, sk_j, sk_c;
	//code
	printf("\n\n");
	for (sk_i = 0; sk_i < 128; sk_i++)
	{
		for (sk_j = 0; sk_j < 128; sk_j++)
		{
			sk_c = ((sk_i & 0x8) == 0) ^ ((sk_j & 0x8) == 0);
			if (sk_c == 0)
				printf(" ");
			if (sk_c == 1)
				printf("* ");
		}
		printf("\n\n");
	}

	getch();
	return(0);
}
