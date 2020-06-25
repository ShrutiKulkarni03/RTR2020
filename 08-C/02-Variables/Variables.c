#include<stdio.h>

int main(void)
{
	int i = 22;
	float f = 19.667f;
	double d = 3.101997;
	char c = 's';

	printf("\ni = %d\nf = %f\nd = %lf\nc = %c\n", i, f, d, c);

	i = 177;
	f = 70.66f;
	d = 15.021995;
	c = 'k';

	printf("\n\n");
	printf("i = %d\n", i);
	printf("f = %f\n", f);
	printf("d = %lf\n", d);
	printf("c = %c\n", c);

	getch();
	return(0);
}
