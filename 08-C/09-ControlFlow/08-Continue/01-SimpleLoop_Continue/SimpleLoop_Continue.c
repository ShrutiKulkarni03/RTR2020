#include<stdio.h>

int main(void)
{
	int sk_i;

	printf("\n\nPrinting Even Numbers from 0 to 80\n\n");

	for (sk_i = 0; sk_i <= 80; sk_i++)
	{
		if (sk_i % 2 != 0)
		{
			continue;
		}
		else
		{
			printf("\t%d\n", sk_i);
		}
	}

	getch();
	return(0);

}
