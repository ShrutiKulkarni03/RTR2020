#include<stdio.h>

int main(void)
{
	printf("\n\n");
	printf("Going On To Next Line...Using \\n Escape Sequence\n\n");
	printf("Demonstrating \t Horizontal \t Tab \t Using \t \\t Escape Sequence !!!\n\n");
	printf("\"This Is A Double Quoted Output\" Done Using \\\" \\\" Escape Sequence\n\n");
	printf("\'This Is A Single Quoted Output\' Done Using \\' \\' Escape Sequence\n\n");
	printf("BACKSPACE Turned To BACKSPACE\b Using Escape Sequence \\b\n\n");

	printf("\rDemonstrating Carriage Return Using \\r Escape Sequence\n");
	printf("Demonstrating \r Carriage Return Using \\r Escape Sequence\n");
	printf("Demonstrating Carriage \r Return Using \\r Escape Sequence\n\n");

	printf("Demonstrating \x53 Using \\xhh Escape Sequence\n\n"); //0x41 Is The Hexadecimal Code For Letter 'A'. 'xhh' Is The Place-holder For 'x' Followed By 2 digits (hh), altogether forming a Hexadecimal Number.
	printf("Demonstrating \113 Using \\ooo Escape Sequence\n\n"); //102 Is The Octal Code For Letter 'B'. 'ooo' Is The Place-holder For 3 Digits Forming An Octal Number.
	return(0);
}
