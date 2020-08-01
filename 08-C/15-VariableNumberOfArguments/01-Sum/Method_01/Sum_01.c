#include<stdio.h>
#include<stdarg.h>

int main(void)
{
	// function prototypes
	int CalculateSum(int, ...);

	// variable declarations
	int ans;

	// code
	
	ans = CalculateSum(7, 14, 21, 28, 35, 42);
	printf("\n\nAnswer = %d\n\n", ans);

	ans = CalculateSum(1, 2, 3, 4, 5, 6, 7, 8, 9);
	printf("Answer = %d\n\n", ans);

	ans = CalculateSum(0);
	printf("Answer = %d\n\n", ans);

	getch();
	return(0);

}

int CalculateSum(int num, ...) // Variadic
{
	// variable declarations
	int sum_total = 0;
	int s;
	va_list num_list;

	// code
	va_start(num_list, num);

	while (num)
	{
		s = va_arg(num_list, int);
		sum_total = sum_total + s;
		num--;
	}

	va_end(num_list);

	return(sum_total);
}
