# WiringNP
This is a GPIO access library for NanoPi. It is based on the WiringOP for Orange PI which is based on original WiringPi for Raspberry Pi.

Currently supported boards:  
NanoPI Neo  
NanoPi NEO2  
NanoPI M1  

# Installation

## Install WiringNP on NEO/NEO2
Log into your nano board via SSH, open a terminal and install the WiringNP library by running the following commands:
```
git clone https://github.com/friendlyarm/WiringNP
cd WiringNP/
chmod 755 build
./build
```

## Install WiringNP on NanoPi M1
Log into your nano board via SSH, open a terminal and install the WiringNP library by running the following commands:
```
git clone https://github.com/friendlyarm/WiringNP
cd WiringNP/
git checkout nanopi-m1
chmod 755 build
./build
```

# Verify WiringNP
The WiringNP library contains a set of gpio commands. Users can use them to access the GPIO pins on a nano board. You can verify your WiringNP by running the following command:
```
gpio readall
```
If your installation is successful the following messages will show up. Here is the message list for NEO2:
```   
root@FriendlyARM:~# gpio readall
  +-----+-----+----------+------+---+-NanoPi NEO/NEO2--+------+----------+-----+-----+
 | BCM | wPi |   Name   | Mode | V | Physical | V | Mode | Name     | wPi | BCM |
 +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+
 |     |     |     3.3V |      |   |  1 || 2  |   |      | 5V       |     |     |
 |  12 |   8 |  GPIOA12 |  OFF | 0 |  3 || 4  |   |      | 5V       |     |     |
 |  11 |   9 |  GPIOA11 |  OFF | 0 |  5 || 6  |   |      | 0v       |     |     |
 | 203 |   7 |  GPIOG11 |  OUT | 1 |  7 || 8  | 0 | OFF  | GPIOG6   | 15  | 198 |
 |     |     |       0v |      |   |  9 || 10 | 0 | OFF  | GPIOG7   | 16  | 199 |
 |   0 |   0 |   GPIOA0 |  OFF | 0 | 11 || 12 | 0 | OFF  | GPIOA6   | 1   | 6   |
 |   2 |   2 |   GPIOA2 |  OFF | 0 | 13 || 14 |   |      | 0v       |     |     |
 |   3 |   3 |   GPIOA3 |  OFF | 0 | 15 || 16 | 0 | OFF  | GPIOG8   | 4   | 200 |
 |     |     |     3.3v |      |   | 17 || 18 | 0 | OFF  | GPIOG9   | 5   | 201 |
 |  64 |  12 |   GPIOC0 |  OFF | 0 | 19 || 20 |   |      | 0v       |     |     |
 |  65 |  13 |   GPIOC1 |  OFF | 0 | 21 || 22 | 0 | OFF  | GPIOA1   | 6   | 1   |
 |  66 |  14 |   GPIOC2 |  OFF | 0 | 23 || 24 | 0 | OFF  | GPIOC3   | 10  | 67  |
 +-----+-----+----------+------+---+----++----+---+------+----------+-----+-----+
 | BCM | wPi |   Name   | Mode | V | Physical | V | Mode | Name     | wPi | BCM |
 +-----+-----+----------+------+---+-NanoPi NEO/NEO2--+------+----------+-----+-----+
 
 +-----+----NanoPi NEO/NEO2 Debug UART-+----+
 | BCM | wPi |   Name   | Mode | V | Ph |
 +-----+-----+----------+------+---+----+
 |   4 |  17 |   GPIOA4 | ALT5 | 0 | 37 |
 |   5 |  18 |   GPIOA5 | ALT4 | 0 | 38 |
 +-----+-----+----------+------+---+----+
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
