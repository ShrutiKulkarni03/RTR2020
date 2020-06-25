#include<stdio.h>

#define NUM_ROWS 3
#define NUM_COLUMNS 4

int main(void)
{
	int sArray_2D[NUM_ROWS][NUM_COLUMNS];
	int sArray_1D[NUM_ROWS * NUM_COLUMNS];

	int a, b, num;

	printf("\n\nEnter Elements of 2D Integer Array");
	for (a = 0; a < NUM_ROWS; a++)
	{
		printf("\n\nFor ROW NUMBER %d \n", (a + 1));
		for (b = 0; b < NUM_COLUMNS; b++)
		{
			printf("Enter Element Number %d", (b + 1));
			scanf("%d", &num);
			sArray_2D[a][b] = num;

		}
	}

	printf("2D Array of Integers :\n");
	for (a = 0; a < NUM_ROWS; a++)
	{
		printf("\nROW %d\n", (a + 1));
		for (b = 0; b < NUM_COLUMNS; b++)
		{
			printf("sArray_2D[%d][%d] = %d\n", a, b, sArray_2D[a][b]);
		}
	}
	for (a = 0; a < NUM_ROWS; a++)
	{
		for (b = 0; b < NUM_COLUMNS; b++)
		{
			sArray_1D[(a * NUM_COLUMNS) + b] = sArray_2D[a][b];
		}
	}

	printf("\n\n1D Array of Integers :\n");
	for (a = 0; a < (NUM_ROWS * NUM_COLUMNS); a++)
	{
		printf("sArray_1D[%d] = %d\n", a, sArray_1D[a]);

	}

	getch();
	return(0);
}
