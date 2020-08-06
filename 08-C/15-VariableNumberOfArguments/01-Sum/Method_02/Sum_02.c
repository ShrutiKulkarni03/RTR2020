#include<stdio.h>
#include<stdarg.h>

int main(void)
{
	// function prototypes
	int CalculateSum(int, ...);

	// variable declarations
	int ans;

	// code
	
	ans = CalculateSum(7, 7, 14, 21, 28, 35, 42, 49);
	printf("\n\nAnswer = %d\n\n", ans);

	ans = CalculateSum(9, 1, 2, 3, 4, 5, 6, 7, 8, 9);
	printf("Answer = %d\n\n", ans);

	ans = CalculateSum(0);
	printf("Answer = %d\n\n", ans);

	getch();
	return(0);

}

int CalculateSum(int num, ...) // Variadic
{
	// function prototype
	int va_CalculateSum(int, va_list);

	// variable declarations
	int sum = 0;
	va_list num_list;

	// code
	va_start(num_list, num);
	sum = va_CalculateSum(num, num_list);
	va_end(num_list);

	return(sum);
}

int va_CalculateSum(int num, va_list list)
{
	// variable declaration
	int s;
	int sum_total = 0;

	// code
	while (num)
	{
		s = va_arg(list, int);
		sum_total = sum_total + s;
		num--;
	}

	return(sum_total);
}
