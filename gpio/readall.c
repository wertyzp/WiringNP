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


static int physToWpi_m1 [64] =
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
  -1,  11,   //25, 26
  30,  31,   //27, 28
  21,  -1,   //29, 30
  22,  26,   //31, 32
  23,  -1,   //33, 34
  24,  27,   //35, 36
  25,  28,   //37, 38
  -1,  29,   //39, 40
   -1, -1, 32, 33, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //41-> 55
   -1, -1, -1, -1, -1, -1, -1, -1 // 56-> 63
} ;


static char *physNames_m1 [64] = 
{
  NULL,

 "    3.3v", "5v      ",
 "   SDA.0", "5V      ",
 "   SCL.0", "0v      ",
 "  GPIO.7", "TxD3    ",
 "      0v", "RxD3    ",
 "    RxD2", "GPIO.1  ",
 "    TxD2", "0v      ",
 "    CTS2", "GPIO.4  ",
 "    3.3v", "GPIO.5  ",
 "    MOSI", "0v      ",
 "    MISO", "RTS2    ",
 "    SCLK", "CE0     ",
 "      0v", "GPIO.11 ",
 "   SDA.1", "SCL.1   ",
 " GPIO.21", "0v      ",
 " GPIO.22", "RTS1    ",
 " GPIO.23", "0v      ",
 " GPIO.24", "CTS1    ",
 " GPIO.25", "TxD1    ",
 "      0v", "RxD1    ",
  
 "      0v", "      5v", 
 "  GPIO.4", "  GPIO.5",  
  
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
  "GPIO.17", "GPIO.18",
  "GPIO.19", "GPIO.20",
   NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
} ;


static int physToWpi_neo [64] =
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
static char *physNames_neo [64] = 
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

static int physToWpi_duo [64] =
{
  -1,        // 0
  -1,  -1,   // 1, 2
  -1,  -1,   // 3, 4
  -1,  -1,   // 5, 6
  -1,  -1,   // 7, 8
  -1,  -1,   // 9, 10
   8,  -1,   //11, 12
   9,  -1,   //13, 14
   7,  -1,   //15, 16
   0,  -1,   //17, 18
   2,  -1,   //19, 20
   3,  16,   //21, 22
  12,  -1,   //23, 24

  13,  -1,   //25, 26
  -1,  -1,   //27, 28
  14,  -1,   //29, 30
  15,  -1,   //31, 32
   /* ---------nanopi duo end----------- */
  -1,  -1,   //33, 34
  -1,  -1,   //35, 36

  -1,  -1,   //37, 38

    /* 39~63 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
} ;

static char *physNames_duo [64] = 
{
  NULL,
/* 32 Pin */
 "   MIC_N", "EPhySPD ",
 "   MIC_P", "EPhyLinK",
 "LineOutR", "EPhyTXP ",
 "LineOutL", "EPhyTXN ",
 "    CVBS", "EPhyRXP ",
 "  GPIOG6", "EPhyRXN ",
 "  GPIOG7", "USB-DP2 ",  //13, 14
 " GPIOA15", "USB-DM2 ",
 " GPIOA16", "USB-DP3 ",
 " GPIOA14", "USB-DM3 ",
 " GPIOA13", "GPIOG11 ",
 " GPIOA12", "IR-RX   ",
 " GPIOA11", "0v      ", //25, 26
 "      0v", "3.3v    ",
 "  GPIOA4", "5v      ",
 "  GPIOA5", "5v      ",
   /* ---------nanopi duo end----------- */

       NULL, NULL,
       NULL, NULL,

/* UART0, tx, rx */
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
       NULL, NULL,
       NULL
} ;


/*
 * readallPhys:
 *	Given a physical pin output the data on it and the next pin:
 *| BCM | wPi |   Name  | Mode | Val| Physical |Val | Mode | Name    | wPi | BCM |
 *********************************************************************************
 */

static void readallPhys (int boardId, int physPin, int pair)
{
  int pin ;
  int *physToWpi;
  char **physNames;

  if (boardId == NanoPi_M1 || boardId == NanoPi_M1_Plus || boardId == NanoPi_M1_Plus2) {
       physToWpi = physToWpi_m1;
       physNames = physNames_m1;
  } else if (boardId == NanoPi_NEO || boardId == NanoPi_NEO_Air || boardId == NanoPi_NEO2 || boardId == NanoPi_NEO_Plus2) {
      physToWpi = physToWpi_neo;
      physNames = physNames_neo;
  } else if (boardId == NanoPi_Duo) {
      physToWpi = physToWpi_duo;
      physNames = physNames_duo;
  } else {
      return ;
  }

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
void NanoPiReadAll()
{
  int pin ;
  BoardHardwareInfo* retBoardInfo;
  int boardId;
  boardId = getBoardType(&retBoardInfo);
  if (boardId >= 0) {
      if (boardId > ALLWINNER_BASE && boardId <= ALLWINNER_MAX 
                && boardId != NanoPi_A64
                && boardId != NanoPi_NEO_Core) {
          // nothing to do.
      } else {
        printf ("This NanoPi model is currently not supported. ") ;
        return ;
      }
  } else {
      printf ("Your NanoPi has an unknown model type. Please report this to\n") ;
      printf ("    support@friendlyarm.com\n") ;
      printf ("with a copy of your /proc/cpuinfo if possible\n") ;
      return ;
  }

  int pinCount;
  if (boardId == NanoPi_M1 || boardId == NanoPi_M1_Plus || boardId == NanoPi_M1_Plus2) {
      pinCount = 40;
  } else if (boardId == NanoPi_NEO || boardId == NanoPi_NEO_Air || boardId == NanoPi_NEO2 || boardId == NanoPi_NEO_Plus2) {
      pinCount = 24;
  } else if (boardId == NanoPi_Duo) {
      pinCount = 32;
  } else {
    return ;
  }


  printf (" +-----+-----+----------+------+---+-%s--+------+----------+-----+-----+\n", retBoardInfo->boardDisplayName) ;
  printf (" | BCM | wPi |   Name   | Mode | V | Physical | V | Mode | Name     | wPi | BCM |\n") ;
  printf (" +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+\n") ;
  for (pin = 1 ; pin <= pinCount ; pin += 2)
    readallPhys (boardId, pin, 1) ;
  printf (" +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+\n") ;
  printf (" | BCM | wPi |   Name   | Mode | V | Physical | V | Mode | Name     | wPi | BCM |\n") ;
  printf (" +-----+-----+----------+------+---+-%s--+------+----------+-----+-----+\n", retBoardInfo->boardDisplayName) ;	
  printf ("\n");


  if (boardId == NanoPi_M1 || boardId == NanoPi_M1_Plus || boardId == NanoPi_M1_Plus2) {
    printf (" +-----+----%s Debug UART-+----+\n", retBoardInfo->boardDisplayName) ;
    printf (" | BCM | wPi |   Name   | Mode | V | Ph |\n") ;
    printf (" +-----+-----+----------+------+---+----+\n") ;
    for (pin = 41 ; pin < 45 ; pin++) {
      readallPhys (boardId, pin, 0) ;
    }
    printf (" +-----+-----+----------+------+---+----+\n") ;
  } else if (boardId == NanoPi_NEO || boardId == NanoPi_NEO_Air || boardId == NanoPi_NEO2 || boardId == NanoPi_NEO_Plus2) {
    printf (" +-----+----%s Debug UART-+----+\n", retBoardInfo->boardDisplayName) ;
    printf (" | BCM | wPi |   Name   | Mode | V | Ph |\n") ;
    printf (" +-----+-----+----------+------+---+----+\n") ;
    for (pin = 37 ; pin <= 38 ; pin++) {
      readallPhys (boardId, pin, 0) ;
    }
    printf (" +-----+-----+----------+------+---+----+\n") ;
  } else if (boardId == NanoPi_Duo) {
      // nothing to do.
  }
}
//guenter ende

void doReadall (void)
{
  if (wiringPiNodes != NULL) // External readall
  {
    doReadallExternal () ;
    return ;
  }

  NanoPiReadAll();
}
