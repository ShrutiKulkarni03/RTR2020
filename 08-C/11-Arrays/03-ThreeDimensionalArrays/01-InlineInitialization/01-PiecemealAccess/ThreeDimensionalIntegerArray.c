#include<stdio.h>

int main(void)
{
	int  sArray[4][3][2] = { { { 1, 2 }, { 3, 4 }, { 5, 6 } },
							 { { 2, 4 }, { 6, 8 }, { 10, 12 } },
							 { { 3, 6 }, { 9, 12 }, { 15, 18 } },
							 { { 4, 8 }, { 12, 16 }, { 20, 24 } } };

	int int_size;
	int sArraySize, sArray_num_elements, sArray_width, sArray_height, sArray_depth;

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

	printf("\nROW 1\t");
	printf("COLUMN 1\n");
	printf("sArray[0][0][0] is %d\n", sArray[0][0][0]);
	printf("sArray[0][0][1] is %d\n", sArray[0][0][1]);
	
	printf("\nROW 1\t");
	printf("COLUMN 2\n");
	printf("sArray[0][1][0] is %d\n", sArray[0][1][0]);
	printf("sArray[0][1][1] is %d\n", sArray[0][1][1]);

	printf("\nROW 1\t");
	printf("COLUMN 3\n");
	printf("sArray[0][2][0] is %d\n", sArray[0][2][0]);
	printf("sArray[0][2][1] is %d\n", sArray[0][2][1]);

	printf("\nROW 2\t");
	printf("COLUMN 1\n");
	printf("sArray[1][0][0] is %d\n", sArray[1][0][0]);
	printf("sArray[1][0][1] is %d\n", sArray[1][0][1]);

	printf("\nROW 2\t");
	printf("COLUMN 2\n");
	printf("sArray[1][1][0] is %d\n", sArray[1][1][0]);
	printf("sArray[1][1][1] is %d\n", sArray[1][1][1]);

	printf("\nROW 2\t");
	printf("COLUMN 3\n");
	printf("sArray[1][2][0] is %d\n", sArray[1][2][0]);
	printf("sArray[1][2][1] is %d\n", sArray[1][2][1]);

	printf("\nROW 3\t");
	printf("COLUMN 1\n");
	printf("sArray[2][0][0] is %d\n", sArray[2][0][0]);
	printf("sArray[2][0][1] is %d\n", sArray[2][0][1]);

	printf("\nROW 3\t");
	printf("COLUMN 2\n");
	printf("sArray[2][1][0] is %d\n", sArray[2][1][0]);
	printf("sArray[2][1][1] is %d\n", sArray[2][1][1]);

	printf("\nROW 3\t");
	printf("COLUMN 3\n");
	printf("sArray[2][2][0] is %d\n", sArray[2][2][0]);
	printf("sArray[2][2][1] is %d\n", sArray[2][2][1]);

	printf("\nROW 4\t");
	printf("COLUMN 1\n");
	printf("sArray[3][0][0] is %d\n", sArray[3][0][0]);
	printf("sArray[3][0][1] is %d\n", sArray[3][0][1]);

	printf("\nROW 4\t");
	printf("COLUMN 2\n");
	printf("sArray[4][1][0] is %d\n", sArray[3][1][0]);
	printf("sArray[4][1][1] is %d\n", sArray[3][1][1]);

	printf("\nROW 4\t");
	printf("COLUMN 3\n");
	printf("sArray[4][2][0] is %d\n", sArray[3][2][0]);
	printf("sArray[4][2][1] is %d\n", sArray[3][2][1]);

	getch();
	return(0);

}
