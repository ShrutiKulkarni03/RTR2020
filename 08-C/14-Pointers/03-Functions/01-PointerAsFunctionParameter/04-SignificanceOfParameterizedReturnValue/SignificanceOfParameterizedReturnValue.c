#include<stdio.h>

enum
{
	NEGATIVE = -1,
	ZERO,
	POSITIVE
};

int main(void)
{
	//function declaration
	int Difference(int, int, int*);

	//variable declaration
	int x, y, ans, ret;

	//code
	printf("\n\nEnter Value of 'X' : ");
	scanf("%d", &x);

	printf("\n\nEnter Value of 'Y' : ");
	scanf("%d", &y);

	ret = Difference(x, y, &ans);

	printf("\n\nDifference of %d and %d = %d\n\n", x, y, ans);

	if (ret == POSITIVE)
		printf("The Difference of %d and %d is Positive!\n\n", x, y);

	else if (ret == NEGATIVE)
		printf("The Difference of %d and %d is Negative!\n\n", x, y);

	else
		printf("The Difference of %d and %d is Zero!\n\n", x, y);

	getch();
	return(0);

}

int Difference(int a, int b, int* difference)
{
	//code

	*difference = a - b;

	if (*difference > 0)
		return(POSITIVE);

	else if (*difference < 0)
	{
		return(NEGATIVE);
	}

	else
		return(ZERO);
}
