#include<stdio.h>

#define NUM_ELEMENTS 10

int main(void)
{
	int sArray[NUM_ELEMENTS], s, k, num, count = 0;

	printf("\n\nEnter Integer Elements for array\n");

	for (s = 0; s < NUM_ELEMENTS; s++)
	{
		scanf("%d", &num);

		if (num < 0)
			num = -1 * num;

		sArray[s] = num;
	}

	printf("\n\nArray Elements are\n");
	for (s = 0; s < NUM_ELEMENTS; s++)
		printf("%d\n", sArray[s]);


	printf("\n\nPrime numbers from array are\n");
	for (s = 0; s < NUM_ELEMENTS; s++)
	{
		for (k = 1; k <= sArray[s]; k++)
		{
			if ((sArray[s] % k) == 0)
				count++;
		}

		if (count == 2)
			printf("%d\n", sArray[s]);
		count = 0;
		
	}
	

	getch();
	return(0);
}
