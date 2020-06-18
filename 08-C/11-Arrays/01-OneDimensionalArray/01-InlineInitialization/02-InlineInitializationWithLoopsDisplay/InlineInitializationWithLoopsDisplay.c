#include<stdio.h>

int main(void)
{
	int sArray[] = { 3, 6, 9, 12, 15, 18, 21, 24, 27, 30 };
	int int_size, sArray_size, sArray_num_elements;

	float kArray[] = { 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9, 11.0 };
	int float_size, kArray_size, kArray_num_elements;

	char pArray[] = { 'A', 'S', 'T', 'R', 'O', 'M', 'E', 'D', 'I', 'C', 'O', 'M', 'P' };
	int char_size, pArray_size, pArray_num_elements;

	int a;

	printf("\n\nIn-line Initialization & Loop of Integer type Array\n");

	int_size = sizeof(int);
	sArray_size = sizeof(sArray);
	sArray_num_elements = sArray_size / int_size;

	for (a = 0; a < sArray_num_elements; a++)
	{
		printf("sArray[%d] (Elememt %d) is %d\n", a, (a + 1), sArray[a]);
	}
	printf("\nSize of datatype 'Integer' is %d\n", int_size);
	printf("Number of Elements in Integer type Array are %d Elements\n", sArray_num_elements);
	printf("Size of Integer type Array (%d Elements * %d Bytes) is %d Bytes\n", sArray_num_elements, int_size, sArray_size);


	
	printf("\n\nIn-line Initialization & Loop of Float type Array\n");

	float_size = sizeof(float);
	kArray_size = sizeof(kArray);
	kArray_num_elements = kArray_size / float_size;

	a = 0;
	while (a < kArray_num_elements)
	{
		printf("kArray[%d] (Elememt %d) is %f\n", a, (a + 1), kArray[a]);
		a++;
	}
	printf("\nSize of datatype 'Float' is %d\n", float_size);
	printf("Number of Elements in Float type Array are %d Elements\n", kArray_num_elements);
	printf("Size of Float type Array (%d Elements * %d Bytes) is %d Bytes\n", kArray_num_elements, float_size, kArray_size);



	printf("\n\nIn-line Initialization & Loop of Character type Array\n");

	char_size = sizeof(char);
	pArray_size = sizeof(pArray);
	pArray_num_elements = pArray_size / char_size;

	a = 0;
	do
	{
		printf("pArray[%d] (Elememt %d) is %c\n", a, (a + 1), pArray[a]);
		a++;
	} while (a < pArray_num_elements);

	printf("\nSize of datatype 'Character' is %d\n", char_size);
	printf("Number of Elements in Character type Array are %d Elements\n", pArray_num_elements);
	printf("Size of Character type Array (%d Elements * %d Bytes) is %d Bytes\n", pArray_num_elements, char_size, pArray_size);

	getch();
	return(0);

}
