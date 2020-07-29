#include<stdio.h>
#include<stdlib.h>

#define NUM_ROWS 5
#define NUM_COLUMNS 3

int main(void)
{
	//variable declaration
	int sArray[NUM_ROWS][NUM_COLUMNS];
	int s, k;

	int* ptr_sArray_row = NULL;

	//code
	for (s = 0; s < NUM_ROWS; s++)
	{
		ptr_sArray_row = sArray[s];

		for (k = 0; k < NUM_COLUMNS; k++)
			*(ptr_sArray_row + k) = (s + 1) * (k + 1);
	}

	printf("\n\n2D Integer Array Elements along with Addresses : \n\n");
	for (s = 0; s < NUM_ROWS; s++)
	{
		ptr_sArray_row = sArray[s];
		for (k = 0; k < NUM_COLUMNS; k++)
		{
			printf("*(ptr_sArray_row + %d) = %d \t at Address (ptr_sArray_row + k) : %p\n", k, *(ptr_sArray_row + k), (ptr_sArray_row + k));
		}
		printf("\n\n");
	}

	getch();
	return(0);

}
