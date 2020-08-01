#include<stdio.h>
#include<stdlib.h>

int main(void)
{
	//function prototype
	void SwapNumbers(int, int);

	//variable declaration
	int s, k;

	//code
	printf("\n\nEnter Value for 'S' : ");
	scanf("%d", &s);

	printf("\nEnter Value for 'K' : ");
	scanf("%d", &k);

	printf("\n\nBefore Swapping\n\n");
	printf("Value of 'S' = %d\n", s);
	printf("Value of 'K' = %d\n", k);

	SwapNumbers(s, k);

	printf("\n\nAfter Swapping\n\n");
	printf("Value of 'S' = %d\n", s);
	printf("Value of 'K' = %d\n", k);

	return(0);

}

void SwapNumbers(int x, int y)
{
	int temp;

	printf("\n\nBefore Swapping\n\n");
	printf("Value of 'X' = %d\n", x);
	printf("Value of 'Y' = %d\n", y);

	temp = x;
	x = y;
	y = temp;

	printf("\n\nAfter Swapping\n\n");
	printf("Value of 'X' = %d\n", x);
	printf("Value of 'Y' = %d\n", y);

}

