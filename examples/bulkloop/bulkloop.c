
#include <stdio.h>

#include <fx2regs.h>
#include <fx2macros.h>
#include <serial.h>
#include <delay.h>
#include <usbjt.h>
#include <lights.h>
#include <setupdat.h>
#include <eputils.h>


#define SYNCDELAY() SYNCDELAY4
#define REARMVAL 0x80
#define REARM() EP2BCL=REARMVAL



volatile WORD bytes;
volatile bit gotbuf;
volatile BYTE icount;
volatile bit got_sud;
DWORD lcount;
bit on;

void main() {

 d2off();
 on=0;
 lcount=0;
 got_sud=FALSE;
 icount=0;
 gotbuf=FALSE;
 bytes=0;

 // renumerate
 RENUMERATE(); 
 

 SETCPUFREQ(CLK_48M);
 SETIF48MHZ();
 sio0_init(57600);
 
 
 USE_USB_INTS(); 
 ENABLE_SUDAV();
 ENABLE_SOF();
 ENABLE_HISPEED();
 ENABLE_USBRESET();
 
 
 // only valid endpoints are 2/6
 EP2CFG = 0xA2; // 10100010
 SYNCDELAY();
 EP6CFG = 0xE2; // 11100010 
 SYNCDELAY();
 EP1INCFG &= ~bmVALID;
 SYNCDELAY();
 EP1OUTCFG &= ~bmVALID;
 SYNCDELAY();
 EP4CFG &= ~bmVALID;
 SYNCDELAY();
 EP8CFG &= ~bmVALID;
 SYNCDELAY(); 
 
 
 // arm ep2
 EP2BCL = 0x80; // write once
 SYNCDELAY();
 EP2BCL = 0x80; // do it again

 
 // make it so we enumberate
 

 EA=1; // global interrupt enable 
 printf ( "Done initializing stuff\n" );

 
 d3off();
 
 while(TRUE) {
 
  if ( got_sud ) {
      printf ( "Handle setupdata\n" );
      handle_setupdata(); 
      got_sud=FALSE;
  }

  if ( !(EP2468STAT & bmEP2EMPTY) ) {
       printf ( "ep2 out received data\n" );
      if  ( !(EP2468STAT & bmEP6FULL) ) { // wait for at least one empty in buffer
                 WORD i;
                 printf ( "Sending data to ep6 in\n");
    
                 bytes = MAKEWORD(EP2BCH,EP2BCL);
                 
                 for (i=0;i<bytes;++i) EP6FIFOBUF[i] = EP2FIFOBUF[i];
                 
                 // can copy whole string w/ autoptr instead.
                 // or copy directly from one buf to another

                 // ARM ep6 out
                 EP6BCH=MSB(bytes);
                 SYNCDELAY();
                 EP6BCL=LSB(bytes); 

                 REARM(); // ep2
                 //printf ( "Re-Armed ep2\n" );

         }
   }
 }

}

// copied routines from setupdat.h

// value (low byte) = ep
#define VC_EPSTAT 0xB1

BOOL handle_vendorcommand(BYTE cmd) {

 switch ( cmd ) {
 
     case VC_EPSTAT:
        {         
         xdata BYTE* pep= ep_addr(SETUPDAT[2]);
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
 }
 return FALSE;
}

// this firmware only supports 0,0
BOOL handle_get_interface(BYTE ifc, BYTE* alt_ifc) { 
 printf ( "Get Interface\n" );
 if (ifc==0) {*alt_ifc=0; return TRUE;} else { return FALSE;}
}
BOOL handle_set_interface(BYTE ifc, BYTE alt_ifc) { 
 printf ( "Set interface %d to alt: %d\n" , ifc, alt_ifc );
 
 if (ifc==0&&alt_ifc==0) {
    // SEE TRM 2.3.7
    // reset toggles
    RESETTOGGLE(0x02);
    RESETTOGGLE(0x86);
    // restore endpoints to default condition
    RESETFIFO(0x02);
    EP2BCL=0x80;
    SYNCDELAY();
    EP2BCL=0X80;
    SYNCDELAY();
    RESETFIFO(0x86);
    return TRUE;
 } else 
    return FALSE;
}

// get/set configuration
BYTE handle_get_configuration() {
 return 1; 
 }
BOOL handle_set_configuration(BYTE cfg) { 
 return cfg==1 ? TRUE : FALSE; // we only handle cfg 1
}


// copied usb jt routines from usbjt.h
void sudav_isr() interrupt SUDAV_ISR {
  
  got_sud=TRUE;
  CLEAR_SUDAV();
}

bit on5;
xdata WORD sofct=0;
void sof_isr () interrupt SOF_ISR using 1 {
    ++sofct;
    if(sofct==8000) { // about 8000 sof interrupts per second at high speed
        on5=!on5;
        if (on5) d5on(); else d5off();
        sofct=0;
    }
    CLEAR_SOF();
}

void sutok_isr() interrupt SUTOK_ISR {}
void suspend_isr() interrupt SUSPEND_ISR {}
void usbreset_isr() interrupt USBRESET_ISR {
    CLEAR_USBRESET();
}
void hispeed_isr() interrupt HISPEED_ISR {
    handle_hispeed();
    CLEAR_HISPEED();
}
void ep0ack_isr() interrupt EP0ACK_ISR {}
void ep0in_isr() interrupt EP0IN_ISR {}
void ep0out_isr() interrupt EP0OUT_ISR {}
void ep1in_isr() interrupt EP1IN_ISR {}
void ep1out_isr() interrupt EP1OUT_ISR {}
void ep2_isr() interrupt EP2_ISR {}
void ep4_isr() interrupt EP4_ISR {}
void ep6_isr() interrupt EP6_ISR {}
void ep8_isr() interrupt EP8_ISR {}
void ibn_isr() interrupt IBN_ISR {}
void ep0ping_isr() interrupt EP0PING_ISR {}
void ep1ping_isr() interrupt EP1PING_ISR {}
void ep2ping_isr() interrupt EP2PING_ISR {}
void ep4ping_isr() interrupt EP4PING_ISR {}
void ep6ping_isr() interrupt EP6PING_ISR {}
void ep8ping_isr() interrupt EP8PING_ISR {}
void errlimit_isr() interrupt ERRLIMIT_ISR {}
void ep2isoerr_isr() interrupt EP2ISOERR_ISR {}
void ep4isoerr_isr() interrupt EP4ISOERR_ISR {}
void ep6isoerr_isr() interrupt EP6ISOERR_ISR {}
void ep8isoerr_isr() interrupt EP8ISOERR_ISR {}
void spare_isr() interrupt RESERVED_ISR {}
void ep2pf_isr() interrupt EP2PF_ISR{}
void ep4pf_isr() interrupt EP4PF_ISR{}
void ep6pf_isr() interrupt EP6PF_ISR{}
void ep8pf_isr() interrupt EP8PF_ISR{}
void ep2ef_isr() interrupt EP2EF_ISR{}
void ep4ef_isr() interrupt EP4EF_ISR{}
void ep6ef_isr() interrupt EP6EF_ISR{}
void ep8ef_isr() interrupt EP8EF_ISR{}
void ep2ff_isr() interrupt EP2FF_ISR{}
void ep4ff_isr() interrupt EP4FF_ISR{}
void ep6ff_isr() interrupt EP6FF_ISR{}
void ep8ff_isr() interrupt EP8FF_ISR{}
void gpifdone_isr() interrupt GPIFDONE_ISR{}
void gpifwf_isr() interrupt GPIFWF_ISR{}