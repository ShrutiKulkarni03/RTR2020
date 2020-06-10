#include<stdio.h>
int main(void)
{
	int sk_num;

	printf("\n\nEnter an unsigned integer\n");
	scanf("%d", &sk_num);

	if ((sk_num >= 0) && (sk_num <= 100))
	{
		printf("Number is between 0 and 100\n");
	}
	else if ((sk_num > 100) && (sk_num <= 200))
	{
		printf("Number is between 101 and 200\n");
	}
	else if ((sk_num > 200) && (sk_num <= 300))
	{
		printf("Number is between 201 and 300\n");
	}
	else if ((sk_num > 300) && (sk_num <= 400))
	{
		printf("Number is between 301 and 400\n");
	}
	else if ((sk_num > 400) && (sk_num <= 500))
	{
		printf("Number is between 401 and 500\n");
	}
	else if (sk_num > 500)
	{
		printf("Number is greater than 500\n");
	}
	else
	{
		printf("Invalid Input\n");
	}

	getch();
	return(0);

}
