/*
 * readall.c:
 *	The readall functions - getting a bit big, so split them out.
 *	Copyright (c) 2012-2013 Gordon Henderson
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <wiringPi.h>

extern int wpMode ;

#ifndef TRUE
#  define       TRUE    (1==1)
#  define       FALSE   (1==2)
#endif

/*
 * doReadallExternal:
 *	A relatively crude way to read the pins on an external device.
 *	We don't know the input/output mode of pins, but we can tell
 *	if it's an analog pin or a digital one...
 *********************************************************************************
 */

static void doReadallExternal (void)
{
  int pin ;

  printf ("+------+---------+--------+\n") ;
  printf ("|  Pin | Digital | Analog |\n") ;
  printf ("+------+---------+--------+\n") ;

  for (pin = wiringPiNodes->pinBase ; pin <= wiringPiNodes->pinMax ; ++pin)
    printf ("| %4d |  %4d   |  %4d  |\n", pin, digitalRead (pin), analogRead (pin)) ;

  printf ("+------+---------+--------+\n") ;
}


/*
 * doReadall:
 *	Read all the GPIO pins
 *	We also want to use this to read the state of pins on an externally
 *	connected device, so we need to do some fiddling with the internal
 *	wiringPi node structures - since the gpio command can only use
 *	one external device at a time, we'll use that to our advantage...
 *********************************************************************************
 */

static char *alts [] =
{
  "IN", "OUT", "ALT5", "ALT4", "ALT0", "ALT1", "ALT2", "OFF"
} ;

// guenter anfang
static int physToWpi [64] =
{
  -1,        // 0
  -1,  -1,   // 1, 2
   8,  -1,   // 3, 4
   9,  -1,   // 5, 6
   7,  15,   // 7, 8
  -1,  16,   // 9, 10
   0,   1,   //11, 12
   2,  -1,   //13, 14
   3,   4,   //15, 16
  -1,   5,   //17, 18
  12,  -1,   //19, 20
  13,   6,   //21, 22
  14,  10,   //23, 24

  -1,  -1,   //25, 26
  -1,  -1,   //27, 28
  -1,  -1,   //29, 30
  -1,  -1,   //31, 32
  -1,  -1,   //33, 34
  -1,  -1,   //35, 36

  17,  18,   //37, 38

    /* 39~63 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
} ;
//guenter ende

//guenter orange pi
static char *physNames [64] = 
{
  NULL,
/* 24 Pin */
 "    3.3V", "5V      ",
 " GPIOA12", "5V      ",
 " GPIOA11", "0v      ",
 " GPIOG11", "GPIOG6  ",
 "      0v", "GPIOG7  ",
 "  GPIOA0", "GPIOA6  ",
 "  GPIOA2", "0v      ",
 "  GPIOA3", "GPIOG8  ",
 "    3.3v", "GPIOG9  ",
 "  GPIOC0", "0v      ",
 "  GPIOC1", "GPIOA1  ",
 "  GPIOC2", "GPIOC3  ",

/* 12 Pin */
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,

/* UART0, tx, rx */
 "  GPIOA4", "  GPIOA5", 

       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL
} ;
// guenter ende


/*
 * readallPhys:
 *	Given a physical pin output the data on it and the next pin:
 *| BCM | wPi |   Name  | Mode | Val| Physical |Val | Mode | Name    | wPi | BCM |
 *********************************************************************************
 */

static void readallPhys (int physPin, int pair)
{
  int pin ;

  if (physPinToGpio (physPin) == -1)
    printf (" |     |    ") ;
  else
    printf (" | %3d | %3d", physPinToGpio (physPin), physToWpi [physPin]) ;

  printf (" | %s", physNames [physPin]) ;

  if (physToWpi [physPin] == -1)
    printf (" |      |  ") ;
  else
  {
   if (wpMode == WPI_MODE_GPIO) {
       // printf("#### WPI_MODE_GPIO\n");
       pin = physPinToGpio (physPin) ;
   } else if (wpMode == WPI_MODE_PHYS) {
       // printf("#### WPI_MODE_PHYS\n");
       pin = physPin ;
    } else {
       // printf("#### Unknow Mode, physPin=%d, physToWpi [physPin] =%d\n", physPin, physToWpi [physPin]);
       pin = physToWpi [physPin] ;
    }

     printf (" | %4s", alts [getAlt (pin)]) ;
     printf (" | %d", digitalRead (pin)) ;
  }

// Pin numbers:

  printf (" | %2d", physPin) ;
  if (!pair) {
      printf (" |\n") ;
      return;
  }


  ++physPin ;

  printf (" || %-2d", physPin) ;

// Same, reversed
  if (physToWpi [physPin] == -1)
    printf (" |   |     ") ;
  else
  {
   if (wpMode == WPI_MODE_GPIO) {
       // printf("#### WPI_MODE_GPIO\n");
       pin = physPinToGpio (physPin) ;
   } else if (wpMode == WPI_MODE_PHYS) {
       // printf("#### WPI_MODE_PHYS\n");
       pin = physPin ;
    } else {
       // printf("#### Unknow Mode, physPin=%d, physToWpi [physPin] =%d\n", physPin, physToWpi [physPin]);
       pin = physToWpi [physPin] ;
    }

    printf (" | %d", digitalRead (pin)) ;
    printf (" | %-4s", alts [getAlt (pin)]) ;
  }

  printf (" | %-5s", physNames [physPin]) ;

  if (physToWpi[physPin] == -1)
    printf (" |     |    ") ;
  else
    printf (" | %-3d | %-3d", physToWpi [physPin], physPinToGpio (physPin)) ;

  printf (" |\n") ;
}


//guenter 
void NanoPiReadAll(void)
{
  int pin ;

  printf (" +-----+-----+----------+------+---+-NanoPi NEO/NEO2--+------+----------+-----+-----+\n") ;
  printf (" | BCM | wPi |   Name   | Mode | V | Physical | V | Mode | Name     | wPi | BCM |\n") ;
  printf (" +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+\n") ;
  for (pin = 1 ; pin <= 24 ; pin += 2)
    readallPhys (pin, 1) ;
  printf (" +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+\n") ;
  printf (" | BCM | wPi |   Name   | Mode | V | Physical | V | Mode | Name     | wPi | BCM |\n") ;
  printf (" +-----+-----+----------+------+---+-NanoPi NEO/NEO2--+------+----------+-----+-----+\n") ;	
  printf ("\n");

  printf (" +-----+----NanoPi NEO/NEO2 Debug UART-+----+\n") ;
  printf (" | BCM | wPi |   Name   | Mode | V | Ph |\n") ;
  printf (" +-----+-----+----------+------+---+----+\n") ;
  for (pin = 37 ; pin <= 38 ; pin++) {
    readallPhys (pin, 0) ;
  }
  printf (" +-----+-----+----------+------+---+----+\n") ;
}
//guenter ende

void doReadall (void)
{
  int model, rev, mem, maker, overVolted ;

  if (wiringPiNodes != NULL)	// External readall
  {
    doReadallExternal () ;
    return ;
  }

  piBoardId (&model, &rev, &mem, &maker, &overVolted) ;

  if (model == PI_MODEL_M1) {
	 NanoPiReadAll();
  }
  else
    printf ("Oops - unable to determine board type... model: %d\n", model) ;
}
