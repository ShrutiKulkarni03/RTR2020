#include<stdio.h>

int main(void)
{
	//variable declaration
	int num;
	int* ptr = NULL;
	int ans;

	//code
	num = 3;
	ptr = &num;

	printf("\n\n");
	printf("num = %d\n", num);
	printf("&num = %p\n", &num);
	printf("*(&num) = %d\n", *(&num));
	printf("ptr = %p\n", ptr);
	printf("*ptr = %d\n\n", *ptr);

	printf("Answer of (ptr + 10) = %p\n", (ptr + 10));

	printf("Answer of *(ptr + 10) = %d\n", *(ptr + 10));

	printf("Answer of (*ptr + 10) = %d\n", (*ptr + 10));

	++*ptr;

	printf("Answer of ++*ptr = %d\n", *ptr);

	*ptr++;

	printf("Answer of *ptr++ = %d\n", *ptr);

	ptr = &num;
	(*ptr)++;

	printf("Answer of (*ptr)++ = %d\n\n", *ptr);

	return(0);

}
