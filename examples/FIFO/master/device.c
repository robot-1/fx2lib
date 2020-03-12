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
//#include <lights.h>
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
     // set the CPU clock to 48MHz
  //CPUCS = ((CPUCS & ~bmCLKSPD) | bmCLKSPD1);
  CPUCS = 0x08;                 // CLKSPD[1:0]=10, for 48MHz operation
  SYNCDELAY;                    // CLKOE=0, don't drive CLKOUT
  IFCONFIG = 0b10001010;
  SYNCDELAY;

  REVCTL = 0x03;
  SYNCDELAY;
  EP2CFG = 0b10100010;//bmVALID | bmTYPE1 | bmBUF1;
  SYNCDELAY;           
  EP6CFG = 0b11100010;     // EP6IN, bulk, size 512, 2x buffered
  SYNCDELAY;
  EP4CFG = 0x00;     // EP4 not valid
  SYNCDELAY;               
  EP8CFG = 0x00;     // EP8 not valid
  SYNCDELAY;




//   EP6GPIFFLGSEL = 0x01; // For EP6IN, GPIF uses EF flag
//   SYNCDELAY;

  FIFORESET = 0x80;  // set NAKALL bit to NAK all transfers from host
  SYNCDELAY;
  FIFORESET = 0x02;  // reset EP2 FIFO
  SYNCDELAY;
  FIFORESET = 0x06;  // reset EP6 FIFO
  SYNCDELAY;
  FIFORESET = 0x04;  // reset EP6 FIFO
  SYNCDELAY;
  FIFORESET = 0x08;  // clear NAKALL bit to resume normal operation
  SYNCDELAY;
  FIFORESET = 0x00;  // clear NAKALL bit to resume normal operation
  SYNCDELAY;

//   INPKTEND = 0x86;
//   SYNCDELAY;
//   INPKTEND = 0x86;
//   SYNCDELAY;

   // EP6BCL = 0x80;   // arm EP2OUT by writing byte count w/skip. (E691)
   // SYNCDELAY;
   // EP6BCL = 0x80;
   // SYNCDELAY;
  OUTPKTEND = 0x82; // SKIP OUT data
  SYNCDELAY;
  OUTPKTEND = 0x82; // SKIP OUT data
  REVCTL = 0x00;
  EP2FIFOCFG = 0x00; 
  SYNCDELAY;
  EP6FIFOCFG = 0x08; 
  SYNCDELAY;  
  EP4FIFOCFG = 0x00; 
  SYNCDELAY;
  EP8FIFOCFG = 0x00;
  SYNCDELAY;
    
}

static void
send_state(__xdata const unsigned char *msg,unsigned int data_len){
    __xdata unsigned char *dest = EP6FIFOBUF;
    
   const char *mss = "hello";
   unsigned int len = 0;
   *dest++ = 0x68;
   ++len;
    while(*mss > 0){
        *dest++ = *mss++;
        ++len;
    }

    EP6BCH= len & 0xFF00;
    SYNCDELAY;
    EP6BCL= len & 0x00FF;

}


static void
accept_cmd(void){
    __xdata const unsigned char *src = EP2FIFOBUF;
    unsigned len = ((unsigned)EP2BCH)<<8 | EP2BCL;

    if (len<1) return;

    if (!(EP6CS & bmEPFULL)) send_state(src,len);

    FIFORESET = 0x80;
    SYNCDELAY;
    FIFORESET = 0x02;
    SYNCDELAY;
    FIFORESET = 0x00;
    SYNCDELAY;
    OUTPKTEND = 0x82; // SKIP OUT data
    SYNCDELAY;
    OUTPKTEND = 0x82; // SKIP OUT data
    SYNCDELAY;
}

//********************  INIT ***********************
static BYTE g_data = 0x00;
BYTE __xdata mydata;
unsigned char state = 0;

WORD len = sizeof(g_data);
void main_init() {
const char __xdata WaveData[128] =
{ 
// Wave 0 
/* LenBr */ 0x01,     0x01,     0x01,     0x01,     0x01,     0x01,     0x01,     0x07,
/* Opcode*/ 0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,
/* Output*/ 0x01,     0x01,     0x01,     0x01,     0x01,     0x01,     0x01,     0x01,
/* LFun  */ 0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x3F,
// Wave 1 
/* LenBr */ 0x01,     0x01,     0x01,     0x01,     0x01,     0x01,     0x01,     0x07,
/* Opcode*/ 0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,
/* Output*/ 0x01,     0x01,     0x01,     0x01,     0x01,     0x01,     0x01,     0x01,
/* LFun  */ 0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x3F,
// Wave 2 
/* LenBr */ 0x02,     0x05,     0x9A,     0x14,     0x01,     0x17,     0x01,     0x07,
/* Opcode*/ 0x00,     0x00,     0x01,     0x00,     0x02,     0x01,     0x00,     0x00,
/* Output*/ 0x01,     0x01,     0x01,     0x00,     0x00,     0x01,     0x01,     0x01,
/* LFun  */ 0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x3F,
// Wave 3 
/* LenBr */ 0x01,     0x01,     0x01,     0x01,     0x01,     0x01,     0x01,     0x07,
/* Opcode*/ 0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,
/* Output*/ 0x01,     0x01,     0x01,     0x01,     0x01,     0x01,     0x01,     0x01,
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
/* Regs  */ 0xE0,0x10,0x00,0x01,0x8A,0x4E,0x00
};



 OEA = 0x01;
 IOA = 0x00;
 // SEnder CLK 416ns
 // master 33ns
 initialize();
 gpif_init(WaveData,InitData);

}

void main_loop() {
   if(GPIFREADYSTAT & 0x01){
      if (!(EP6CS & bmEPFULL)){
               gpif_set_tc16(512);   
               gpif_fifo_read(GPIF_EP6);
               while(!(GPIFTRIG & 0x80 ));
               IOA = 0x01;
               GPIFTCB0=01;  
               SYNCDELAY;  
               GPIFTCB1=00;  
               SYNCDELAY;  
               GPIFTCB2=00;  
               SYNCDELAY;  
               GPIFTCB3=00;  
               SYNCDELAY;
      }

   }
}
