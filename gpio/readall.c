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

#define DEBUG_READALL 0

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

#define MAX_PIN_COUNT 74

static int physToWpi_m1 [MAX_PIN_COUNT] =
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
  -1, -1, -1, -1, -1, -1, -1, -1                              // 56-> 63

  /* 64~73 */
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
} ;


static char *physNames_m1 [MAX_PIN_COUNT] = 
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
 "      0v", "RxD1    ",    //39, 40
  
 "      0v", "      5v", 
 "  GPIO.4", "  GPIO.5",  
  
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,         //49, 50
  "GPIO.17", "GPIO.18",
  "GPIO.19", "GPIO.20",

  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,       //55 ~ 63
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //64 ~ 73

} ;


static int physToWpi_neo [MAX_PIN_COUNT] =
{
  -1,        // 0
  /* 24pins */
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

  /* 12pins */
  -1, -1,      //25, 26  -> 1, 2
  -1, -1,      //27, 28  -> 3, 4
  -1, -1,      //29, 30  -> 5, 6   "GPIOL11 "
  19, -1,      //31, 32  -> 7, 8
  -1, -1,      //33, 34  -> 9, 10
  -1, -1,      //35, 36  -> 11, 12

  17,  18,   //37, 38

    /* 39~63 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    /* 64~73 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
} ;

static char *physNames_neo [MAX_PIN_COUNT] = 
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
 "      5V", " USB-DP1",      //25, 26  -> 1, 2
 " USB-DM1", " USB-DP2",      //27, 28  -> 3, 4
 " USB-DM2", "   IR-RX",      //29, 30  -> 5, 6   "GPIOL11 "
 " GPIOA17", " PCM/I2C",      //31, 32  -> 7, 8
 " PCM/I2C", " PCM/I2C",      //33, 34  -> 9, 10
 " PCM/I2C", "      0V",      //35, 36  -> 11, 12
       
/* UART0, tx, rx */
 "  GPIOA4", "  GPIOA5", 
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,     //47, 48

       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL,           //63

       //64 ~ 73
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,  
} ;

static int physToWpi_duo [MAX_PIN_COUNT] =
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
  12,  18,   //23, 24

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

    /* 64~73 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
} ;

static char *physNames_duo [MAX_PIN_COUNT] = 
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
 " GPIOA12", "GPIOL11 ",
 " GPIOA11", "0v      ",  //25, 26
 "      0v", "3.3v    ",
 "  GPIOA4", "5v      ",
 "  GPIOA5", "5v      ",
 /* ---------nanopi duo end----------- */

       NULL, NULL,
       NULL, NULL,

/* UART0, tx, rx */
       NULL, NULL, 

       NULL, NULL,     //39, 40
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,     //49, 50
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,     //59, 60
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,     //71, 72
       NULL            //73
} ;

static int physToWpi_duo2 [MAX_PIN_COUNT] =
{
  -1,        // 0
  -1,  15,   // 1, 2
  -1,  14,   // 3, 4
  -1,  -1,   // 5, 6
  -1,  13,   // 7, 8

  18,  12,   // 9, 10
  16,   3,   //11, 12
  -1,   2,   //13, 14
  -1,   0,   //15, 16
  -1,   7,   //17, 18
  -1,   9,   //19, 20
  -1,   8,   //21, 22
  -1,  -1,   //23, 24

  -1,  -1,   //25, 26
  -1,  -1,   //27, 28
  -1,  -1,   //29, 30
  -1,  -1,   //31, 32


   /* ---------nanopi duo end----------- */
  -1,  -1,   //33, 34
  -1,  -1,   //35, 36

  -1,  -1,   //37, 38

    /* 39~63 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    /* 64~73 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
} ;

static char *physNames_duo2 [MAX_PIN_COUNT] = 
{
  NULL,
// 32 Pin 
 "      5v", "GPIOA5  ",
 "      5v", "GPIOA4  ", 
 "    3.3v", "0v      ",
 "      0v", "GPIOA11 ",
 " GPIOL11", "GPIOA12 ",
 " GPIOG11", "GPIOA13 ",
 " USB-DM3", "GPIOA14 ",  //13, 14
 " USB-DP3", "GPIOA16 ",
 " USB-DM2", "GPIOA15 ",
 " USB-DP2", "GPIOG7  ",
 " EPhyRXN", "GPIOG6  ",
 " EPhyRXP", "CVBS    ",
 " EPhyTXN", "LineOutL",  //25, 26
 " EPhyTXP", "LineOutR",
 "EPhyLinK", "MIC_P   ",
 " EPhySPD", "MIC_N   ",

 /* ---------nanopi duo end----------- */
       NULL, NULL,
       NULL, NULL,

/* UART0, tx, rx */
       NULL, NULL, 

       NULL, NULL,     //39, 40
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,     //49, 50
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,     //59, 60
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,     //71, 72
       NULL            //73
} ;

static int physToWpi_t3 [MAX_PIN_COUNT] =
{
  -1,        // 0
  -1,  -1,   // 1, 2
   3,   4,   // 3, 4
  -1,  -1,   // 5, 6
   7,   8,   // 7, 8

   9,  10,   // 9, 10
  11,  12,   //11, 12
  13,  14,   //13, 14
  15,  16,   //15, 16
  17,  18,   //17, 18
  19,  20,   //19, 20
  21,  22,   //21, 22
  23,  24,   //23, 24

  25,  -1,   //25, 26
  27,  -1,   //27, 28
  -1,  -1,   //29, 30
  -1,  -1,   //31, 32


   /* ---------nanopi duo end----------- */
  -1,  -1,   //33, 34
  -1,  -1,   //35, 36

  -1,  -1,   //37, 38

    /* 39~63 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    /* 64~73 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
} ;

static char *physNames_t3 [MAX_PIN_COUNT] = 
{
  NULL,
// 32 Pin 
 "    3.3v", "0v      ",
 " GPIOD20", "GPIOD16 ", 
 "I2C0_SCL", "I2C0_SDA",
 " GPIOC31", "GPIOD0  ",
 " GPIOC29", "GPIOC30 ",
 " GPIOD21", "GPIOD17 ",
 " GPIOB29", "GPIOB28 ",  //13, 14
 " GPIOB31", "GPIOB30 ",
 "  GPIOC4", "GPIOC7  ",
 "  GPIOC8", "GPIOC24 ",
 " GPIOC28", "GPIOB26 ",
 "  GPIOD1", "GPIOD8  ",
 " GPIOC13", "AliGPIO3",  //25, 26
 " GPIOC14", "AliGPIO5",
 "      5v", "0v      ",

 /* ---------nanopc t3 end----------- */
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,

/* UART0, tx, rx */
       NULL, NULL, 

       NULL, NULL,     //39, 40
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,     //49, 50
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,     //59, 60
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,     //71, 72
       NULL            //73
} ;

static int physToWpi_neocore [MAX_PIN_COUNT] =
{
/* GPIO-1 24Pin */
  -1,        // 0
  -1,  -1,   // 1, 2  -> 1, 2
   8,  -1,   // 3, 4  -> 3, 4
   9,  -1,   // 5, 6  -> 5, 6
   7,  15,   // 7, 8  -> 7, 8
  -1,  16,   // 9, 10  -> 9, 10
   0,   1,   //11, 12  -> 11, 12
   2,  -1,   //13, 14  -> 13, 14
   3,   4,   //15, 16  -> 15, 16
  -1,   5,   //17, 18  -> 17, 18
  12,  -1,   //19, 20  -> 19, 20
  13,   6,   //21, 22  -> 21, 22
  14,  10,   //23, 24  -> 23, 24

/* GPIO-2 24Pin */
  -1,  22,   //25, 26  -> 1, 2
  -1,  23,   //27, 28  -> 3, 4
  -1,  21,   //29, 30  -> 5, 6
  -1,  20,   //31, 32  -> 7, 8
  -1,  -1,   //33, 34  -> 9, 10
  -1,  -1,   //35, 36  -> 11, 12
  19,  -1,   //37, 38  -> 13, 14
  -1,  -1,   //39, 40  -> 15, 16
  -1,  18,   //41, 42  -> 17, 18
  -1,  17,   //43, 44  -> 19, 20
  -1,  -1,   //45, 46  -> 21, 22
  -1,  -1,   //47, 48  -> 23, 24

 /* GPIO-3 24Pin */
  -1,  -1,   //49, 50  -> 1, 2
  -1,  -1,   //51, 52  -> 3, 4
  -1,  -1,   //53, 54  -> 5, 6
  -1,  -1,   //55, 56  -> 7, 8
  -1,  -1,   //57, 58  -> 9, 10
  -1,  -1,   //59, 60  -> 11, 12
  -1,  24,   //61, 62  -> 13, 14
  -1,  -1,   //63, 64  -> 15, 16
  -1,  -1,   //65, 66  -> 17, 18
  -1,  -1,   //67, 68  -> 19, 20
  -1,  -1,   //69, 70  -> 21, 22
  -1,  -1,   //71, 72  -> 23, 24
  -1,        //73
} ;

static char *physNames_neocore [MAX_PIN_COUNT] = 
{
  NULL,                 // 0
 /* GPIO-1 24Pin */
 "    3.3v", "5v      ",   // 1, 2  -> 1, 2
 " GPIOA12", "5v      ",   // 3, 4  -> 3, 4
 " GPIOA11", "0v      ",   // 5, 6  -> 5, 6
 " GPIOG11", "GPIOG6  ",   // 7, 8  -> 7, 8
 "      0v", "GPIOG7  ",   // 9, 10  -> 9, 10
 "  GPIOA0", "GPIOA6  ",   //11, 12  -> 11, 12
 "  GPIOA2", "0v      ",   //13, 14  -> 13, 14
 "  GPIOA3", "GPIOG8  ",   //15, 16  -> 15, 16
 "    3.3v", "GPIOG9  ",   //17, 18  -> 17, 18
 "  GPIOC0", "0v      ",   //19, 20  -> 19, 20
 "  GPIOC1", "GPIOA1  ",   //21, 22  -> 21, 22
 "  GPIOC2", "GPIOC3  ",   //23, 24  -> 23, 24

 /* GPIO-2 24Pin */
 "      5v", "GPIOA15 ",   //25, 26  -> 1, 2
 "    USB1", "GPIOA16 ",   //27, 28  -> 3, 4
 "    USB1", "GPIOA14 ",   //29, 30  -> 5, 6
 "    USB2", "GPIOA13 ",   //31, 32  -> 7, 8
 "    USB2", "Mic     ",   //33, 34  -> 9, 10
 "      IR", "Mic     ",   //35, 36  -> 11, 12   " GPIOL11"
 " GPIOA17", "Audio   ",   //37, 38  -> 13, 14
 "     I2S", "Audio   ",   //39, 40  -> 15, 16
 "     I2S", "GPIOA5  ",   //41, 42  -> 17, 18
 "     I2S", "GPIOA4  ",   //43, 44  -> 19, 20
 "     I2S", "5V      ",   //45, 46  -> 21, 22
 "      0v", "0v      ",   //47, 48  -> 23, 24

 /* GPIO-3 20Pin */
 "     Eth", "Eth     ",   //49, 50  -> 1, 2
 "     Eth", "Eth     ",   //51, 52  -> 3, 4
 "     Eth", "Eth     ",   //53, 54  -> 5, 6
 "      NC", "NC      ",   //55, 56  -> 7, 8
 "      NC", "NC      ",   //57, 58  -> 9, 10
 "      0v", "0v      ",   //59, 60  -> 11, 12
 "    USB3", "GPIOA7  ",   //61, 62  -> 13, 14
 "    USB3", "GPIOE12 ",   //63, 64  -> 15, 16
 "      5v", "GPIOE13 ",   //65, 66  -> 17, 18
 "      5v", "3.3v    ",   //67, 68  -> 19, 20

       NULL, NULL,         //69, 70  -> 21, 22
       NULL, NULL,         //71, 72  -> 23, 24
       NULL,               //73
} ;



/*
 * readallPhys:
 *	Given a physical pin output the data on it and the next pin:
 *| BCM | wPi |   Name  | Mode | Val| Physical |Val | Mode | Name    | wPi | BCM |
 *********************************************************************************
 */

static void readallPhys (int faBoardId, int physPin, int pair)
{
  int pin ;
  int *physToWpi;
  char **physNames;

  if (faBoardId == NanoPi_M1 || faBoardId == NanoPi_K1_Plus || faBoardId == NanoPi_M1_Plus || faBoardId == NanoPi_M1_Plus2) {
       physToWpi = physToWpi_m1;
       physNames = physNames_m1;
  } else if (faBoardId == NanoPi_NEO || faBoardId == NanoPi_NEO_Air || faBoardId == NanoPi_NEO2 || faBoardId == NanoPi_NEO_Plus2) {
      physToWpi = physToWpi_neo;
      physNames = physNames_neo;
  } else if (faBoardId == NanoPi_Duo) {
      physToWpi = physToWpi_duo;
      physNames = physNames_duo;
  } else if (faBoardId == NanoPi_Duo2) {
      physToWpi = physToWpi_duo2;
      physNames = physNames_duo2;
  } else if (faBoardId == NanoPi_NEO_Core || faBoardId == NanoPi_NEO_Core2) {
      physToWpi = physToWpi_neocore;
      physNames = physNames_neocore;
  } else if (faBoardId == NanoPC_T3) {
      physToWpi = physToWpi_t3;
      physNames = physNames_t3;
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

     int alt = getAltSilence (pin);
     if (alt >= 0) {
       printf (" | %4s", alts [alt]) ;
     } else {
       printf (" |     ") ;
     }
     
     printf (" | %d", digitalReadSilence (pin)) ;
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

    printf (" | %d", digitalReadSilence (pin)) ;
    int alt = getAltSilence (pin);
    if (alt >= 0) {
       printf (" | %4s", alts [alt]) ;
    } else {
       printf (" |     ") ;
    }
  }

  printf (" | %-5s", physNames [physPin]) ;

  if (physToWpi[physPin] == -1)
    printf (" |     |    ") ;
  else
    printf (" | %-3d | %-3d", physToWpi [physPin], physPinToGpio (physPin)) ;

  printf (" |\n") ;
}

static void debugReadallPhys (int faBoardId, int physPin)
{
  int pin ;
  int *physToWpi;
  char **physNames;

  if (faBoardId == NanoPi_M1 || faBoardId == NanoPi_K1_Plus || faBoardId == NanoPi_M1_Plus || faBoardId == NanoPi_M1_Plus2) {
       physToWpi = physToWpi_m1;
       physNames = physNames_m1;
  } else if (faBoardId == NanoPi_NEO || faBoardId == NanoPi_NEO_Air || faBoardId == NanoPi_NEO2 || faBoardId == NanoPi_NEO_Plus2) {
      physToWpi = physToWpi_neo;
      physNames = physNames_neo;
  } else if (faBoardId == NanoPi_Duo) {
      physToWpi = physToWpi_duo;
      physNames = physNames_duo;
  } else if (faBoardId == NanoPi_Duo2) {
      physToWpi = physToWpi_duo2;
      physNames = physNames_duo2;
  } else if (faBoardId == NanoPi_NEO_Core || faBoardId == NanoPi_NEO_Core2) {
      physToWpi = physToWpi_neocore;
      physNames = physNames_neocore;
  } else if (faBoardId == NanoPC_T3) {
      physToWpi = physToWpi_t3;
      physNames = physNames_t3;
  } else {
      return ;
  }

  printf("## physPin: %d, %s\n", physPin, physNames [physPin]);
  if (physPinToGpio (physPin) == -1)
    printf("\tnot gpio\n");
  else
    printf("\tphysPinToGpio=%d, physToWpi=%d\n", physPinToGpio (physPin), physToWpi [physPin]);

  if (physToWpi [physPin] == -1) {
    printf("\t\tnot Wpi pin\n");
  } else {
   if (wpMode == WPI_MODE_GPIO) {
       pin = physPinToGpio (physPin) ;
       printf("\t\tWPI_MODE_GPIO, physPinToGpio (physPin) = %d\n", pin);
   } else if (wpMode == WPI_MODE_PHYS) {
       pin = physPin ;
       printf("\t\tWPI_MODE_PHYS, pin = physPin = %d\n", pin);
    } else {
       printf("\t\tUnknow Mode, physPin=%d, physToWpi [physPin] =%d\n", physPin, physToWpi [physPin]);
       pin = physToWpi [physPin] ;
    }

     printf ("\t\t\talts = %4s\n", alts [getAltSilence (pin)]) ;
     printf ("\t\t\tdigitalRead = %d\n", digitalReadSilence (pin)) ;
  }
}

void NanoPiReadAll()
{
  int pin ;
  BoardHardwareInfo* retBoardInfo;
  int ret = getBoardType(&retBoardInfo);
  if (ret >= 0) {
      if (retBoardInfo->boardTypeId > ALLWINNER_BASE 
        && retBoardInfo->boardTypeId <= ALLWINNER_MAX 
        && retBoardInfo->boardTypeId != NanoPi_A64) {
          // nothing to do.
      } else if(retBoardInfo->boardTypeId == NanoPC_T3) {       
        printf ("This NanoPC-T3  is only supported GPIO input and GPIO out. \n") ;
        printf("Please becareful to use this !!! \n");
      } else {
        printf ("This NanoPi model is currently not supported. \n") ;
        printf ("This NanoPi's boardTypeId is %d .\n",retBoardInfo->boardTypeId);
        return ;
      }
  } else {
      printf ("Your NanoPi has an unknown model type. Please report this to\n") ;
      printf ("    support@friendlyarm.com\n") ;
      printf ("with a copy of your /proc/cpuinfo if possible\n") ;
      return ;
  }

  int pinCount;
  if (retBoardInfo->boardTypeId == NanoPi_M1 
    || retBoardInfo->boardTypeId == NanoPi_K1_Plus 
    || retBoardInfo->boardTypeId == NanoPi_M1_Plus 
    || retBoardInfo->boardTypeId == NanoPi_M1_Plus2) {
      pinCount = 40;
  } else if (retBoardInfo->boardTypeId == NanoPi_NEO 
    || retBoardInfo->boardTypeId == NanoPi_NEO_Air 
    || retBoardInfo->boardTypeId == NanoPi_NEO2 
    || retBoardInfo->boardTypeId == NanoPi_NEO_Plus2
    || retBoardInfo->boardTypeId == NanoPi_NEO_Core
    || retBoardInfo->boardTypeId == NanoPi_NEO_Core2
    ) {
      pinCount = 24;
  } else if (retBoardInfo->boardTypeId == NanoPi_Duo 
    || retBoardInfo->boardTypeId == NanoPi_Duo2) {
      pinCount = 32;
  } else if (retBoardInfo->boardTypeId == NanoPC_T3) {
      pinCount = 30;
  } else {
      printf ("This NanoPi model(id: %d) is currently not supported. \n", retBoardInfo->boardTypeId) ;
      return ;
  }
  if (getenv("WIRINGPI_DEBUG") != NULL)
    printf("This pinCount num is %d .\nDEBUG_READALL is %d.\nwpMode is %d .\n",
           pinCount, DEBUG_READALL, wpMode);

  if (DEBUG_READALL) {
    printf (" +-----+-----+----------+------+---+-%s--+------+----------+-----+-----+\n", retBoardInfo->boardDisplayName) ;
    for (pin = 1 ; pin <= pinCount ; pin++) {
      debugReadallPhys (retBoardInfo->boardTypeId, pin) ;
    }
    return ;
  }

  /* Print 1 ~ 24/40 pins */
  printf (" +-----+-----+----------+------+---+-%s--+------+----------+-----+-----+\n", retBoardInfo->boardDisplayName) ;
  printf (" | BCM | wPi |   Name   | Mode | V | Physical | V | Mode | Name     | wPi | BCM |\n") ;
  printf (" +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+\n") ;
  for (pin = 1 ; pin <= pinCount ; pin += 2)
    readallPhys (retBoardInfo->boardTypeId, pin, 1) ;
  printf (" +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+\n") ;
  printf (" | BCM | wPi |   Name   | Mode | V | Physical | V | Mode | Name     | wPi | BCM |\n") ;
  printf (" +-----+-----+----------+------+---+-%s--+------+----------+-----+-----+\n", retBoardInfo->boardDisplayName) ;	
  printf ("\n");

  /* Print Second 1 ~ 12/24 pins */
  if (retBoardInfo->boardTypeId == NanoPi_M1 
    || retBoardInfo->boardTypeId == NanoPi_K1_Plus 
    || retBoardInfo->boardTypeId == NanoPi_M1_Plus 
    || retBoardInfo->boardTypeId == NanoPi_M1_Plus2) {
    // nothing to do.
  } else if (retBoardInfo->boardTypeId == NanoPi_NEO 
    || retBoardInfo->boardTypeId == NanoPi_NEO_Air 
    || retBoardInfo->boardTypeId == NanoPi_NEO2 
    || retBoardInfo->boardTypeId == NanoPi_NEO_Plus2) {
    printf (" +-----+----%s USB/Audio-+----+\n", retBoardInfo->boardDisplayName) ;
    printf (" | BCM | wPi |   Name   | Mode | V | Ph |\n") ;
    printf (" +-----+-----+----------+------+---+----+\n") ;
    for (pin = 25 ; pin <= 36 ; pin++) {
      readallPhys (retBoardInfo->boardTypeId, pin, 0) ;
    }
    printf (" +-----+-----+----------+------+---+----+\n") ;
	printf ("\n");
  } else if (retBoardInfo->boardTypeId == NanoPi_Duo 
    || retBoardInfo->boardTypeId == NanoPi_Duo2 
    || retBoardInfo->boardTypeId == NanoPi_NEO_Core 
    || retBoardInfo->boardTypeId == NanoPi_NEO_Core2) {
      // nothing to do.
  }

  if (retBoardInfo->boardTypeId == NanoPi_NEO_Core 
    || retBoardInfo->boardTypeId == NanoPi_NEO_Core2) {
    printf (" +-----+-----+----------+---- %s USB/Audio ----+----------+-----+-----+\n", retBoardInfo->boardDisplayName) ;
    printf (" | BCM | wPi |   Name   | Mode | V | Physical | V | Mode | Name     | wPi | BCM |\n") ;
    printf (" +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+\n") ;
    for (pin = 25 ; pin <= 48 ; pin += 2)
      readallPhys (retBoardInfo->boardTypeId, pin, 1) ;
    printf (" +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+\n") ;
    printf (" | BCM | wPi |   Name   | Mode | V | Physical | V | Mode | Name     | wPi | BCM |\n") ;
    printf (" +-----+-----+----------+------+---+-%s--+------+----------+-----+-----+\n", retBoardInfo->boardDisplayName) ; 
    printf ("\n");

    printf (" +-----+-----+----------+---- %s Network ----+----------+-----+-----+\n", retBoardInfo->boardDisplayName) ;
    printf (" | BCM | wPi |   Name   | Mode | V | Physical | V | Mode | Name     | wPi | BCM |\n") ;
    printf (" +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+\n") ;
    for (pin = 49 ; pin <= 68 ; pin += 2)
      readallPhys (retBoardInfo->boardTypeId, pin, 1) ;
    printf (" +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+\n") ;
    printf (" | BCM | wPi |   Name   | Mode | V | Physical | V | Mode | Name     | wPi | BCM |\n") ;
    printf (" +-----+-----+----------+------+---+-%s--+------+----------+-----+-----+\n", retBoardInfo->boardDisplayName) ; 
    printf ("\n");
  }

  if (retBoardInfo->boardTypeId == NanoPi_M1 
    || retBoardInfo->boardTypeId == NanoPi_K1_Plus 
    || retBoardInfo->boardTypeId == NanoPi_M1_Plus 
    || retBoardInfo->boardTypeId == NanoPi_M1_Plus2) {
    printf (" +-----+----%s Debug UART-+----+\n", retBoardInfo->boardDisplayName) ;
    printf (" | BCM | wPi |   Name   | Mode | V | Ph |\n") ;
    printf (" +-----+-----+----------+------+---+----+\n") ;
    for (pin = 41 ; pin < 45 ; pin++) {
      readallPhys (retBoardInfo->boardTypeId, pin, 0) ;
    }
    printf (" +-----+-----+----------+------+---+----+\n") ;
	printf ("\n");
  } else if (retBoardInfo->boardTypeId == NanoPi_NEO 
    || retBoardInfo->boardTypeId == NanoPi_NEO_Air 
    || retBoardInfo->boardTypeId == NanoPi_NEO2 
    || retBoardInfo->boardTypeId == NanoPi_NEO_Plus2) {
    printf (" +-----+----%s Debug UART-+----+\n", retBoardInfo->boardDisplayName) ;
    printf (" | BCM | wPi |   Name   | Mode | V | Ph |\n") ;
    printf (" +-----+-----+----------+------+---+----+\n") ;
    for (pin = 37 ; pin <= 38 ; pin++) {
      readallPhys (retBoardInfo->boardTypeId, pin, 0) ;
    }
    printf (" +-----+-----+----------+------+---+----+\n") ;
	printf ("\n");
  } else if (retBoardInfo->boardTypeId == NanoPi_Duo 
    || retBoardInfo->boardTypeId == NanoPi_Duo2 
    || retBoardInfo->boardTypeId == NanoPi_NEO_Core 
    || retBoardInfo->boardTypeId == NanoPi_NEO_Core2) {
      // nothing to do.
  }
}

void doReadall (void)
{
  if (wiringPiNodes != NULL) // External readall
  {
    doReadallExternal () ;
    return ;
  }

  NanoPiReadAll();
}
