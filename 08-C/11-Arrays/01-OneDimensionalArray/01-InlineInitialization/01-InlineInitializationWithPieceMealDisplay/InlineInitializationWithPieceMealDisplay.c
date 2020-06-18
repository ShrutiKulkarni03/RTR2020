#include<stdio.h>

int main(void)
{
	int sArray[] = { 3, 6, 9, 12, 15, 18, 21, 24, 27, 30 };
	int int_size, sArray_size, sArray_num_elements;

	float kArray[] = { 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9, 11.0 };
	int float_size, kArray_size, kArray_num_elements;

	char pArray[] = { 'A', 'S', 'T', 'R', 'O', 'M', 'E', 'D', 'I', 'C', 'O', 'M', 'P' };
	int char_size, pArray_size, pArray_num_elements;

	printf("\n\nIn-line and Piece-meal Initialization of Integer Type Array\n");
	printf("sArray[0] (1st Element) is %d\n", sArray[0]);
	printf("sArray[1] (2nd Element) is %d\n", sArray[1]);
	printf("sArray[2] (3rd Element) is %d\n", sArray[2]);
	printf("sArray[3] (4th Element) is %d\n", sArray[3]);
	printf("sArray[4] (5th Element) is %d\n", sArray[4]);
	printf("sArray[5] (6th Element) is %d\n", sArray[5]);
	printf("sArray[6] (7th Element) is %d\n", sArray[6]);
	printf("sArray[7] (8th Element) is %d\n", sArray[7]);
	printf("sArray[8] (9th Element) is %d\n", sArray[8]);
	printf("sArray[9] (10th Element) is %d\n", sArray[9]);

	int_size = sizeof(int);
	sArray_size = sizeof(sArray);
	sArray_num_elements = sArray_size / int_size;

	printf("\nSize of datatype int is %d\n", int_size);
	printf("Number of Elements in Integer Type Array are %d Elements\n", sArray_num_elements);
	printf("Size of sArray[] (%d elements * %d bytes) is %d bytes\n\n", sArray_num_elements, int_size, sArray_size);



	printf("\n\nIn-line and Piece-meal Initialization of Float Type Array\n");
	printf("Akrray[0] (1st Element) is %f\n", kArray[0]);
	printf("kArray[1] (2nd Element) is %f\n", kArray[1]);
	printf("kArray[2] (3rd Element) is %f\n", kArray[2]);
	printf("kArray[3] (4th Element) is %f\n", kArray[3]);
	printf("kArray[4] (5th Element) is %f\n", kArray[4]);
	printf("kArray[5] (6th Element) is %f\n", kArray[5]);
	printf("kArray[6] (7th Element) is %f\n", kArray[6]);
	printf("kArray[7] (8th Element) is %f\n", kArray[7]);
	printf("kArray[8] (9th Element) is %f\n", kArray[8]);
	printf("kArray[9] (10th Element) is %f\n", kArray[9]);

	float_size = sizeof(float);
	kArray_size = sizeof(kArray);
	kArray_num_elements = kArray_size / float_size;

	printf("\nSize of datatype float is %d\n", float_size);
	printf("Number of Elements in Float Type Array are %d Elements\n", kArray_num_elements);
	printf("Size of kArray[] (%d elements * %d bytes) is %d bytes\n\n", kArray_num_elements, float_size, kArray_size);



	printf("\n\nIn-line and Piece-meal Initialization of Char Type Array\n");
	printf("pArray[0] (1st Element) is %c\n", pArray[0]);
	printf("pArray[1] (2nd Element) is %c\n", pArray[1]);
	printf("pArray[2] (3rd Element) is %c\n", pArray[2]);
	printf("pArray[3] (4th Element) is %c\n", pArray[3]);
	printf("pArray[4] (5th Element) is %c\n", pArray[4]);
	printf("pArray[5] (6th Element) is %c\n", pArray[5]);
	printf("pArray[6] (7th Element) is %c\n", pArray[6]);
	printf("pArray[7] (8th Element) is %c\n", pArray[7]);
	printf("pArray[8] (9th Element) is %c\n", pArray[8]);
	printf("pArray[9] (10th Element) is %c\n", pArray[9]);
	printf("pArray[10] (11th Element) is %c\n", pArray[10]);
	printf("pArray[11] (12th Element) is %c\n", pArray[11]);
	printf("pArray[12] (13th Element) is %c\n", pArray[12]);

	char_size = sizeof(char);
	pArray_size = sizeof(pArray);
	pArray_num_elements = pArray_size / char_size;

	printf("\nSize of datatype int is %d\n", char_size);
	printf("Number of Elements in Integer Type Array are %d Elements\n", pArray_num_elements);
	printf("Size of sArray[] (%d elements * %d bytes) is %d bytes\n\n", pArray_num_elements, char_size, pArray_size);

	getch();
	return(0);

}
