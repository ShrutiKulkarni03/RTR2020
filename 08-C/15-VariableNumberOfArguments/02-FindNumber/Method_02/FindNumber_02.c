#include<stdio.h>
#include<stdarg.h>

#define NUM_TO_BE_FOUND 3
#define NUM_ELEMENTS 10

int main(void)
{
	// function prototypes
	void FindNumber(int, int, ...);

	// variable declarations
	int ans;

	// code
	
	FindNumber(NUM_TO_BE_FOUND, NUM_ELEMENTS, 1, 2, 3, 4, 5, 6, 7, 8, 9, 3);

	getch();
	return(0);

}

void FindNumber(int num_to_be_found, int num, ...) // VARIADIC FUNCTION
{
	// function prototypes
	int va_FindNumber(int, int, va_list);

	// variable declarations
	int count = 0;
	int s;
	va_list num_list;

	// code
	va_start(num_list, num);

	count = va_FindNumber(num_to_be_found, num, num_list);

	if (count == 0)
		printf("\n\nNumber %d Could Not Be Found !!!\n\n", num_to_be_found);

	else
		printf("\n\nNumber %d Found %d Times !!!\n\n", num_to_be_found, count);

	va_end(num_list);

}

int va_FindNumber(int num_to_be_found, int num, va_list list)
{
	// variable declarations
	int count_of_num = 0;
	int s;

	// code
	while (num)
	{
		s = va_arg(list, int);
		if (s == num_to_be_found)
			count_of_num++;
		num--;
	}

	return(count_of_num);
}

