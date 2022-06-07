
#include <linux/fcntl.h>
#include <linux/gpio.h>
//Next libs CAN'T work for kernel development
//#include <stdio.h>
//#include <stdlib.h>
//#include <sys/stat.h>
//#include <sys/types.h>
//#include <unistd.h>

#define IN 0
#define OUT 1

#define LOW 0
#define HIGH 1

#define PIN_24 24 /* P1-18 */
#define POUT 4 /* P1-07 */

static int GPIOExport(int pin);
static int GPIOUnexport(int pin);
static int GPIODirection(int pin, int dir);
static int GPIORead(int pin);
static int GPIOWrite(int pin, int value);