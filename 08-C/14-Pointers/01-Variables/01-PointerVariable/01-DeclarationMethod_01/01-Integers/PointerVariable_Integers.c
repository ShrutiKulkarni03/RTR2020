#include<stdio.h>

int main(void)
{
	//variable declaration
	int s_num;
	int* ptr = NULL;

	//code
	s_num = 3;

	printf("\n\nBefore ptr = &num\n\n");
	printf("Value of 'num' = %d\n\n", s_num);
	printf("Address of 'num' = %p\n\n", &s_num);
	printf("Value at Address of 'num' = %d\n\n", *(&s_num));

	ptr = &s_num;

	printf("\n\nAfter ptr = &num\n\n");
	printf("Value of 'num' = %d\n\n", s_num);
	printf("Address of 'num' = %p\n\n", ptr);
	printf("Value at Address of 'num' = %d\n\n", *ptr);

	getch();
	return(0);

}
