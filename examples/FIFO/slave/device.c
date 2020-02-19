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

//************************** Configuration Handlers *****************************

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
//volatile BYTE config=1
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
    CPUCS = bmCLKSPD1; // 48 MHZ, CLKOUT disabled
    SYNCDELAY;
    IFCONFIG = /*bmIFCLKSRC | bmASYNC |*/ bm3048MHZ | bmIFCFGMASK;    // Internal IFCLK @ 48MHz, SLAVE FIFO, Synchronous
    SYNCDELAY;
    REVCTL = bmNOAUTOARM | bmSKIPCOMMIT;     // Disable auto-arm + Enhanced packet handling
    SYNCDELAY;

    EP6CFG = bmVALID | bmDIR | bmTYPE1 | bmBUF1;      // 1110_0010 bulk IN, 512 bytes, double-buffered
    SYNCDELAY;
    EP2CFG = bmVALID | bmTYPE1 | bmBUF1;     // 1010_0010 bulk OUT, 512 bytes, double-buffered
    SYNCDELAY;
    FIFORESET = bmNAKALL;   // NAK all requests from host.
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
    EP2FIFOCFG =  bmAUTOOUT; // AUto-OUT, 8 bit data
    

}

static void
send_state(__xdata const unsigned char *msg,unsigned int data_len){
    __xdata unsigned char *dest = EP6FIFOBUF;
    unsigned char len = 0;

    for(int i = 0;i<data_len;i++){
        *dest++ = *msg++;
    }
    EP6BCH= data_len & 0xFF00;
    SYNCDELAY;
    EP6BCL= data_len & 0x00FF;

}

static void
accept_cmd(void){
    __xdata const unsigned char *src = EP2FIFOBUF;
    unsigned len = ((unsigned)EP2BCH)<<8 | EP2BCL;

    if (len<1){
        return;
    }
    
    if (!(EP6CS & bmEPFULL)){
        send_state(src,len);
    }
    OUTPKTEND = 0x80;
    
    OUTPKTEND = 0x82;
    SYNCDELAY;
    OUTPKTEND = 0x82;
    
    
}



//********************  INIT ***********************

void main_init() {

 OEA = 0x03;
 initialize();


 PA0 = 1;
 PA1 = 1;
 printf ( "Initialization Done.\n" );

}


void main_loop() {

 if (!(EP2CS & bmEPEMPTY)){    
     PA0 ^= 1;
     accept_cmd();

 }



}


