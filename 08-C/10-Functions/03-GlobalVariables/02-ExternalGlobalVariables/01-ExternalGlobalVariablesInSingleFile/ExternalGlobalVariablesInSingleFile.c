#include<stdio.h>

int main(void)
{
	void change_count(void);

	extern int global_count;

	printf("\nValue of global_count before change_count is %d\n", global_count);

	change_count();

	printf("\nValue of global_count after change_count is %d\n", global_count);

	return(0);

}

int global_count = 1;

void change_count(void)
{
	global_count = 10;
	printf("\nValue of global_count in change_count is %d\n", global_count);
}
