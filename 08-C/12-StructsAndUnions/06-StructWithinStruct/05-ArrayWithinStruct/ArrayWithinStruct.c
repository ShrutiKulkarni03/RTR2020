#include<stdio.h>

struct MyNumber
{
	int num;
	int num_table[10];
};

struct NumTables
{
	struct MyNumber x;
	struct MyNumber y;
	struct MyNumber z;
};

int main(void)
{
	struct NumTables table;
	int s;

	table.x.num = 3;
	for (s = 0; s < 10; s++)
		table.x.num_table[s] = table.x.num * (s + 1);
	printf("\n\nTable of %d is \n\n", table.x.num);
	for (s = 0; s < 10; s++)
		printf("%d * %d = %d\n", table.x.num, (s + 1), table.x.num_table[s]);

	table.y.num = 4;
	for (s = 0; s < 10; s++)
		table.y.num_table[s] = table.y.num * (s + 1);
	printf("\n\nTable of %d is \n\n", table.y.num);
	for (s = 0; s < 10; s++)
		printf("%d * %d = %d\n", table.y.num, (s + 1), table.y.num_table[s]);

	table.z.num = 5;
	for (s = 0; s < 10; s++)
		table.z.num_table[s] = table.z.num * (s + 1);
	printf("\n\nTable of %d is \n\n", table.z.num);
	for (s = 0; s < 10; s++)
		printf("%d * %d = %d\n", table.z.num, (s + 1), table.z.num_table[s]);

	getch();
	return(0);

}
