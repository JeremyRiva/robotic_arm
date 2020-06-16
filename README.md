# Robotic arm

<h2>Outline</h2>
<p>The task involves writing a program that commands a robot to write text in the x and y plane. The program generated is designed to read text from a file, and consequently display the text on a plane by moving the writing arm up/down, left/right. 
The input text (stored in a file, ‘InputText.txt’) is read in ASCII code through the program one character at a time and is stored in an array. Each stored character then calls the equivalent pen movements to write the ASCII character through x/y and pen up/down commands from text file ‘SingleStrokeFont.txt’. The extracted data is processed and converted to G-code via a function and later sent to the robot Arduino via the COM port. For each character the program is required to ensure that the writing has proportional distance. After a character is written, the pens origin is offset to the right in x or to a new line below in y. Each character occupies a 5mm maximum height and width and each new line is 10mm apart. This whole writing process is repeated for all the characters read from the text file until all characters are displayed.
If an error occurs during the program operation, such as not being able to read a file or communicate to the Arduino via the COM PORT, the program is terminated after displaying the error to the screen prompting the user to check the error.</p>

<div style="align-content:center">
    <img src="robot-arm.gif" width="500">
</div>

<h2>User Manual</h2>

<h3>Setup</h3>
<p>To setup the robot program, first the Arduino app needs to be opened. Ensure the robot is both switched on and connected to a USB port then open the Tools bar. Under the tools bar ensure the “Board” (Tools>>Board) and “Processor” (Tools>>Processor) are both connected to the right Arduino type and then see which “Port” the robot is under (Tools>>Port>>COM N, where N is a number i.e. COM 5). To check the correct port is selected, open the “serial monitor” (Tools>>Serial monitor) and press “Enter” on the keyboard, if the reply is “ok” then the right COM port has been selected; otherwise retry using another port. Close the serial monitor.
Then open in Code::Blocks the main.c file contained within the given folder, in addition to the serial.h file. Set the serial.h file to the correct COM port, to do so set the “#define” to the COM port number by subtracting 1. If N is 5, defined previously in Tools>>Port>>COM N, the cport_nr is N-1, hence i.e. #define cport_nr 4.
Once the correct port is selected, save and compile both programs in Code::Blocks (main.c and serial.h), then run the main.c program.
</p>

<h3>FAQ</h3>
<p>How do I solve error “the file could not be opened, exiting”?</p>
<p><em>Check the file directory and ensure all files are in place. Namely “SingleStrokeFont.txt” and “InputText.txt” file should be in the same folder as the main.c.</em></p>

<p>How do I solve error “Unable to open the COM port (specified in serial.h)”?</p>
<p><em>Open serial.h and select suitable COM port (given in the Arduino program). Be aware in serial.h the COM port specified is one number smaller i.e. COM PORT 5, serial.h = 4. If it doesn’t work try selecting another USB port and follow the setup again.</em></p>

<p>How can I change the print text?</p>
<p><em>The printed text is stored in the InputText.txt file. To edit it, open the text file and input desired text. Remember to save in same directory and ensure the file name has not changed.</em></p>

<p>How can I change the print scale?</p>
<p><em>The print scale is defined at the beginning of the program. It can be modified by changing “#define SCALE 5.0” to a desired scale in millimetres; the current scale is set to 5mm. When changing it remember to write a “.0” after an integer number, i.e. 4 -> 4.0.</em></p>

<h2>System Manual</h2>

<h3>Hierarchy</h3>
<p>The program after predefining some variables (x, y, columns[LINES], textBuffer[SIZE]) calls the ReadFile(columns, textBuffer) function. Within the function it reads the text from “InputText.txt” and stores into the predefined array “textBuffer[SIZE]” and the length of the array is calculated. In addition the “SingleStrokeFont.txt” is read and stored in the columns[LINES] structure and then exits the function. If the program has problems reading either “test.txt” or “SingleStrokeFont.txt” then it returns an error and exits the program after closing all files.
If the file opens correctly the program checks that the COM port can be opened. Similarly to before, if an error is returned it exits the program. Once it has successfully passed the tests, it opens the InitialiseRobot(void) function which sets up the Arduino for the writing. Subsequently, it enters a for loop where each character is read one character at a time. Within the loop the function WriteAsciiCharacters(columns, textBuffer, n, x, y) is called. The function, via a series of checks, reads the ASCII number equivalent of the character and proceeds to extract the required data and convert the extracted code into G-code via the ASCIItoGcode(x, y, anteA, anteB, anteC, postA, postB, postC) function. The function prints the commands in G-code to the chosen scale, while transcribing to the robot the position of the pen (pen up or pen down). After each character is written, the next character is displaced accordingly in the x and y direction.
The process is repeater for all characters, until all characters are written. After a character is written the program adds a count to the loop, once the count is reached the program exits the for loop, the COM port is closed and the program is terminated.</p>

<h3>Files used in project</h3>
<p>The files contained within the folder are as follows:</p>
<ul>
    <li>Main.c – file to execute program</li>
    <li>Serial.c – file which contains functions used in main.c to verify communication with robot</li>
    <li>Serial.h – file to set COM port correctly</li>
    <li>Rs232.c – file enables robot communication, written by Teunis van Beelen</li>
    <li>Rs232.h – file enables robot communication, written by Teunis van Beelen</li>
    <li>SingleStrokeFont.txt – text file used to read ASCII code to written form</li>
    <li>InputText.txt – data file read by program and then printed by robot Main.c and Serial.h are the only files required to be opened as mentioned before in the USER MANUAL (see setup). The text within InputText.txt file may be changed as desired as also mentioned in the USER MANUAL (see FAQ).</li>
</ul>

<h3>Data storage for Key items</h3>
<p>The first data storage occurs for both the text files in order to allow the access throughout the code. The input text read from the InputText.txt file (calling the ReadFile() function) is stored in the textBuffer[] array. Similarly the SingleStrokeFont.txt data is stored in the columns[] structure. The textBuffer[] array is called within the WriteAsciiCharacters() function for each character using a for loop. The columns[] structure is called both in the WriteAsciiCharacters() function as well as the ASCIItoGcode() function.
In addition to the stored text files in the program, the code makes use of pen positioning. The pen position is stored with the use of pointers for x and y. For the pen positioning, the initial position is set to zero for the x and y position. For every character read, the pen is required to store a character offset in the x-direction ‘returnedX’, to allow proportional distance between each character. After a character is written, ‘x’ shifts by the offset (i.e. x = x + returnedX*(5.0/18.0)). When the character reads a new line (‘Enter’ = Ascii ‘10’) the x direction pointer is reset to zero while the ‘y’ count is offset by 10mm downwards (i.e. x = 0.0, y = y + returnedY*(5.0/18.0)).</p>

<h3>Planned key functions</h3>
<p>The program makes use of multiple functions within “int main ()” to run as shown below:</p>

<p>// function that opens and reads files</p>
<p><em>int ReadFile(struct ColumnVariables* cols, char Textbuffer[SIZE]);</em></p>
<p>The function reads the files InputText.txt and SingleStrokeFont.txt and returns 0 if no problems are encountered otherwise it returns -1 while displaying an error.</p>

<p>// function that converts ASCII code to G-code</p>
<p><em>void ASCIItoGcode(float x, float y, int anteA, int anteB, int anteC, int postA, int postB, int postC);</em></p>
<p>The function writes to G-code by comparing the third column of the previous line (ante C) with the third column of the current line (postC) in order to understand if the pen position has changed. Then in writes in G-code by adding the x and y pointers to the current value in x and y (respectively postA and postB). The function itself doesn’t return anything and is called within the WriteAsciiCharacters() function.</p>

<p>// function finds character from Ascii code</p>
<p><em>void WriteAsciiCharacters(struct ColumnVariables* columns, char Textbuffer[SIZE], int n, float* x, float* y);</em></p>
<p>The function is called within the main function and contains most of the logic of the program. The function compares the columns[] value to the Textbuffer[] value and calls the ASCIItoGcode() function. After the function is called the x and y pointers are updated the function is exited. This function does not return anything to main().</p>

<p>// function to initialise robot for writing</p>
<p><em>void InitialiseRobot(void);</em></p>
<p>The function initialises the robot for writing. In this function there are no inputs or outputs.</p>

<p>// Send the data to the robot - note in 'PC' mode you need to hit space twice</p>
<p><em>void SendCommands(char* buffer);</em></p>
<p>Again, this function does not return anything. The function is used to write to the robot and ensure the robot reads it with enough time.</p>


<h2>Testing Data</h2>
<p>The program tests for errors in opening files within the ReadFile() function. If for example the file “SingleStrokeFont.txt” cannot be opened, an error is displayed on the screen for the user, prompting the user to check the directory. After displaying the error, it then proceeds to close the file and the program is terminated. Similarly is the case for “InputText.txt”.
In addition, the program is also designed to test if it can communicate to the robot via the COM port. If there are problems, it displays an error message to the user via the “CanRS232PortBeOpened()” function. It then closes all files and terminates the program.</p>