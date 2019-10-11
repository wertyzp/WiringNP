# WiringNP
This is a GPIO access library for NanoPi. It is based on the WiringOP for Orange PI which is based on original WiringPi for Raspberry Pi.

Currently supported boards:  
NanoPi Neo  
NanoPi Neo Air  
NanoPi Duo  
NanoPi Duo2  
NanoPi NEO2  
NanoPi NEO Plus2  
NanoPi M1  
NanoPi M1 Plus  
NanoPi NEO Core  
NanoPi NEO Core2  
NanoPi K1 Plus  
NanoPC T3  
  
# Installation

## Install WiringNP 
Log into your nano board via SSH, open a terminal and install the WiringNP library by running the following commands:
```
git clone https://github.com/friendlyarm/WiringNP
cd WiringNP/
chmod 755 build
./build
```

# Verify WiringNP
The WiringNP library contains a set of gpio commands. Users can use them to access the GPIO pins on a nano board. You can verify your WiringNP by running the following command:
```
gpio readall
```
If your installation is successful the following messages will show up. Here is the message list for NanoPi NEO Core:
```   
root@FriendlyARM:~# gpio readall
  +-----+-----+----------+------+---+-NanoPi-NEO-Core--+------+----------+-----+-----+
 | BCM | wPi |   Name   | Mode | V | Physical | V | Mode | Name     | wPi | BCM |
 +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+
 |     |     |     3.3v |      |   |  1 || 2  |   |      | 5v       |     |     |
 |  12 |   8 |  GPIOA12 | ALT5 | 0 |  3 || 4  |   |      | 5v       |     |     |
 |  11 |   9 |  GPIOA11 | ALT5 | 0 |  5 || 6  |   |      | 0v       |     |     |
 | 203 |   7 |  GPIOG11 |  OUT | 1 |  7 || 8  | 0 | ALT5 | GPIOG6   | 15  | 198 |
 |     |     |       0v |      |   |  9 || 10 | 0 | ALT5 | GPIOG7   | 16  | 199 |
 |   0 |   0 |   GPIOA0 |  OUT | 0 | 11 || 12 | 1 | OUT  | GPIOA6   | 1   | 6   |
 |   2 |   2 |   GPIOA2 |  OFF | 0 | 13 || 14 |   |      | 0v       |     |     |
 |   3 |   3 |   GPIOA3 |  OFF | 0 | 15 || 16 | 0 | OFF  | GPIOG8   | 4   | 200 |
 |     |     |     3.3v |      |   | 17 || 18 | 0 | ALT2 | GPIOG9   | 5   | 201 |
 |  64 |  12 |   GPIOC0 | ALT4 | 0 | 19 || 20 |   |      | 0v       |     |     |
 |  65 |  13 |   GPIOC1 | ALT4 | 0 | 21 || 22 | 1 | OUT  | GPIOA1   | 6   | 1   |
 |  66 |  14 |   GPIOC2 | ALT4 | 0 | 23 || 24 | 1 | OUT  | GPIOC3   | 10  | 67  |
 +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+
 | BCM | wPi |   Name   | Mode | V | Physical | V | Mode | Name     | wPi | BCM |
 +-----+-----+----------+------+---+-NanoPi-NEO-Core--+------+----------+-----+-----+

 +-----+-----+----------+---- NanoPi-NEO-Core USB/Audio ----+----------+-----+-----+
 | BCM | wPi |   Name   | Mode | V | Physical | V | Mode | Name     | wPi | BCM |
 +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+
 |     |     |       5v |      |   | 25 || 26 | 0 | ALT4 | GPIOA15  | 22  | 15  |
 |     |     |     USB1 |      |   | 27 || 28 | 0 | ALT4 | GPIOA16  | 23  | 16  |
 |     |     |     USB1 |      |   | 29 || 30 | 0 | ALT4 | GPIOA14  | 21  | 14  |
 |     |     |     USB2 |      |   | 31 || 32 | 0 | ALT4 | GPIOA13  | 20  | 13  |
 |     |     |     USB2 |      |   | 33 || 34 |   |      | Mic      |     |     |
 |     |     |       IR |      |   | 35 || 36 |   |      | Mic      |     |     |
 |  17 |  19 |  GPIOA17 |  OFF | 0 | 37 || 38 |   |      | Audio    |     |     |
 |     |     |      I2S |      |   | 39 || 40 |   |      | Audio    |     |     |
 |     |     |      I2S |      |   | 41 || 42 | 0 | ALT5 | GPIOA5   | 18  | 5   |
 |     |     |      I2S |      |   | 43 || 44 | 0 | ALT5 | GPIOA4   | 17  | 4   |
 |     |     |      I2S |      |   | 45 || 46 |   |      | 5V       |     |     |
 |     |     |       0v |      |   | 47 || 48 |   |      | 0v       |     |     |
 +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+
 | BCM | wPi |   Name   | Mode | V | Physical | V | Mode | Name     | wPi | BCM |
 +-----+-----+----------+------+---+-NanoPi-NEO-Core--+------+----------+-----+-----+

 +-----+-----+----------+---- NanoPi-NEO-Core Network ----+----------+-----+-----+
 | BCM | wPi |   Name   | Mode | V | Physical | V | Mode | Name     | wPi | BCM |
 +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+
 |     |     |      Eth |      |   | 49 || 50 |   |      | Eth      |     |     |
 |     |     |      Eth |      |   | 51 || 52 |   |      | Eth      |     |     |
 |     |     |      Eth |      |   | 53 || 54 |   |      | Eth      |     |     |
 |     |     |       NC |      |   | 55 || 56 |   |      | NC       |     |     |
 |     |     |       NC |      |   | 57 || 58 |   |      | NC       |     |     |
 |     |     |       0v |      |   | 59 || 60 |   |      | 0v       |     |     |
 |     |     |     USB3 |      |   | 61 || 62 | 0 | OFF  | GPIOA7   | 24  | 7   |
 |     |     |     USB3 |      |   | 63 || 64 |   |      | GPIOE12  |     |     |
 |     |     |       5v |      |   | 65 || 66 |   |      | GPIOE13  |     |     |
 |     |     |       5v |      |   | 67 || 68 |   |      | 3.3v     |     |     |
 +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+
 | BCM | wPi |   Name   | Mode | V | Physical | V | Mode | Name     | wPi | BCM |
 +-----+-----+----------+------+---+-NanoPi-NEO-Core--+------+----------+-----+-----+
```
# Code Sample with WiringNP
connect a LED module to a NanoPi (Pin7), Make a C source file:
```
vi test.c
```
Type the following lines:
```
#include <wiringPi.h>
int main(void)
{
  wiringPiSetup() ;
  pinMode (7, OUTPUT) ;
  for(;;)
  {
    digitalWrite(7, HIGH) ;
    delay (500) ;
    digitalWrite(7,  LOW) ;
    delay (500) ;
  }
}
```
Compile and run "test.c":
```
gcc -Wall -o test test.c -lwiringPi -lpthread
sudo ./test
```
You can see the LED is blinking.

# PWM Code Sample
The PWM pin in NanoPi NEO/NEO2 is multiplexing which can be set to either PWM or SerialPort0. To set this pin to PWM you need to run "sudo npi-config" and enter the "Advanced Options" menu to Enable/Disable PWM. Note: after PWM is enabled SerialPort0 will not be accessed and you need to login your board via SSH.  
  
connect a Buzzer to a NanoPi NEO2, create a source file in C:
```
vi pwmtest.c
```
Type the following code:
```
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
int main (void)
{
  int l ;
  printf ("PWM test program\n") ;
 
  //using wiringPi Pin Number
  int pin = 18;              
  if (wiringPiSetup () == -1)
    exit (1) ;               
 
  /*
  //using Physical Pin Number
  int pin = 38;              
  if (wiringPiSetupPhys() == -1)
    exit (1) ;                  
  */                            
 
  /*
  //using BCM Pin Number
  int pin = 5;          
  if (wiringPiSetupGpio() == -1)
    exit (1);                   
  */                            
 
  pinMode (pin, PWM_OUTPUT);
  for (;;) {                      
    for (l = 0 ; l < 1024 ; ++l) {
      pwmWrite (pin, l) ;         
      delay (1) ;        
    }                              
    for (l = 1023 ; l >= 0 ; --l) {
      pwmWrite (pin, l) ;          
      delay (1) ;        
    }            
  }         
  return 0 ;
}
```
Compile the pwmtest.c file and run the generated executable:
```
gcc -Wall -o pwmtest pwmtest.c -lwiringPi -lpthread
./pwmtest
```
Connect a PWM beeper to a NEO/NEO2 and the beeper will sound.
