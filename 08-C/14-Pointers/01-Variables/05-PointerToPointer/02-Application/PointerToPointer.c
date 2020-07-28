#include<stdio.h>

int main(void)
{
	//variable declaration
	int num;
	int* ptr = NULL;
	int** pptr = NULL;

	//code
	num = 3;

	printf("\n\nBefore ptr = &num\n\n");
	printf("Value of 'num' = %d\n", num);
	printf("Address of 'num' = %p\n", &num);
	printf("Value at address of 'num' = %d\n", *(&num));

	ptr = &num;

	printf("\n\nAfter ptr = &num\n\n");
	printf("Value of 'num' = %d\n", num);
	printf("Address of 'num' = %p\n", ptr);
	printf("Value at address of 'num' = %d\n", *ptr);

	pptr = &ptr;

	printf("\n\nAfter pptr = &ptr\n\n");
	printf("Value of 'num' = %d\n", num);
	printf("Address of 'num' (ptr) = %p\n", ptr);
	printf("Address of 'ptr' (pptr) = %p\n", pptr);
	printf("Value at address of 'ptr' (*pptr) = %p\n", *pptr);
	printf("Value at address of 'num' (*ptr) = %d\n", **pptr);

	getch();
	return(0);

}
