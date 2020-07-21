#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAX_STRING_LENGTH 500

struct Character_Count
{
	char c;
	int c_count;
} CharacterCount[] = { { 'A', 0 }, 
						{ 'B', 0 }, 
						{ 'C', 0 }, 
						{ 'D', 0 }, 
						{ 'E', 0 }, 
						{ 'F', 0 },
						{ 'G', 0 },
						{ 'H', 0 },
						{ 'I', 0 },
						{ 'J', 0 },
						{ 'K', 0 },
						{ 'L', 0 },
						{ 'M', 0 },
						{ 'N', 0 },
						{ 'O', 0 },
						{ 'P', 0 },
						{ 'Q', 0 },
						{ 'R', 0 },
						{ 'S', 0 },
						{ 'T', 0 },
						{ 'U', 0 },
						{ 'V', 0 },
						{ 'W', 0 },
						{ 'X', 0 },
						{ 'Y', 0 },
						{ 'Z', 0 } };


#define SIZE_OF_ENTIRE_ARRAY_OF_STRUCTS sizeof(CharacterCount)
#define SIZE_OF_ONE_STRUCT_FROM_THE_ARRAY_OF_STRUCTS sizeof(CharacterCount[0])
#define NUM_ELEMENTS_IN_ARRAY (SIZE_OF_ENTIRE_ARRAY_OF_STRUCTS / SIZE_OF_ONE_STRUCT_FROM_THE_ARRAY_OF_STRUCTS)
	
int main(void)
{
	char str[MAX_STRING_LENGTH];
	int s, k, ActualStringLength = 0;
	
	printf("\n\nEnter A String : \n");
	gets_s(str, MAX_STRING_LENGTH);

	ActualStringLength = strlen(str);
	
	printf("\n\nThe String Entered Is : \n");
	printf("%s\n\n", str);


	for (s = 0; s < ActualStringLength; s++)
	{
		for (k = 0; k < NUM_ELEMENTS_IN_ARRAY; k++) 
		{
			str[s] = toupper(str[s]);
				
			if (str[s] == CharacterCount[k].c) 
				CharacterCount[k].c_count++;
				
		}
	}
	/*for (s = 0; s < ActualStringLength; s++)
	{
		//for (s = 0; s < ActualStringLength; s++)
		//{
			if ('A' >= str[s] <= 'Z')// || ('a' >= str[s] <= 'z'))
			{
				str[s] = toupper(str[s]);
				char c = str[s];
				c = c - 'A';
				count[c] = count[c] + 1;
			}
		//}
	}*/
	
	printf("\nThe Number Of Occurences Of All Characters From The Alphabet Are : \n\n");

	for (s = 0; s < NUM_ELEMENTS_IN_ARRAY; s++)
	{
		printf("%c = %d\n", CharacterCount[s].c, CharacterCount[s].c_count);
	}

	getch();
	return(0);
}
