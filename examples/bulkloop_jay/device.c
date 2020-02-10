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
#include <stdio.h>
#include <fx2regs.h>
#include <serial.h>
#include <delay.h>
#include <autovector.h>
#include <lights.h>
#ifdef DEBUG_FIRMWARE
#include <stdio.h>
#else
#define printf(...)
#endif

BOOL handle_get_descriptor() {

}

//************************** Configuration Handlers *****************************

// change to support as many interfaces as you need
//volatile xdata BYTE interface=0;
//volatile xdata BYTE alt=0; // alt interface

// set *alt_ifc to the current alt interface for ifc
BOOL handle_get_interface(BYTE ifc, BYTE* alt_ifc) {
// *alt_ifc=alt;
 return TRUE;
}
// return TRUE if you set the interface requested
// NOTE this function should reconfigure and reset the endpoints
// according to the interface descriptors you provided.
BOOL handle_set_interface(BYTE ifc,BYTE alt_ifc) {  
 printf ( "Set Interface.\n" );
 //interface=ifc;
 //alt=alt_ifc;
 return TRUE;
}

// handle getting and setting the configuration
// 1 is the default.  If you support more than one config
// keep track of the config number and return the correct number
// config numbers are set int the dscr file.
//volatile BYTE config=1;
BYTE handle_get_configuration() { 
 return 1;
}

// NOTE changing config requires the device to reset all the endpoints
BOOL handle_set_configuration(BYTE cfg) { 
 printf ( "Set Configuration.\n" );
 //config=cfg;
 return TRUE;
}


//******************* VENDOR COMMAND HANDLERS **************************


BOOL handle_vendorcommand(BYTE cmd) {
 // your custom vendor handler code here..
 return FALSE; // not handled by handlers
}


//********************  INIT ***********************


const static void initialize(void){
    CPUCS = 0x10; // 48 MHZ, CLKOUT disabled
    SYNCDELAY;
    IFCONFIG = 0xc0;    // Internal IFCLK @ 48MHz
    SYNCDELAY;
    REVCTL = 0x03;      // Disable auto-arm + Enhanced packet handling
    SYNCDELAY;
    EP6CFG = 0xE2;      // 1110_0010 bulk IN, 512 bytes, double-buffered
    SYNCDELAY;
    EP2CFG = 0xA2;      // 1010_0010 bulk OUT, 512 bytes, double-buffered
    SYNCDELAY;
    FIFORESET = 0x80;   // NAK all requests from host.
    SYNCDELAY;
    FIFORESET = 0x82;   // Reset EP 2
    SYNCDELAY;
    FIFORESET = 0x84;   // Reset EP 4..
    SYNCDELAY;
    FIFORESET = 0x86;
    SYNCDELAY;
    FIFORESET = 0x88;
    SYNCDELAY;
    FIFORESET = 0x00;   // Back to normal..
    SYNCDELAY;
    EP2FIFOCFG = 0x00;  // Disable AUTOOUT
    SYNCDELAY;
    OUTPKTEND = 0x82;   // Clear the 1st buffer
    SYNCDELAY;
    OUTPKTEND = 0x82;   // ..both of them
    SYNCDELAY;


}


void main_init() {

 REVCTL=3;
 SETIF48MHZ();

 // set IFCONFIG
 // config your endpoints etc.
 // config gpif
 
 REVCTL=0;

 
 printf ( "Initialization Done.\n" );

}


void main_loop() {
 // do some work
}


