#include<stdio.h>

int global_count = 0;

int main(void)
{
	void change_count_one(void);
	void change_count_two(void);
	void change_count_three(void);

	change_count_one();
	change_count_two();
	change_count_three();

	return(0);
}

void change_count_one(void)
{
	global_count = +1;
	printf("\n\nGlobal Count is %d\n", global_count);

}
