#include<stdio.h>
int main(void)
{
	int sk_month_num;

	printf("\n\nEnter month number\n");
	scanf("%d", &sk_month_num);
	printf("\n");

	if (sk_month_num == 1)
	{
		printf("Month %d is January\n\n", sk_month_num);
	}
	else if (sk_month_num == 2)
	{
		printf("Month %d is February\n\n", sk_month_num);
	}
	else if (sk_month_num == 3)
	{
		printf("Month %d is March\n\n", sk_month_num);
	}
	else if (sk_month_num == 4)
	{
		printf("Month %d is April\n\n", sk_month_num);
	}
	else if (sk_month_num == 5)
	{
		printf("Month %d is May\n\n", sk_month_num);
	}
	else if (sk_month_num == 6)
	{
		printf("Month %d is June\n\n", sk_month_num);
	}
	else if (sk_month_num == 7)
	{
		printf("Month %d is July\n\n", sk_month_num);
	}
	else if (sk_month_num == 8)
	{
		printf("Month %d is August\n\n", sk_month_num);
	}
	else if (sk_month_num == 9)
	{
		printf("Month %d is September\n\n", sk_month_num);
	}
	else if (sk_month_num == 10)
	{
		printf("Month %d is October\n\n", sk_month_num);
	}
	else if (sk_month_num == 11)
	{
		printf("Month %d is November\n\n", sk_month_num);
	}
	else if (sk_month_num == 12)
	{
		printf("Month %d is December\n\n", sk_month_num);
	}
	else
	{
		printf("Invlid Month Number Input\n\n");
	}

	getch();
	return(0);

}
