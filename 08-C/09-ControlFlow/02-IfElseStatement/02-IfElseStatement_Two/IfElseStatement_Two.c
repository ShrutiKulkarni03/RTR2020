#include<stdio.h>

int main(void)
{
	int sk_age;

	printf("\n\nEnter your age\n");
	scanf("%d", &sk_age);
	printf("\n");

	if (sk_age >= 18)
	{
		printf("You are eligible for voting\n");
	}
	else
	{
		printf("You are not eligible for voting\n\n");
	}
	printf("Thank You!\n");

	return(0);

}
