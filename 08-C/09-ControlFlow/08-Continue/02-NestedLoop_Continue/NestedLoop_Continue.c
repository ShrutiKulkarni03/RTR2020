#include<stdio.h>

int main(void)
{
	int sk_s, sk_k;

	printf("\n\nOuter Loop prints Odd Numbers between 0 to 10.\n");
	printf("Inner Loop Prints Even Numbers Between 1 and 10 For Every Odd Number Printed By Outer Loop.\n");

	for (sk_s = 1; sk_s <= 5; sk_s++)
	{
		if (sk_s % 2 != 2)
		{
			printf("s is %d\n", sk_s);
			
			for (sk_k = 1; sk_k <= 10; sk_k++)
			{
				if (sk_k % 2 == 0)
				{
					printf("\tk is %d\n", sk_k);
				}
				else
				{
					continue;
				}
			}
			printf("\n\n");
		}
		else
		{
			continue;
		}
		
	}

	getch();
	return(0);

}

