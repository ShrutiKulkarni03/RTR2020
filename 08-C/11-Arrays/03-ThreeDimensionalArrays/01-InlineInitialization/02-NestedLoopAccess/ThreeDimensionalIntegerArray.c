#include<stdio.h>

int main(void)
{
	int  sArray[4][3][2] = { { { 1, 2 }, { 3, 4 }, { 5, 6 } },
							 { { 2, 4 }, { 6, 8 }, { 10, 12 } },
							 { { 3, 6 }, { 9, 12 }, { 15, 18 } },
							 { { 4, 8 }, { 12, 16 }, { 20, 24 } } };

	int int_size;
	int sArraySize, sArray_num_elements, sArray_width, sArray_height, sArray_depth;
	int a, b, c;

	int_size = sizeof(int);
	sArraySize = sizeof(sArray);
	printf("\n\nSize of 3D Integer Array is %d\n", sArraySize);

	sArray_width = sArraySize / sizeof(sArray[0]);
	printf("Number of Rows (width) in 3D Integer Array is %d\n", sArray_width);

	sArray_height = sizeof(sArray[0]) / sizeof(sArray[0][0]);
	printf("Number of Columns (height) in 3D Integer Array is %d\n", sArray_height);

	sArray_depth = sizeof(sArray[0][0]) / int_size;
	printf("Depth in 3D Integer Array is %d\n", sArray_depth);

	sArray_num_elements = sArray_width * sArray_height * sArray_depth;
	printf("Number of Elements in 3D Integer Array is %d\n", sArray_num_elements);

	printf("\n\nElements in 3D Integer Array are\n");

	for (a = 0; a < sArray_width; a++)
	{
		printf("\nROW %d\n", (a + 1));
		for (b = 0; b < sArray_height; b++)
		{
			printf("COLUMN %d\n", b + 1);
			for (c = 0; c < sArray_depth; c++)
			{
				printf("sArray[%d][%d][%d] = %d\n", a, b, c, sArray[a][b][c]);
			}
		}
	}

	getch();
	return(0);

}
