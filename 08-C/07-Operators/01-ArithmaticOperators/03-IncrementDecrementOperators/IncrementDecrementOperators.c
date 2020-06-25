#include<stdio.h>

int main(void)
{
	int a = 5, b = 0;

	printf("\n\n");
	printf("A = %d\n", a);
	printf("A = %d\n", a++);
	printf("A = %d\n", a);
	printf("A = %d\n", --a);
	printf("A = %d\n", a);
	printf("A = %d\n", ++a);
	printf("A = %d\n", a);
	printf("A = %d\n\n", a--);

	printf("B = %d\n", b);
	printf("B = %d\n", --b);
	printf("B = %d\n", b);
	printf("B = %d\n", b--);
	printf("B = %d\n", b);
	printf("B = %d\n", b++);
	printf("B = %d\n", b);
	printf("B = %d\n", ++b);

	getch();
	return(0);
}
