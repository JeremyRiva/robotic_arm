// ===============================================================
// The following program writes to a robot arm.
// Before running the code, please check the directory 
// to see if "test.txt" and "SingleStrokeFont" are there.
// Then, set serial.h to the correct COM port and run the program.
// ===============================================================

//#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include "Serial.h"
#include "rs232.h"
//#define DEBUG_ON	// Debug state ON when defined
#define LINES 1027
#define SIZE 10000
#define SCALE 5.0	// letter scale in mm

// structure variables for column 1, 2 and 3
struct ColumnVariables
{
	// variables for column 1, 2 and 3
	int one, two, three;
};

// function that opens and reads files
int ReadFile(struct ColumnVariables* cols, char Textbuffer[SIZE]);
// function that converts ASCII code to G-code
void ASCIItoGcode(float x, float y, int anteA, int anteB, int anteC, int postA, int postB, int postC);
// function finds character from Ascii code
void WriteAsciiCharacters(struct ColumnVariables* columns, char Textbuffer[SIZE], int n, float* x, float* y);
// function to initialise robot for writing
void InitialiseRobot(void);
// Send the data to the robot - note in 'PC' mode you need to hit space twice
void SendCommands(char* buffer);

// main function
int main()
{
	char textBuffer[SIZE];
	struct ColumnVariables columns[LINES];		// define structure called columns of size 1027
	float x = 0, y = 0;		// initialise robot arm location

	// executes readFile function and tests if files were opened correctly
	if (ReadFile(columns, textBuffer) == -1)
	{
		exit(0);
	}

#ifdef DEBUG_ON

	int n;
	// executes loop for n number of letters
	for (n = 0; n < SIZE; n++)
	{
		// determines size of buffer according to text in the file
		if (textBuffer[n] == '\0')
		{
			printf("end of reading");
			break;
		}
		// executes function to write ascii characters
		WriteAsciiCharacters(columns, textBuffer, n, &x, &y);
	}
#else

	// Opens port through serial.h command
	if (CanRS232PortBeOpened() == -1)
	{
		printf("\nUnable to open the COM port (specified in serial.h) ");
		exit(0);
	}

	// Initialises robot arm
	InitialiseRobot();

	int n;
	// executes loop for n number of letters
	for (n = 0; n < SIZE; n++)
	{
		// determines size of buffer according to text in the file
		if (textBuffer[n] == '\0')
		{
			printf("end of reading");
			break;
		}
		// executes function to write ascii characters
		WriteAsciiCharacters(columns, textBuffer, n, &x, &y);
	}

	// close COM port before exiting program
	CloseRS232Port();
	printf("Com port now closed\n");

#endif


	return 0;
}


// function to write ascii characters
void WriteAsciiCharacters(struct ColumnVariables* columns, char Textbuffer[SIZE], int n, float* x, float* y)
{
	int i;
	// executes loop for single letter
	for (i = 0; i < LINES; i++)
	{
		// execute if first column = 999 AND if second column = InputText.txt
		if ((columns[i].one == 999) && (columns[i].two == (int)Textbuffer[n]))
		{
			float returnedX = columns[(i + columns[i].three)].one;	// stores offset in x from previous character
			float returnedY = columns[(i + columns[i].three)].two;	// stores offset in y from previous character

			// if statement for a new line
			if (columns[i].two == 10)
				*x = 0.0;	// x counter is reset to zero

			int j;
			// run loop for third column number of times and print to screen
			for (j = i; j < columns[i].three + i; j++)
			{
				// calls function that converts ASCII to G-code
				ASCIItoGcode(*x, *y, columns[j].one, columns[j].two, columns[j].three, columns[j + 1].one, columns[j + 1].two, columns[j + 1].three);

				// print script to screen equivalent to users letter
				//printf("%d %d %d \n", columns[j + 1].one, columns[j + 1].two, columns[j + 1].three);
			}

			// if statement for a new line
			if (columns[i].two == 10)
				*y = *y + (returnedY * SCALE / 18.0);	// y counter for new line
			else
			{
				*x = *x + (returnedX * SCALE / 18.0);	// x counter
				//y = y + (returnedY * SCALE / 18.0);	// y counter, in this case returnedY = 0
			}

			printf("\n");	// new line between characters

		}

	}

	return;
}


// Function to open and read files
int ReadFile(struct ColumnVariables* cols, char Textbuffer[SIZE])
{
	// open external file for reading
	FILE* fAsciiCharacters;
	fAsciiCharacters = fopen("SingleStrokeFont.txt", "r");

	FILE* fTextFile;
	fTextFile = fopen("InputText.txt", "r");

	// error check for file
	if ((fAsciiCharacters == NULL) || (fTextFile == NULL))
	{
		printf("\nthe file could not be opened, exiting");
		return (-1);
	}

	int i, n = 0;
	// populates structure columns with data from SingleStrokeFont.txt
	for (i = 0; i < LINES; i++)
	{
		// read from file and store into ColumnVariable structure for column 1, 2 and 3
		fscanf(fAsciiCharacters, "%d %d %d", &cols[i].one, &cols[i].two, &cols[i].three);
	}

	// populates Textbuffer with data from InputText
	while (n < SIZE && !feof(fTextFile))
	{
		Textbuffer[n] = fgetc(fTextFile);
		printf("%c", Textbuffer[n]);
		n++;
	}

	Textbuffer[n] = '\0';	// terminate array with '\0'

	printf("\n");
	// close file after reading
	fclose(fAsciiCharacters), fclose(fTextFile);
	return (0);
}


// function to write ascii to g-code
void ASCIItoGcode(float x, float y, int anteA, int anteB, int anteC, int postA, int postB, int postC)
{
	// scales x and y coordinates and adds offsets
	float X = x + (postA * SCALE / 18.0), Y = y + (postB * SCALE / 18.0);

#ifdef DEBUG_ON

	// check if pen position has changed from previous line
	if (postC != anteC)
	{
		if (postC == 1)
			printf("S1000\n");	// commands for pen down

		else
			printf("S0\n");		// commands for pen up
	}
	// comand where x and y are resized and G is either 1 or 0
	printf("G%d X%.2f Y%.2f\n", postC, X, Y);

#else
	char buffer[SIZE];

	// check if pen position has changed from previous line
	if (postC != anteC)
	{
		if (postC == 1)
			SendCommands("S1000\n");	// commands for pen down
		else
			SendCommands("S0\n");	// commands for pen up
	}

	// comand where x and y are resized and G is either 1 or 0
	sprintf(buffer, "G%d X%.2f Y%.2f\n", postC, X, Y);
	SendCommands(buffer);
	// slows buffer down to allow robot comunication
	int WaitForReply();
	Sleep(200);

#endif

	return;
}


#ifndef DEBUG_ON
// function to initialise robot for setup
void InitialiseRobot(void)
{
	// initialising arduino for use
	printf("\nAbout to wake up the robot\n");

	SendCommands("\n");
	Sleep(200);

	// This is a special case - we wait  until we see a dollar ($)
	WaitForDollar();
	printf("\nThe robot is now ready to draw\n");

	SendCommands("G1 X0 Y0 F1000\n");
	SendCommands("M3\n");
	//SendCommands("S0\n");
	return;
}

// Send the data to the robot - note in 'PC' mode you need to hit space twice
// as the dummy 'WaitForReply' has a getch() within the function.
void SendCommands(char* buffer)
{
	// printf ("Buffer to send: %s", buffer); // For diagnostic purposes only, normally comment out
	PrintBuffer(&buffer[0]);
	int WaitForReply();
	Sleep(200); // Can omit this when using the writing robot but has minimal effect
	// getch(); // Omit this once basic testing with emulator has taken place
}

#endif
