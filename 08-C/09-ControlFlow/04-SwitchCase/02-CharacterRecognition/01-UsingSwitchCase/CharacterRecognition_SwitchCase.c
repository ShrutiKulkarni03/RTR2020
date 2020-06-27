#include<stdio.h>
#include<conio.h>

#define CHAR_ALPHABET_UPPER_CASE_BEGINNING 65
#define CHAR_ALPHABET_UPPER_CASE_ENDING 90

#define CHAR_ALPHABET_LOWER_CASE_BEGINNING 97
#define CHAR_ALPHABET_LOWER_CASE_ENDING 122

#define CHAR_DIGIT_BEGINNING 48
#define CHAR_DIGIT_ENDING 57

int main(void)
{
	char sk_c;
	int sk_c_value;

	printf("\n\nEnter a Character\n");
	sk_c = getch();

	switch (sk_c)
	{
		case 'A':
		case 'a':

		case 'E':
		case 'e':

		case 'I':
		case 'i':

		case 'O':
		case 'o':

		case 'U':
		case 'u':

			printf("Character \'%c\' is a VOWEL\n", sk_c);
			break;

		default:
			sk_c_value = (int)sk_c;

			if ((sk_c_value >= CHAR_ALPHABET_UPPER_CASE_BEGINNING && sk_c_value <= CHAR_ALPHABET_UPPER_CASE_ENDING) || (sk_c_value >= CHAR_ALPHABET_LOWER_CASE_BEGINNING && sk_c_value <= CHAR_ALPHABET_LOWER_CASE_ENDING))
			{
				printf("Character \'%c\' is a CONSONANT\n", sk_c);
			}
			else if (sk_c_value >= CHAR_DIGIT_BEGINNING && sk_c_value <= CHAR_DIGIT_ENDING)
			{
				printf("Character \'%c\' is a DIGIT\n", sk_c);
			}
			else
			{
				printf("Character \'%c\' is a SPECIAL CHARACTER\n", sk_c);
			}
			break;


	}

	printf("\n\nSwitch Case Block Complete\n");

	getch();
	return(0);



}
