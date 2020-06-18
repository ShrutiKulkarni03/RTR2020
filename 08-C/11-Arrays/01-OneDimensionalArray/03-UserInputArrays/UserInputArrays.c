#include<stdio.h>

#define IARRAY_NUM_ELEMENTS 5
#define FARRAY_NUM_ELEMENTS 3
#define CARRAY_NUM_ELEMENTS 7

int main(void)
{
	int iArray[IARRAY_NUM_ELEMENTS];
	float fArray[FARRAY_NUM_ELEMENTS];
	char cArray[CARRAY_NUM_ELEMENTS];
	int i, num;

	printf("\n\n");
	printf("Enter Elements for Integer Array : \n");
	for (i = 0; i < IARRAY_NUM_ELEMENTS; i++)
		scanf("%d", &iArray[i]);
	
	printf("\n\n");
	printf("Enter Elements for Floating-point Array : \n");
	for (i = 0; i < FARRAY_NUM_ELEMENTS; i++)
		scanf("%f", &fArray[i]);

	printf("\n\n");
	printf("Enter Elements for Character Array : \n");
	for (i = 0; i < CARRAY_NUM_ELEMENTS; i++)
	{
		cArray[i] = getch();
		printf("%c\n", cArray[i]);
	}
	

	printf("\n\nInteger Array Entered is :");
	for (i = 0; i < IARRAY_NUM_ELEMENTS; i++)
		printf("%d\n", iArray[i]);

	printf("\n\nFloating-point Array Entered is :");
	for (i = 0; i < FARRAY_NUM_ELEMENTS; i++)
		printf("%f\n", fArray[i]);

	printf("\n\nCharacter Array Entered is :");
	for (i = 0; i < CARRAY_NUM_ELEMENTS; i++)
		printf("%c\n", cArray[i]);

	getch();
	return(0);

}
