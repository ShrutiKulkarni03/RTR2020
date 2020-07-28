#include<stdio.h>

int main(void)
{
	//variable declaration
	double sArray[10];
	int si;

	//code
	for (si = 0; si < 10; si++)
		sArray[si] = (float)(si + 1) * 1.521995f;

	printf("\n\nElements of Double Array are : \n\n");
	for (si = 0; si < 10; si++)
		printf("sArray[%d] = %lf\n", si, sArray[si]);
	printf("\n");

	printf("Elements of the Double Array are : \n\n");
	for (si = 0; si < 10; si++)
		printf("sArray[%d] = %lf \t Address = %p\n", si, sArray[si], &sArray[si]);

	getch();
	return(0);

}
