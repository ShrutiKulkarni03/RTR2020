#include<stdio.h>

int global_count = 0;

int main(void)
{
	//function declarations
	void change_count1(void);
	void change_count2(void);
	void change_count3(void);

	//code
	printf("\n\nmain() : Value of global_count = %d\n", global_count);

	//call
	change_count1();
	change_count2();
	change_count3();

	printf("\n\n");

	getch();
	return(0);
}

void change_count1(void)
{
	//code
	global_count = 100;
	printf("change_count1() : Value of global_count = %d\n", global_count);

}

void change_count2(void)
{
	global_count += 1;
	printf("change_count2() : Value of global_count = %d\n", global_count);
}

void change_count3(void)
{
	global_count += 10;
	printf("change_count3() : Value of global_count = %d\n", global_count);
}

