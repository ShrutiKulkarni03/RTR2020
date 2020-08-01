#include<stdio.h>
#include<stdlib.h>

#define NUM_ROWS 5
#define NUM_COLUMNS_ONE 3
#define NUM_COLUMNS_TWO 4

int main(void)
{
	//variable declarations
	int* sArray[NUM_ROWS];
	int s, k;

	//code
	printf("\n\n");
	printf("First Memory Allocation to 2D Integer Array\n\n");

	for (s = 0; s < NUM_ROWS; s++)
	{
		sArray[s] = (int*)malloc(NUM_COLUMNS_ONE * sizeof(int));

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

	//Assigning values
	
	for (s = 0; s < NUM_ROWS; s++)
	{
		for (k = 0; k < NUM_COLUMNS_ONE; k++)
		{
			sArray[s][k] = (s + 1) * (k + 1);
		}
	}

	
	//Display
	printf("\n\nDisplaying 2D Array : \n\n");
	for (s = 0; s < NUM_ROWS; s++)
	{
		for (k = 0; k < NUM_COLUMNS_ONE; k++)
		{
			printf("sArray[%d][%d] = %d\n", s, k, sArray[s][k]);
		}
		printf("\n");
	}
	printf("\n");

	//freeing memory
	for (s = (NUM_ROWS - 1); s >= 0; s--)
	{
		free(sArray[s]);
		sArray[s] = NULL;

		printf("Memory Allocated to row %d of 2D Integer Array has been successfully freed!\n", s);
	}

	//TWO (Allocating Memory for an Array of 8 Integer per row)
	printf("\n\nSecond Memory Allocation to 2D Integer Array\n\n");
	for (s = 0; s < NUM_ROWS; s++)
	{
		sArray[s] = (int*)malloc(NUM_COLUMNS_TWO * sizeof(int));
		if (sArray[s] == NULL)
		{
			printf("Failed to Allocate Memory to row %d of 2D Integer Array! Exitting Now.\n", s);
			exit(0);
		}
		else
		{
			printf("Memory Allocation to row %d of 2D Integer Array Succeeded!\n", s);
		}
	}

	//assigning values to 2D Array
	for (s = 0; s < NUM_ROWS; s++)
	{
		for (k = 0; k < NUM_COLUMNS_TWO; k++)
		{
			sArray[s][k] = (s + 1) * (k + 1);
		}
	}

	//displaying 2D Array
	printf("\n\nDisplaying 2D Array : \n\n");
	for (s = 0; s < NUM_COLUMNS_TWO; s++)
	{
		for (k = 0; k < NUM_COLUMNS_TWO; k++)
		{
			printf("sArray[%d][%d] = %d\n", s, k, sArray[s][k]);
		}
		printf("\n");
	}
	printf("\n");

	//freeing memory 
	for (s = (NUM_ROWS - 1); s >= 0; s--)
	{
		free(sArray[s]);
		sArray[s] = NULL;
		printf("Memory Allocation to row %d of 2D Integer Array has been Successfully freed!\n", s);
	}
	
	getch();
	return(0);

}
