/*
THIS WILL ACT AS SLAVE FIFO READ MODE. THE EXTERNAL MASTER (ANOTHER FX2LP IN GPIF MODE) WILL POINT TO ENDPOINT2(EP2-OUT) AND READ THE BUFFER WHEN 512 1024 BYTES IS FILLED.
 */


#include <fx2regs.h>
#include <fx2macros.h>
#include <autovector.h>
#include <delay.h>
#include <fx2sdly.h>
#include <eputils.h>
#include <setupdat.h>




BOOL handle_get_descriptor(){
  return FALSE;
}

BOOL handle_get_interface(BYTE ifc, BYTE *alt_ifc){
  if (ifc == 0){
      *alt_ifc = 0;
      return TRUE;
  }else{
    return FALSE;
  }
}

BOOL handle_set_interface(BYTE ifc, BYTE alt_ifc){
  if (ifc == 0 && alt_ifc == 0){
    RESETTOGGLE(0x02);
    RESETTOGGLE(0x86);

    RESETFIFO(0x02);
    EP2CBCL = 0x80;
    SYNCDELAY;
    EP2BCL=0x80;
    SYNCDELAY;
    RESETFIFO(0x86);
    return TRUE;
  }else{
    return FALSE;
  }
}

BYTE handle_get_configuration(){
  return TRUE;
}

BOOLD handle_set_configuration(BYTE cfg){
  return cfg == 1 ? TRUE : FALSE;
}

#define VC_EPSTAT 0x81

BOOL handle_vendorcommand(BYTE cmd){
  switch(cmd){
  case VC_EPSTAT:
    __xdata BYTE *pep = ep_addr(SETUPDAT[2]);
    if(pep){
      EP0BUF[0] = *pep;
      EP0BCH = 0;
      EP0BCL = 1;
      return TRUE;
    }
  default:
    printf("Need to implement vendor command: %02x\n", cmd);
  }{
    return FALSE;
  }
}

const static void initialize(void){
  // CPU Control and Status Register
  CPUCS = (1 << bmCLKSPD1); //48MHz CPU clock, CLKOUT disabled
  SYNCDELAY;
  // Interface Configuration (Ports, GPIF, Slave FIFOSs) Register
  IFCONFIG = (1 << bmIFCLKPOL) | (1 << bmIFCFG1) | (1 << bmIFCFG0) ; //external IFCLK @ 48MHz for FIFO, SLAVE FIFO MODE, SYNCHRONOUS
  SYNCDELAY;
  // Chip Revision Control Register
  REVCTL = (1 << bmNOAUTOARM) | (1 << bmSKIPCOMMIT); // Disabled auto-arm + Enhanced packet handling
  SYNCDELAY;
  FIFORESET = 0x80; // NAK all requests from host
  SYNCDELAY;
  FIFORESET = 0x82; // Reset EP 2
  SYNCDELAY;
  //  FIFORESET = 0x84; //Reset EP 4..
  //  SYNCDELAY;
  //  FIFORESET = 0x86;
  //  SYNCDELAY;
  //  FIFORESET = 0x88;
  //  SYNCDELAY;
  FIFORESET = 0x00;
  SYNCDELAY;
  OUTPKTEND = 0x82;
  SYNCDELAY;
  OUTPKTEND = 0x82; 
  // CONFIGURE THE ENDPOINTS
  SYNCDELAY;
  EP2FIFOCFG = (1 << bmAUTOOUT); // AUTO DISPATCHED THE DATA FROM EP2 to the FIFO BUS, 8-bits wide
  SYNCDELAY;
  EP6CFG = (1 << bmVALID) | (1 << bmDIR) | (1 << bmTYPE1) | (1 << bmBUF1); // bulk IN, 512 bytes, double-buffered
  SYNCDELAY;
  EP2CFG = (1 << bmVALID) | (1 << bmTYPE1) | (1 << bmBUF1); // bulk OUT, 512 bytes, double-buffered // END CONFIGURATION
  SYNCDELAY;
  EP4CFG = 0x00; // Disable unused as suggested by TRM
  SYNCDELAY;
  EP8CFG = 0x00;

  // CONFIGURE FLAGS
  EP2FIFOFLGS = (1 << bmBIT1); // EMPTY FLAG
  EP2FIFOFLGS = (1 << bmBIT0); // FULL FLAG


  EP2FIFOCFG = (1 << bmAUTOOUT); // AUTO DISPATCHED THE DATA FROM EP2 to the FIFO BUS, 8-bits wide
  SYNCDELAY;
  OUTPKTEND = (1 << bmBIT1); // Auto-dispatch the OUT packet in EP2
  SYNCDELAY;
  REVCTL = 0x00; // Enable auto-arm + Disable Enhanced packet handling

  // OUT endpoints do NOT come up armed
  SYNCDELAY;
  OUTPKTEND = 0x82;
  SYNCDELAY;
  OUTPKTEND = 0x82;
}

void main_init(){

  initialize();

}

void main_loop(){
}
