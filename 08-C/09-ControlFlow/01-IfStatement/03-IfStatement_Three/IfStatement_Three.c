#include<stdio.h>

int main(void)
{
	int sk_num;

	printf("\n\nEnter an Integer\n");
	scanf("%d", &sk_num);
	printf("\n\n");

	if ((sk_num >= 0) && (sk_num <= 100))
	{
		printf("Your number is between 0 to 100\n\n");
	}
	
	if ((sk_num > 100) && (sk_num <= 200))
	{
		printf("Your number is between 100 to 200\n\n");
	}

	if ((sk_num > 200) && (sk_num <= 300))
	{
		printf("Your number is between 200 to 300\n\n");
	}

	if ((sk_num > 300) && (sk_num <= 400))
	{
		printf("Your number is between 300 to 400\n\n");
	}

	if ((sk_num > 400) && (sk_num <= 500))
	{
		printf("Your number is between 400 to 500\n\n");
	}

	return(0);

}
