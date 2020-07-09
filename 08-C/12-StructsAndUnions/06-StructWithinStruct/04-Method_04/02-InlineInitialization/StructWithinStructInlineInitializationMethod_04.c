#include<stdio.h>

int main(void)
{

	int l, b, area;

	struct Point
	{
		int s_x;
		int s_y;

	};

	struct Rect
	{
		struct Point pt1;
		struct Point pt2;
	};

	struct Rect rectangle = { {1,2},{3,4} };


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
