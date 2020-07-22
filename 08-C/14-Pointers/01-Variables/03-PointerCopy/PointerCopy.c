#include<stdio.h>

int main(void)
{
	//variable declaration
	int num;
	int* ptr = NULL;
	int* copy_ptr = NULL;

	//code
	num = 3;
	ptr = &num;

	printf("\n\nBefore copy_ptr = ptr\n\n");
	printf("num \t= %d\n", num);
	printf("&num \t= %p\n", &num);
	printf("*(&num) = %d\n", *(&num));
	printf("ptr \t= %p\n", ptr);
	printf("*ptr \t= %d\n", *ptr);

	copy_ptr = ptr;

	printf("\n\nAfter copy_ptr = ptr\n\n");
	printf("num \t= %d\n", num);
	printf("&num \t= %p\n", &num);
	printf("*(&num) = %d\n", *(&num));
	printf("ptr \t= %p\n", ptr);
	printf("*ptr \t= %d\n", *ptr);
	printf("copy_ptr  = %p\n", copy_ptr);
	printf("*copy_ptr = %d\n\n", *copy_ptr);

	getch();
	return(0);

}

