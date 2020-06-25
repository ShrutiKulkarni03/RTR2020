#include<stdio.h>

int main(void)
{
	int sArray1[5];
	int sArray2[5][3];
	int sArray3[50][50][5];

	int num_rows_2D;
	int num_columns_2D;

	int num_rows_3D;
	int num_columns_3D;
	int depth_3D;

	//1D
	printf("\n\nSize of 1D Integer Array is %lu\n", sizeof(sArray1));

	printf("Number of elements in 1D Integer  Array sArray_1D = %lu\n", (sizeof(sArray1) / sizeof(int)));

	//2D
	printf("\n\nSize of 2D Integer Array is %lu\n", sizeof(sArray2));

	printf("Number of rows in 2D Integer Array sArray_2D = %lu\n", (sizeof(sArray2) / sizeof(sArray2[0])));
	num_rows_2D = (sizeof(sArray2) / sizeof(sArray2[0]));

	printf("Number of columns in each row in 2D Integer  Array sArray_2D = %lu\n", (sizeof(sArray2[0]) / sizeof(sArray2[0][0])));
	num_columns_2D = (sizeof(sArray2[0]) / sizeof(sArray2[0][0]));

	printf("Number of total elements in 2D Integer  Array sArray_2D = %d\n", (num_rows_2D*num_columns_2D));


	//3D
	printf("\n\nSize of 3D Integer Array sArray3 = %lu\n", sizeof(sArray3));

	printf("Number of Rows (width) in 3D Integer Array is %d\n", (sizeof(sArray3)/sizeof(sArray3[0])));
	num_rows_3D = (sizeof(sArray3) / sizeof(sArray3[0]));

	printf("Number of Columns (height) in 3D Integer Array is %lu\n", (sizeof(sArray3[0])/sizeof(sArray3[0][0])));
	num_columns_3D = (sizeof(sArray3[0]) / sizeof(sArray3[0][0]));

	printf("Number of elements (depth) in 3D Integer Array is %lu\n", (sizeof(sArray3[0][0]) / sizeof(sArray3[0][0][0])));
	depth_3D = (sizeof(sArray3[0][0]) / sizeof(sArray3[0][0][0]));

	printf("Number of total Elements in 3D Integer Array is %d\n", (num_rows_3D * num_columns_3D * depth_3D));


	getch();
	return(0);

}
