#include<stdio.h>

int main(void)
{
	int sk_num;

	printf("\n\n");
	printf("Enter a number\n");
	scanf("%d", &sk_num);

	if (sk_num < 0)
	{
		printf("Number is less than zero (negative)\n");
	}
	else
	{
		if ((sk_num >= 0) && (sk_num <= 100))
		{
			printf("\nNumber is between 0 and 100\n");
		}
		else
		{
			if ((sk_num > 101) && (sk_num <= 200))
			{
				printf("Number is between 101 and 200\n");
			}
			else
			{
				if ((sk_num > 201) && (sk_num <= 300))
				{
					printf("Number is between 201 and 300\n");
				}
				else
				{
					if ((sk_num > 301) && (sk_num <= 400))
					{
						printf("Number is between 301 and 400\n");
					}
					else
					{
						if ((sk_num > 401) && (sk_num <= 500))
						{
							printf("Number is between 401 and 500\n");
						}
						else
						{
							printf("Number is greater than 500\n");
						}
					}
				}
			}
		}
	}

	getch();
	return(0);

}
