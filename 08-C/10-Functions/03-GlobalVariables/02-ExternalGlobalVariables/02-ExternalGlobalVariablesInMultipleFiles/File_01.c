#include<stdio.h>

extern int global_count;

void change_count_two(void)
{
	global_count += 1;
	printf("Change_Count_Two() : Value Of global_count in File_01 = %d\n", global_count);
}
