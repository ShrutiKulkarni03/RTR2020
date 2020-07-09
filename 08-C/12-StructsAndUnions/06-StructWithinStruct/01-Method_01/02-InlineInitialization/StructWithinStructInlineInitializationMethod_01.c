#include<stdio.h>

struct Rect
{
	struct Point
	{
		int s_x;
		int s_y;

	}pt1, pt2;

}rectangle = { {10,20},{30,40} };


int main(void)
{

	int l, b, area;

	l = rectangle.pt2.s_y - rectangle.pt1.s_y;
	if (l < 0)
		l = l * -1;

	b = rectangle.pt2.s_x - rectangle.pt1.s_x;
	if (b < 0)
		b = b * -1;

	area = l * b;

	printf("\n\nLength Of Rectangle is %d\n", l);
	printf("Breadth Of Rectangle is %d\n", b);
	printf("Area Of Rectangle is %d\n", area);

	getch();
	return(0);

}
