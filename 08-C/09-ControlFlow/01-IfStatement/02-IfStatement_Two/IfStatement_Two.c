#include<stdio.h>

int main(void)
{
	int sk_age;

	printf("\n\n");
	printf("Enter your Age\n");
	scanf("%d", &sk_age);

	if (sk_age >= 18)
	{
		printf("You are eligible for voting\n\n");
	}

	return(0);

	}
