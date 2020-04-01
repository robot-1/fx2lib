/**
 * Copyright (C) 2009 Ubixum, Inc. 
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 **/

#include <fx2macros.h>
#include <fx2ints.h>
#include <autovector.h>
#include <delay.h>
#include <setupdat.h>

#ifdef DEBUG_FIRMWARE 
#include <serial.h>
#include <stdio.h>
#else
#define printf(...)
#endif




volatile __bit dosud=FALSE;
volatile __bit dosuspend=FALSE;

// custom functions
extern void main_loop();
extern void main_init();


void main() {

#ifdef DEBUG_FIRMWARE
 SETCPUFREQ(CLK_48M); // required for sio0_init 
 // main_init can still set this to whatever you want.
 sio0_init(57600); // needed for printf if debug defined 
#endif

/* input by Jean Jay
__data  --> for small Memory Model,stored at directly addressable portion of the internal RAM of a 8051
__xdata --> for external RAM, for Large Memory Model
__idata --> for indirectly addressable portion of internal RAM of 8051
__pdata --> paged xdata, for medium memory model, max 256 bytes 
__code  --> will be placed in the code memory
__bit   --> a data type and address space,wil be place in the bit addressable memory of 8051
__sfr
__sfr16
__sfr32
__sbit  --> both data-type and named address space, describes special function registers and spcecial __bit variable of 8051

Special Function Registers located on an address dividable by 8 are bit-addressable,an __sbit addresses a
 specific bit within these sfr.

pointer physically in internal ram pointing to object in external ram
  __xdata unsigned char * __data p;

pointer physically in external ram pointing to object in internal ram
  __data unsigned char * __xdata p;

pointer physically in code rom pointing to data in xdata space
  __xdata unsigned char * __code p;

pointer physically in code space pointing to data in code space
  __code unsigned char * __code p;

generic pointer physically located in xdata space
  unsigned char * __xdata p;

generic pointer physically located in default memory space
  unsigned char * p;

the following is a function pointer physically located in data space
  char (* __data fp)(void);


MCS Memory Layout
Minimum of 128 bytes internal RAM
- Bytes 00-1F - 32 bytes to hold up to 4 banks of the registers R0 to R7,
- Bytes 20-2F - 16 bytes to hold 128 bit variables and,
- Bytes 30-7F - 80 bytes for general purpose use.

Additional 128 bytes, indirectly addressable, internal RAM memory (__data) --> only to some members of MCS51
Some chips have built in external memory (__xdata) which should not be confused with the internal, directly addressable
 RAM memory (__data)


*/




 main_init();

 // set up interrupts.
 USE_USB_INTS();
 
 ENABLE_SUDAV(); //Setup Data Available **
 ENABLE_USBRESET();
 ENABLE_HISPEED(); 
 ENABLE_SUSPEND();
 ENABLE_RESUME();

 EA=1;

// iic files (c2 load) don't need to renumerate/delay
// trm 3.6
#ifndef NORENUM
 RENUMERATE();
#else
 USBCS &= ~bmDISCON;
#endif
 
 while(TRUE) {

     main_loop(); // purt todos inside this function in the device.c file

     if (dosud) { // do setup device
       dosud=FALSE;
       handle_setupdata();
     }

     if (dosuspend) {
        dosuspend=FALSE;
        do {
           printf ( "I'm going to Suspend.\n" );
           WAKEUPCS |= bmWU|bmWU2; // make sure ext wakeups are cleared
           SUSPEND=1;
           PCON |= 1;
           __asm
           nop
           nop
           nop
           nop
           nop
           nop
           nop
           __endasm;

        } while ( !remote_wakeup_allowed && REMOTE_WAKEUP()); 
        printf ( "I'm going to wake up.\n");

        // resume
        // trm 6.4
        if ( REMOTE_WAKEUP() ) {
            delay(5);
            USBCS |= bmSIGRESUME;
            delay(15);
            USBCS &= ~bmSIGRESUME;
        }

     }

 } // end while

} // end main

void resume_isr() __interrupt RESUME_ISR {
 CLEAR_RESUME();
}
  
void sudav_isr() __interrupt SUDAV_ISR {
 dosud=TRUE;
 CLEAR_SUDAV();
}
void usbreset_isr() __interrupt USBRESET_ISR {
 handle_hispeed(FALSE);
 CLEAR_USBRESET();
}
void hispeed_isr() __interrupt HISPEED_ISR {
 handle_hispeed(TRUE);
 CLEAR_HISPEED();
}

void suspend_isr() __interrupt SUSPEND_ISR {
 dosuspend=TRUE;
 CLEAR_SUSPEND();
}
