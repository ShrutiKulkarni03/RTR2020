#include<stdio.h>

int main(void)
{
	int sArray[5][3] = { {1,2,3}, {4,5,6}, {7,8,9}, {10,11,12}, {13,14,15} };
	int int_size, sArraySize, sArray_num_elements, sArray_num_rows, sArray_num_columns;
	int a, b;

	int_size = sizeof(int);
	sArraySize = sizeof(sArray);

	printf("\n\nSize of 2D Integer Array is %d\n", sArraySize);

	sArray_num_rows = sArraySize / sizeof(sArray[0]);
	printf("Number of rows in 2D Integer Array are %d\n", sArray_num_rows);

	sArray_num_columns = sizeof(sArray[0]) / int_size;
	printf("Number of columns in 2D Integer Array are %d\n", sArray_num_columns);

	sArray_num_elements = sArray_num_rows / sArray_num_columns;
	printf("Number of elements in 2D Integer Array are %d\n", sArray_num_elements);

	printf("\nElements in 2D Array are:\n");

	for (a = 0; a < sArray_num_rows; a++)
	{
		printf("\nROW %d\n", (a + 1));
		for (b = 0; b < sArray_num_columns; b++)
		{
			printf("sArray[%d][%d] is %d\n", a, b, sArray[a][b]);
		}
	}

	getch();
	return(0);

}
