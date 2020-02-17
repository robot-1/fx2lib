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
// #include "../../include/fx2sdly.h"
#ifdef DEBUG_FIRMWARE
#include <stdio.h>
#else
#define printf(...)
#endif

#define SYNCDELAY SYNCDELAY4

BOOL handle_get_descriptor() {
    return FALSE;
}

//************************** Configuration Handlers *****************************

// change to support as many interfaces as you need
//volatile xdata BYTE interface=0;
//volatile xdata BYTE alt=0; // alt interface

// set *alt_ifc to the current alt interface for ifc
BOOL handle_get_interface(BYTE ifc, BYTE* alt_ifc) {
// *alt_ifc=alt;
 printf ( "Get Interface\n" );
 if (ifc==0) {*alt_ifc=0; return TRUE;} else { return FALSE;}

 return TRUE;
}
// return TRUE if you set the interface requested
// NOTE this function should reconfigure and reset the endpoints
// according to the interface descriptors you provided.
BOOL handle_set_interface(BYTE ifc,BYTE alt_ifc) {  
 printf ( "Set Interface.\n" );
 //interface=ifc;
 //alt=alt_ifc;
 if (ifc==0&&alt_ifc==0) {
    // SEE TRM 2.3.7
    // reset toggles
    RESETTOGGLE(0x02);
    RESETTOGGLE(0x86);
    // restore endpoints to default condition
    RESETFIFO(0x02);
    EP2BCL=0x80;
    SYNCDELAY;
    EP2BCL=0X80;
    SYNCDELAY;
    RESETFIFO(0x86);
    return TRUE;
 } else{
    return FALSE;
 }

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
 //return TRUE;
 return cfg==1 ? TRUE : FALSE; // we only handle cfg 1

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

static void accept_cmd(void)
{
    __xdata const unsigned char *src = EP2FIFOBUF;
    unsigned len = ((unsigned)EP2BCH)<<8 | EP2BCL;
    if ( len < 1 )
    {
        return;
    }
    PA0 = *src & 1;
    PA1 = *src & 2;
    OUTPKTEND = 0x82;
}
static void 
send_state(void)
{
    __xdata unsigned char *dest = EP6FIFOBUF;
    const char *msg1 = PA0 ? "PA0=1" : "PA0=0";
    const char *msg2 = PA1 ? "PA1=1" : "PA1=0";
    unsigned char len=0;

    while (*msg1)
    {
            *dest++ = *msg++;
            ++len;
    }
    *dest++ = ',';
    ++len;
    while ( *msg2 )
    {
        *dest++ = *msg2++;
        ++len;
    }

    SYNCDELAY;
    EP6BCH=0;
    SYNCDELAY;
    EP6BCL=len;
}

void main_init() {

 REVCTL=3;
 SETIF48MHZ();
 OEA = 0x03;

 // set IFCONFIG
 // config your endpoints etc.
 // config gpif
 
 REVCTL=0;

 
 printf ( "Initialization Done.\n" );

}


void main_loop() {
 // do some work
}


