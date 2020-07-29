#include<stdio.h>
#include<stdlib.h>

#define INT_SIZE sizeof(int)
#define FLOAT_SIZE sizeof(float)
#define DOUBLE_SIZE sizeof(double)
#define CHAR_SIZE sizeof(char)

int main(void)
{
	//variable declaration
	int* ptr_siArray = NULL;
	unsigned int intArrayLength = 0;

	float* ptr_sfArray = NULL;
	unsigned int floatArrayLength = 0;

	double* ptr_sdArray = NULL;
	unsigned int doubleArrayLength = 0;

	char* ptr_scArray = NULL;
	unsigned int charArrayLength = 0;

	int s;

	//code
	
	//int

	printf("\n\nEnter the number of elements you want in the Integer Array : ");
	scanf("%u", &intArrayLength);

	ptr_siArray = (int*)malloc(INT_SIZE * intArrayLength);
	if (ptr_siArray == NULL)
	{
		printf("\n\nMemory Allocation for Integer Array Failed! Exitting Now.\n\n");
		exit(0);
	}
	else
	{
		printf("\nMemory Allocation for Integer Array Successful!\n\n");
	}

	printf("Enter the %d Integer Elements to fill up the Integer Array : \n", intArrayLength);
	for (s = 0; s < intArrayLength; s++)
	{
		scanf("%d", (ptr_siArray + s));
	}

	//float

	printf("\n\nEnter the number of elements you want in the Float Array : ");
	scanf("%u", &floatArrayLength);

	ptr_sfArray = (float*)malloc(FLOAT_SIZE * floatArrayLength);
	if (ptr_sfArray == NULL)
	{
		printf("\n\nMemory Allocation for Float Array Failed! Exitting Now.\n\n");
		exit(0);
	}
	else
	{
		printf("\nMemory Allocation for Float Array Successful!\n\n");
	}

	printf("Enter the %d Floating-Point Elements to fill up the Float Array : \n", floatArrayLength);
	for (s = 0; s < floatArrayLength; s++)
	{
		scanf("%f", (ptr_sfArray + s));
	}

	//double

	printf("\n\nEnter the number of elements you want in the Double Array : ");
	scanf("%u", &doubleArrayLength);

	ptr_sdArray = (double*)malloc(DOUBLE_SIZE * doubleArrayLength);
	if (ptr_sdArray == NULL)
	{
		printf("\n\nMemory Allocation for Double Array Failed! Exitting Now.\n\n");
		exit(0);
	}
	else
	{
		printf("\nMemory Allocation for Double Array Successful!\n\n");
	}

	printf("Enter the %d Double Elements to fill up the Double Array : \n", doubleArrayLength);
	for (s = 0; s < doubleArrayLength; s++)
	{
		scanf("%lf", (ptr_sdArray + s));
	}

	//char

	printf("\n\nEnter the number of elements you want in the Character Array : ");
	scanf("%u", &charArrayLength);

	ptr_scArray = (char*)malloc(CHAR_SIZE * charArrayLength);
	if (ptr_scArray == NULL)
	{
		printf("\n\nMemory Allocation for Character Array Failed! Exitting Now.\n\n");
		exit(0);
	}
	else
	{
		printf("\nMemory Allocation for Character Array Successful!\n\n");
	}

	printf("Enter the %d Character Elements to fill up the Character Array : \n", charArrayLength);
	for (s = 0; s < charArrayLength; s++)
	{
		*(ptr_scArray + s) = getch();
		printf("%c\n", *(ptr_scArray + s));
	}

	//Display of Array

	//int
	printf("\n\nThe Integer Array entered by you of %d Elements is : \n\n", intArrayLength);
	for (s = 0; s < intArrayLength; s++)
		printf("%d \t at Address : %p\n", *(ptr_siArray + s), (ptr_siArray + s));

	//float
	printf("\n\nThe Float Array entered by you of %d Elements is : \n\n", floatArrayLength);
	for (s = 0; s < floatArrayLength; s++)
		printf("%f \t at Address : %p\n", *(ptr_sfArray + s), (ptr_sfArray + s));

	//double
	printf("\n\nThe Double Array entered by you of %d Elements is : \n\n", doubleArrayLength);
	for (s = 0; s < doubleArrayLength; s++)
		printf("%lf \t at Address : %p\n", *(ptr_sdArray + s), (ptr_sdArray + s));

	//Char
	printf("\n\nThe Character Array entered by you of %d Elements is : \n\n", charArrayLength);
	for (s = 0; s < charArrayLength; s++)
		printf("%c \t at Address : %p\n", *(ptr_scArray + s), (ptr_scArray + s));

	//freeing memory

	if (ptr_scArray)
	{
		free(ptr_scArray);
		ptr_scArray = NULL;

		printf("\n\nMemory of Character Array Successfully Freed!\n\n");
	}

	if (ptr_sdArray)
	{
		free(ptr_sdArray);
		ptr_sdArray = NULL;

		printf("Memory of Double Array Successfully Freed!\n\n");
	}

	if (ptr_sfArray)
	{
		free(ptr_sfArray);
		ptr_sfArray = NULL;

		printf("Memory of Float Array Successfully Freed!\n\n");
	}

	if (ptr_siArray)
	{
		free(ptr_siArray);
		ptr_siArray = NULL;

		printf("Memory of Integer Array Successfully Freed!\n\n");
	}

	getch();
	return(0);

}
