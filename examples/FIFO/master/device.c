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
#include <setupdat.h>
#include <fx2sdly.h>
#include <eputils.h>
#include <gpif.h>
#ifdef DEBUG_FIRMWARE
#include <stdio.h>
#else
#define printf(...)
#endif

BOOL handle_get_descriptor() {
    return FALSE;

}

//******************** Configuration Handlers *************************

// change to support as many interfaces as you need
//volatile xdata BYTE interface=0;
//volatile xdata BYTE alt=0; // alt interface

// set *alt_ifc to the current alt interface for ifc
BOOL handle_get_interface(BYTE ifc, BYTE* alt_ifc) {
// *alt_ifc=alt;
 printf ( "Get Interface\n" );
 if(ifc==0){
    *alt_ifc=0; return TRUE;
 }else{ 
    return FALSE;
  }
}
// return TRUE if you set the interface requested
// NOTE this function should reconfigure and reset the endpoints
// according to the interface descriptors you provided.
BOOL handle_set_interface(BYTE ifc,BYTE alt_ifc) {
 printf ( "Set Interface.\n" );
 //interface=ifc;
 //alt=alt_ifc;
  printf ( "Set interface %d to alt: %d\n" , ifc, alt_ifc );

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
 return cfg==1 ? TRUE : FALSE; // we only handle cfg 1}
}


//******************* VENDOR COMMAND HANDLERS **************************

#define VC_EPSTAT 0xB1
BOOL handle_vendorcommand(BYTE cmd) {
 // your custom vendor handler code here..
  switch ( cmd ) {

     case VC_EPSTAT:
        {
         __xdata BYTE* pep= ep_addr(SETUPDAT[2]);
         printf ( "ep %02x\n" , *pep );
         if (pep) {
          EP0BUF[0] = *pep;
          EP0BCH=0;
          EP0BCL=1;
          return TRUE;
         }
        }
     default:
          printf ( "Need to implement vendor command: %02x\n", cmd );
 }{
     return FALSE;
 }
}

const static void initialize(void){
    // 48 MHZ, CLKOUT enabled
    CPUCS = bmCLKSPD1;
    SYNCDELAY;
    // Internal IFCLK @ 48MHz, IFCLK out,GPIF MASTER
    IFCONFIG = bmIFCLKSRC | bm3048MHZ | bmIFCLKOE | bmIFCFG1;
    SYNCDELAY;
    // Disable auto-arm + Enhanced packet handling
    REVCTL = bmNOAUTOARM | bmSKIPCOMMIT;
    SYNCDELAY;
    // bulk IN, 512 bytes, double-buffered 0xE2
    EP6CFG = bmVALID | bmDIR | bmTYPE1 | bmBUF1;
    SYNCDELAY;
    // bulk OUT, 512 bytes, double-buffered 0xA2
    EP2CFG =  bmVALID | bmTYPE1 | bmBUF1;
    SYNCDELAY;
    // NAK all requests from host. 0x80
    FIFORESET = bmNAKALL;
    SYNCDELAY;
    // Reset EP 2 0x82
    FIFORESET = bmBIT7 | bmBIT1;
    SYNCDELAY;
    // Reset EP 4..
    FIFORESET = 0x84;
    SYNCDELAY;
    FIFORESET = 0x86;
    SYNCDELAY;
    FIFORESET = 0x88;
    SYNCDELAY;
    // Back to normal..
    FIFORESET = 0x00;
    SYNCDELAY;
    // Disable AUTOOUT
    EP2FIFOCFG = 0x00;
    SYNCDELAY;
    // Clear the 1st buffer
    OUTPKTEND = 0x82;
    SYNCDELAY;
    // ..both of them
    OUTPKTEND = 0x82;
    SYNCDELAY;
    INPKTEND = 0x86;
    SYNCDELAY;
    INPKTEND = 0x86;
    SYNCDELAY;
    // manual IN, 8 bit data bmAUTOIN
    EP6FIFOCFG = 0x00;
    SYNCDELAY;

}
//********************  INIT ***********************

void main_init() {

const char __xdata WaveData[128] =
{
// Wave 0
/* LenBr */ 0x01,     0x01,     0x01,     0x01,     0x01,     0x01,     0x01,     0x07,
/* Opcode*/ 0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,
/* Output*/ 0x07,     0x07,     0x07,     0x07,     0x07,     0x07,     0x07,     0x07,
/* LFun  */ 0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x3F,
// Wave 1
/* LenBr */ 0x01,     0x01,     0x01,     0x01,     0x01,     0x01,     0x01,     0x07,
/* Opcode*/ 0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,
/* Output*/ 0x07,     0x07,     0x07,     0x07,     0x07,     0x07,     0x07,     0x07,
/* LFun  */ 0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x3F,
// Wave 2
/* LenBr */ 0x02,     0x01,     0x93,     0x01,     0xBA,     0x01,     0x01,     0x07,
/* Opcode*/ 0x00,     0x00,     0x01,     0x02,     0x01,     0x00,     0x00,     0x00,
/* Output*/ 0x07,     0x07,     0x04,     0x04,     0x07,     0x07,     0x07,     0x07,
/* LFun  */ 0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x3F,
//
/* LenBr */ 0x01,     0x01,     0x01,     0x9B,     0x01,     0x01,     0x01,     0x07,
/* Opcode*/ 0x00,     0x00,     0x00,     0x01,     0x00,     0x00,     0x00,     0x00,
/* Output*/ 0x05,     0x06,     0x07,     0x07,     0x07,     0x07,     0x07,     0x07,
/* LFun  */ 0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x3F,
};
const char __xdata FlowStates[36] =
{
/* Wave 0 FlowStates */ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* Wave 1 FlowStates */ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* Wave 2 FlowStates */ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* Wave 3 FlowStates */ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
const char __xdata InitData[7] =
{
/* Regs  */ 0xE0,0x10,0x00,0x07,0xFE,0x4E,0x00
};
 OEA = 0x01;
 PA0 = 0;
 initialize();
 gpif_init(WaveData,InitData);
 gpif_setflowstate(FlowStates,0); // bank 0
 gpif_set_tc16(4);
 printf ( "Initialization Done.\n" );

}

void main_loop() {
// do some work
gpif_fifo_read(GPIF_EP6); // data to EP6

// unsigned len = ((unsigned)EP6BCH) << 8 | EP6BCL;

// if(len < 1) return;
if(!(EP6CS & bmEPFULL)) return;
PA0 = 1;
}


