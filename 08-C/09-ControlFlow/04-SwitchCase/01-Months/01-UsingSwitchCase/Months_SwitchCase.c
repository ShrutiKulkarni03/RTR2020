#include<stdio.h>
int main(void)
{
	int sk_month_num;

	printf("\n\nEnter number of month\n");
	scanf("%d", &sk_month_num);
	printf("\n");

	switch (sk_month_num)
	{
	case 1:
		printf("Month %d is January\n\n", sk_month_num);
		break;

	case 2:
		printf("Month %d is February\n\n", sk_month_num);
		break;

	case 3:
		printf("Month %d is March\n\n", sk_month_num);
		break;

	case 4:
		printf("Month %d is April\n\n", sk_month_num);
		break;

	case 5:
		printf("Month %d is May\n\n", sk_month_num);
		break;

	case 6:
		printf("Month %d is June\n\n", sk_month_num);
		break;

	case 7:
		printf("Month %d is July\n\n", sk_month_num);
		break;

	case 8:
		printf("Month %d is August\n\n", sk_month_num);
		break;

	case 9:
		printf("Month %d is September\n\n", sk_month_num);
		break;

	case 10:
		printf("Month %d is October\n\n", sk_month_num);
		break;

	case 11:
		printf("Month %d is November\n\n", sk_month_num);
		break;

	case 12:
		printf("Month %d is December\n\n", sk_month_num);
		break;

	default:
		printf("Invalid number of month input\n\n");
		break;

	}

	printf("Switch Case Block Complete\n");

	getch();
	return(0);
}
