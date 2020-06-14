#include<stdio.h>
#include<conio.h>

int main(void)
{
	int sk_i, sk_j;

	printf("\n\n");

	for (sk_i = 1; sk_i <= 50; sk_i++);
	{
		for (sk_j = 1; sk_j <= 50; sk_j++)
		{
			if (sk_j > sk_i)
			{
				break;
			}
			else
			{
				printf("*");
			}
		}
		printf("\n");
	}

	getch();
	return(0);

}
