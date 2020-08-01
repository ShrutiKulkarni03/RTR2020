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

void FindNumber(int num_to_be_found, int num, ...) // Variadic
{
	// variable declarations
	int count = 0;
	int s;
	va_list num_list;

	// code
	va_start(num_list, num);

	while (num)
	{
		s = va_arg(num_list, int);
		if (s == num_to_be_found)
			count++;
		num--;
	}

	if (count == 0)
		printf("\n\nNumber %d Could Not Be Found !!!\n\n", num_to_be_found);

	else
		printf("\n\nNumber %d Found %d Times !!!\n\n", num_to_be_found, count);

	va_end(num_list);

}

