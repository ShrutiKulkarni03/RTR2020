#include<stdio.h>
#define MAX_STRING_LENGTH 200

int main(void)
{
	
	int sArray[3][5];

	int int_size, sArraySize, sArray_num_elements, sArray_num_rows, sArray_num_columns, sActual_num_char = 0;
	int a, b;

	int_size = sizeof(int);
	sArraySize = sizeof(sArray);

	printf("\n\nSize of 2D Integer Array is %d\n", sArraySize);

	sArray_num_rows = sArraySize / sizeof(sArray[0]);
	printf("Number of rows in 2D String Array are %d\n", sArray_num_rows);

	sArray_num_columns = sizeof(sArray[0]) / int_size;
	printf("Number of columns in 2D String Array are %d\n", sArray_num_columns);

	sArray_num_elements = sArray_num_rows * sArray_num_columns;
	printf("Number of elements in 2D String Array are %d\n", sArray_num_elements);

	printf("\nElements in 2D Array are\n");

	sArray[0][0] = 3;
	sArray[0][1] = 5;
	sArray[0][2] = 7;
	sArray[0][3] = 9;
	sArray[0][4] = 11;
	sArray[1][0] = 13;
	sArray[1][1] = 15;
	sArray[1][2] = 17;
	sArray[1][3] = 19;
	sArray[1][4] = 21;
	sArray[2][0] = 23;
	sArray[2][1] = 25;
	sArray[2][2] = 27;
	sArray[2][3] = 29;
	sArray[2][4] = 31;

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
