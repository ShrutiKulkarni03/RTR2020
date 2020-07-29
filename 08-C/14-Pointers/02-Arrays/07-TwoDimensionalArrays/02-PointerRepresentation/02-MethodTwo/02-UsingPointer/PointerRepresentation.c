#include<stdio.h>
#include<stdlib.h>

#define NUM_ROWS 5
#define NUM_COLUMNS 3

int main(void)
{
	//variable declaration
	int** ptr_sArray = NULL;
	int s, k;

	//code
	ptr_sArray = (int**)malloc(NUM_ROWS * sizeof(int*));

	if (ptr_sArray == NULL)
	{
		printf("\n\nMemory Allocation to the 1D Array of Base Addresses of %d Rows Failed! Exitting Now.\n\n", NUM_ROWS);
		exit(0);
	}
	else
	{
		printf("\n\nMemory Allocation to the 1D Array of Base Addresses of %d Rows Succeeded!\n\n", NUM_ROWS);
	}

	for (s = 0; s < NUM_ROWS; s++)
	{
		ptr_sArray[s] = (int*)malloc(NUM_COLUMNS * sizeof(int));

		if (ptr_sArray == NULL)
		{
			printf("Memory Allocation to the Columns of Row %d Failed! Exitting Now.\n\n", s);
			exit(0);
		}
		else
		{
			printf("Memory Allocation to the Columns of Row %d Succeeded!\n\n", s);
		}
	}

	for (s = 0; s < NUM_ROWS; s++)
	{
		for (k = 0; k < NUM_COLUMNS; k++)
			*(*(ptr_sArray + s) + k) = (s + 1) * (k + 1);
	}

	//displaying values

	printf("\n\n2D Integer Array Elements along with Addresses : \n\n");
	for (s = 0; s < NUM_ROWS; s++)
	{
		for (k = 0; k < NUM_COLUMNS; k++)
		{
			printf("ptr_sArray_row[%d][%d] = %d \t at Address &ptr_sArray_row[%d][%d] : %p\n", s, k, ptr_sArray[s][k], s, k, &ptr_sArray[s][k]);
		}
		printf("\n\n");
	}

	//freeing memory

	for (s = (NUM_ROWS - 1); s >= 0; s--)
	{
		if (*(ptr_sArray + s))
		{
			free(*(ptr_sArray + s));
			*(ptr_sArray + s) = NULL;
			printf("Memory Allocated to Row %d Freed!\n", s);
		}
	}

	if (ptr_sArray)
	{
		free(ptr_sArray);
		ptr_sArray = NULL;
		printf("Memory Allocated to ptr_sArray Freed!\n");
	}

	getch();
	return(0);

}
