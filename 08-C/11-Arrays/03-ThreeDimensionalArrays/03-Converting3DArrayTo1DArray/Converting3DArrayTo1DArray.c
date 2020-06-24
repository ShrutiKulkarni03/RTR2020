#include<stdio.h>

#define NUM_ROWS 4
#define NUM_COLUMNS 3
#define DEPTH 2

int main(void)
{
	int  sArray[NUM_ROWS][NUM_COLUMNS][DEPTH] = { { { 1, 2 }, { 3, 4 }, { 5, 6 } },
							 { { 2, 4 }, { 6, 8 }, { 10, 12 } },
							 { { 3, 6 }, { 9, 12 }, { 15, 18 } },
							 { { 4, 8 }, { 12, 16 }, { 20, 24 } } };

	int a, b, c;
	int sArray_1D[NUM_ROWS * NUM_COLUMNS * DEPTH];

	printf("\n\nElements in 3D Integer Array are\n");

	for (a = 0; a <NUM_ROWS; a++)
	{
		printf("\nROW %d\n", (a + 1));
		for (b = 0; b < NUM_COLUMNS; b++)
		{
			printf("COLUMN %d\n", b + 1);
			for (c = 0; c < DEPTH; c++)
			{
				printf("sArray[%d][%d][%d] = %d\n", a, b, c, sArray[a][b][c]);
			}
		}
	}

	for (a = 0; a < NUM_ROWS; a++)
	{
		for (b = 0; b < NUM_COLUMNS; b++)
		{
			for (c = 0; c < DEPTH; c++)
			{
				sArray_1D[(a * NUM_COLUMNS * DEPTH) + (b * DEPTH) + c] = sArray[a][b][c];
			}
		}
	}

	printf("\n\nElements in 1D Array are\n");
	for (a = 0; a < (NUM_ROWS * NUM_COLUMNS * DEPTH); a++)
	{
		printf("sArray_1D[%d] = %d\n", a, sArray_1D[a]);
	}

	getch();
	return(0);

}
