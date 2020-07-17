#include<stdio.h>

struct MyNumber
{
	int num;
	int num_table[10];
};

struct NumTable
{
	struct MyNumber s;
};

int main(void)
{
	//variable declaration
	struct NumTable table[10];
	int p, k;

	//code
	for (p = 0; p < 3; p++)
	{
		table[p].s.num = (p + 1);
	}
	for (p = 0; p < 3; p++)
	{
		printf("\n\nTable of %d is \n\n", table[p].s.num);
		for (k = 0; k < 10; k++)
		{
			table[p].s.num_table[k] = table[p].s.num * (k + 1);
			printf("%d * %d = %d\n", table[p].s.num, (k + 1), table[p].s.num_table[k]);
		}
	}
	
	return(0);
}

