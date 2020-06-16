#include <stdio.h>
#include <string.h>


#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED


#define cport_nr    5                  /* COM number minus 1 */
#define bdrate      115200              /* 115200  */

int PrintBuffer (char *buffer);                 //JIB: Needed to match the function
int WaitforReply (void);                        // Wit for OK function
int WaitForDollar (void);                       // Wait for '$' function (for startup)
int CanRS232PortBeOpened ( void );              // Port open check
void CloseRS232Port (void);

#endif // SERIAL_H_INCLUDED
