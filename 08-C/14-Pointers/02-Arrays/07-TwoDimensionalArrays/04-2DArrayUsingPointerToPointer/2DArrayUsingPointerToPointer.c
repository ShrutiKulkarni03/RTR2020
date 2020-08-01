#include<stdio.h>
#include<stdlib.h>

int main(void)
{
	//variable declaration
	int** ptr_sArray = NULL;
	int s, k, num_rows, num_columns;

	//code
	printf("\n\nEnter Number of Rows : ");
	scanf("%d", &num_rows);

	printf("\nEnter Number of Columns : ");
	scanf("%d", &num_columns);

	printf("\nMemory Allocation to 2D Integer Array\n\n");
	ptr_sArray = (int**)malloc(num_rows * sizeof(int*));
	if (ptr_sArray == NULL)
	{
		printf("Failed to Allocate Memory to %d rows of 2D Integer Array!Exitting Now.\n\n", num_rows);
		exit(0);
	}
	else
	{
		printf("Memory Allocation to %d rows of 2D Integer Array Succeeded!\n", num_rows);
	}

	for (s = 0; s < num_rows; s++)
	{
		ptr_sArray[s] = (int*)malloc(num_columns * sizeof(int));

		if (ptr_sArray[s] == NULL)
		{
			printf("Failed to Allocate Memory to columns of %d row of 2D Integer Array!Exitting Now.\n\n", s);
			exit(0);
		}
		else
		{
			printf("Memory Allocation to columns of %d row of 2D Integer Array Succeeded!\n", s);
		}
		
	}
	printf("\n");

	for (s = 0; s < num_rows; s++)
	{
		for (k = 0; k < num_columns; k++)
		{
			ptr_sArray[s][k] = (s * 1) + (k * 1);
		}
	}

	//Displaying
	for (s = 0; s < num_rows; s++)
	{
		for (k = 0; k < num_columns; k++)
		{
			printf("ptr_sArray[%d][%d] = %d \t at Address %p\n", s, k, ptr_sArray[s][k], &ptr_sArray[s][k]);
		}
		printf("\n");
	}

	//freeing
	for (s = (num_rows - 1); s >= 0; s--)
	{
		if (ptr_sArray[s])
		{
			free(ptr_sArray[s]);
			ptr_sArray[s] = NULL;
			printf("Memory Allocated to row %d has been successfully freed!\n", s);
		}
	}

	if (ptr_sArray)
	{
		free(ptr_sArray);
		ptr_sArray = NULL;
		printf("Memory Allocated to ptr_sArray has been successfully freed!\n");
	}
	
	getch();
	return(0);

}
