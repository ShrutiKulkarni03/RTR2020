#include<stdio.h>

int main(int argc, char* argv[], char* envp[])
{
	void function_country();

	function_country();
	return(0);

}

void function_country(void)
{
	void function_ofAMC(void);

	function_ofAMC();

	printf("\n\nI Live in India!\n");

}

void function_ofAMC(void)
{
	void function_surname(void);

	function_surname();

	printf("\n\nof AMC\n");

}

void function_surname(void)
{
	void function_middlename(void);

	function_middlename();

	printf("\n\nKulkarni\n");

}

void function_middlename(void)
{
	void function_firstname(void);

	function_firstname();

	printf("\n\nParesh\n");

}

void function_firstname(void)
{
	void function_is(void);

	function_is();

	printf("\n\nShruti\n");

}

void function_is(void)
{
	void function_name(void);

	function_name();

	printf("\n\nIs\n");

}

void function_name(void)
{
	void function_my(void);

	function_my();

	printf("\n\nName\n");

}

void function_my(void)
{
	printf("\n\nMy\n");
}

