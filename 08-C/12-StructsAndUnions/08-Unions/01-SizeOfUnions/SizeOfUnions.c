#include <stdio.h>

struct S_Struct
{
	int si;
	float sf;
	double sd;
	char sc;
};
union U_Union
{
	int si;
	float sf;
	double sd;
	char sc;
};
int main(void)
{
	//variable declaration
	struct S_Struct s;
	union U_Union u;

	//code
	printf("\n\nSize Of MyStruct = %lu\n", sizeof(s));
	printf("Size Of MyUnion = %lu\n", sizeof(u));

	getch();
	return(0);
}

