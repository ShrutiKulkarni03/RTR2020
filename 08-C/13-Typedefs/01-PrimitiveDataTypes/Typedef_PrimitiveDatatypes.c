#include<stdio.h>

typedef int S_INT;

int main(void)
{
	//function declaration
	S_INT Add(S_INT, S_INT);

	//typedefs
	typedef int S_INT;
	typedef float S_FLOAT;
	typedef double S_DOUBLE;
	typedef char S_CHAR;

	//Win32 SDK typedefs
	typedef unsigned int UINT;
	typedef UINT HANDLE;
	typedef HANDLE HWND;
	typedef HANDLE HINSTANCE;

	//variable declaration
	S_INT sI = 10, k;
	S_INT kArray[] = { 3,6,9,12,15,18,21,24,27,30 };

	S_FLOAT sF = 3.1097f;
	const S_FLOAT sF_pi = 3.1428f;

	S_CHAR c = '!';
	S_CHAR cArray1[] = "Hello";
	S_CHAR cArray2[][10] = { "RTR", "2020-2021" };
	S_DOUBLE sd = 15.21995;

	UINT uint = 6724;
	HANDLE handle = 973;
	HWND hwnd = 8274;
	HINSTANCE hInstance = 493;

	//code
	printf("\n\nType S_INT variable i = %d\n\n", sI);

	for (k = 0; k < (sizeof(kArray) / sizeof(int)); k++)
	{
		printf("Type S_INT array variable kArray[%d] = %d\n", k, kArray[k]);
	}
	printf("\nType S_FLOAT variable f = %f\n", sF);
	printf("Type S_FLOAT constant s_pi = %f\n", sF_pi);

	printf("\nType S_DOUBLE variable d = %lf\n", sd);
	printf("\nType S_CHAR variable c = %c\n", c);

	printf("Type S_CHAR array variable cArray1 = %s ", cArray1);

	for (k = 0; k < (sizeof(cArray2) / sizeof(cArray2[0])); k++)
	{
		printf("\nType S_CHAR array variable cArray2 = %s ", cArray2[k]);
	}

	printf("\n\nType UINT variable uint = %u\n", uint);
	printf("Type HANDLE variable handle = %u\n", handle);
	printf("Type HWND variable hwnd = %u\n", hwnd);
	printf("Type HINSTANCE variable hInstance = %u\n", hInstance);

	S_INT x = 90;
	S_INT y = 30;
	S_INT ret;

	ret = Add(x, y);
	printf("ret = %d\n", ret);

	return(0);
}

S_INT Add(S_INT a, S_INT b)
{
	//code
	S_INT c;
	c = a + b;
	return(c);

}

