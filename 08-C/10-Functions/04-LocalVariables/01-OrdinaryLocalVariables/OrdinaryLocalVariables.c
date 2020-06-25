#include<stdio.h>

int main(void)
{
	int sk_s = 10;

	void change_count(void);

	printf("\n\nValue of 'S' is %d\n", sk_s);

	change_count();
	change_count();
	change_count();
	change_count();
	change_count();
	change_count();

	return(0);

}

void change_count(void)
{
	int local_count = 0;

	local_count += 1;

	printf("Local Count is %d\n", local_count);
}
