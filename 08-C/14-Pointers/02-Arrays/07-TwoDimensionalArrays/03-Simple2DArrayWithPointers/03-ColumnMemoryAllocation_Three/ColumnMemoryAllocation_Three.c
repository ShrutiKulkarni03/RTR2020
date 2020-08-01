#include<stdio.h>
#include<stdlib.h>

#define NUM_ROWS 5
#define NUM_COLUMNS 3

int main(void)
{
	//variable declarations
	int* sArray[NUM_ROWS];
	int s, k;

	//code
	printf("\n\n");
	printf("Memory Allocation to 2D Integer Array\n\n");

	for (s = 0; s < NUM_ROWS; s++)
	{
		sArray[s] = (int*)malloc((NUM_COLUMNS - s) * sizeof(int));

		if (sArray[s] == NULL)
		{
			printf("Memory Allocation of row %d of 2D Integer Array failed! Exitting Now.\n", s);
			exit(0);
		}
		else
		{
			printf("Memory Allocation of row %d of 2D Integer Array succeeded!\n", s);
		}

	}

	
	for (s = 0; s < 5; s++)
	{
		for (k = 0; k < (NUM_COLUMNS -s); k++)
		{
			sArray[s][k] = (s * 1) + (k * 1);
		}
	}

	for (s = 0; s < 3; s++)
	{
		for (k = 0; k < (NUM_COLUMNS - s); k++)
		{
			printf("sArray[%d][%d] = %d \t at Address %p\n", s, k, sArray[s][k], &sArray[s][k]);
		}
		printf("\n\n");
	}

	for (s = (NUM_ROWS - 1); s >= 0; s++)
	{
		if (sArray[s])
		{
			free(sArray[s]);
			sArray[s] = NULL;
			printf("Memory Allocated to row %d has been successfully freed!\n\n", s);
		}
	}

	
	getch();
	return(0);

}
