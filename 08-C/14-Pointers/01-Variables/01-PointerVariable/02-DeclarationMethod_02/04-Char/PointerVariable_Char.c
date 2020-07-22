#include<stdio.h>

int main(void)
{
	//variable declaration
	char s_num;
	char *ptr = NULL;

	//code
	s_num = 'S';

	printf("\n\nBefore ptr = &num\n\n");
	printf("Value of 'num' = %c\n\n", s_num);
	printf("Address of 'num' = %p\n\n", &s_num);
	printf("Value at Address of 'num' = %c\n\n", *(&s_num));

	ptr = &s_num;

	printf("\n\nAfter ptr = &num\n\n");
	printf("Value of 'num' = %c\n\n", s_num);
	printf("Address of 'num' = %p\n\n", ptr);
	printf("Value at Address of 'num' = %c\n\n", *ptr);

	getch();
	return(0);

}
