#include<stdio.h>

extern int global_count;

void change_count_three(void)
{
	extern int global_count;
	global_count += 1;
	printf("Change_Count_Three() : Value Of global_count in File_02 = %d\n\n", global_count);
}
