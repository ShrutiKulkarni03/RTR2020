#include<stdio.h>

#define NUM_ELEMENTS 10

int main(void)
{
	int sArray[NUM_ELEMENTS], s, num, sum = 0;

	printf("\n\nEnter Integer Elements for array\n");

	for (s = 0; s < NUM_ELEMENTS; s++)
	{
		scanf("%d", &num);
		sArray[s] = num;
	}

	for (s = 0; s < NUM_ELEMENTS; s++)
	{
		sum += sArray[s];
	}

	printf("\nSum of all Elements of Array is %d\n", sum);

	getch();
	return(0);
}
