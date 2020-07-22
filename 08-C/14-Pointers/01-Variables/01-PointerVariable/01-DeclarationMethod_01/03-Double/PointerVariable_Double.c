#include<stdio.h>

int main(void)
{
	//variable declaration
	double s_num;
	double* ptr = NULL;

	//code
	s_num = 8.64862405f;

	printf("\n\nBefore ptr = &num\n\n");
	printf("Value of 'num' = %lf\n\n", s_num);
	printf("Address of 'num' = %p\n\n", &s_num);
	printf("Value at Address of 'num' = %lf\n\n", *(&s_num));

	ptr = &s_num;

	printf("\n\nAfter ptr = &num\n\n");
	printf("Value of 'num' = %lf\n\n", s_num);
	printf("Address of 'num' = %p\n\n", ptr);
	printf("Value at Address of 'num' = %lf\n\n", *ptr);

	getch();
	return(0);

}
