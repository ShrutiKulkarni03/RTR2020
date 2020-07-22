#include<stdio.h>

int main(void)
{
	//variable declaration
	float s_num;
	float *ptr = NULL;

	//code
	s_num = 4.6348f;

	printf("\n\nBefore ptr = &num\n\n");
	printf("Value of 'num' = %f\n\n", s_num);
	printf("Address of 'num' = %p\n\n", &s_num);
	printf("Value at Address of 'num' = %f\n\n", *(&s_num));

	ptr = &s_num;

	printf("\n\nAfter ptr = &num\n\n");
	printf("Value of 'num' = %f\n\n", s_num);
	printf("Address of 'num' = %p\n\n", ptr);
	printf("Value at Address of 'num' = %f\n\n", *ptr);

	getch();
	return(0);

}
