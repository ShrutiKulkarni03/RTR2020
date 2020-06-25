#include<stdio.h>

#define NUM_ELEMENTS 5

int main(void)
{
	int sArray[NUM_ELEMENTS], s, num, sum = 0;

	printf("\n\nEnter Integer Elements for array\n");

	for (s = 0; s < NUM_ELEMENTS; s++)
	{
		scanf("%d", &num);
		sArray[s] = num;
	}

	printf("\n\nEven numbers from array are\n");
	for (s = 0; s < NUM_ELEMENTS; s++)
	{
		if ((sArray[s] % 2) == 0)
			printf("%d\n", sArray[s]);
	}

	printf("\n\nOdd numbers from array are\n");
	for (s = 0; s < NUM_ELEMENTS; s++)
	{
		if ((sArray[s] % 2) != 0)
			printf("%d\n", sArray[s]);
	}
	

	getch();
	return(0);
}
