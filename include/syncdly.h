#ifndef _IFREQ 
#define _IFREQ 48000   // IFCLK frequency in kHz
#endif

// CFREQ can be any one of: 48000, 24000, or 12000
#ifndef _CFREQ
#define _CFREQ 48000   // CLKOUT frequency in kHz
#endif

#if( _IFREQ < 5000 )
#error "_IFREQ too small!  Valid Range: 5000 to 48000..."
#endif

#if( _IFREQ > 48000 )
#error "_IFREQ too large!  Valid Range: 5000 to 48000..."
#endif

#if( _CFREQ != 48000 )
#if( _CFREQ != 24000 )
#if( _CFREQ != 12000 )
#error "_CFREQ invalid!  Valid values: 48000, 24000, 12000..."
#endif
#endif
#endif

// Synchronization Delay formula: see TRM section 15-14
#define _SCYCL ( 3*(_CFREQ) + 5*(_IFREQ) - 1 ) / ( 2*(_IFREQ) )

#define Nop __asm\
			nop\
			__endasm

#if( _SCYCL == 1 )
#define SYNCDELAY Nop
#endif

#if( _SCYCL == 2 )
#define SYNCDELAY Nop; \
                  Nop
#endif

#if( _SCYCL == 3 )
#define SYNCDELAY Nop; \
                  Nop; \
                  Nop
#endif

#if( _SCYCL == 4 )
#define SYNCDELAY Nop; \
                  Nop; \
                  Nop; \
                  Nop
#endif

#if( _SCYCL == 5 )
#define SYNCDELAY Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop
#endif

#if( _SCYCL == 6 )
#define SYNCDELAY Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop
#endif

#if( _SCYCL == 7 )
#define SYNCDELAY Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop
#endif

#if( _SCYCL == 8 )
#define SYNCDELAY Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop
#endif

#if( _SCYCL == 9 )
#define SYNCDELAY Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop
#endif

#if( _SCYCL == 10 )
#define SYNCDELAY Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop
#endif

#if( _SCYCL == 11 )
#define SYNCDELAY Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop
#endif

#if( _SCYCL == 12 )
#define SYNCDELAY Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop
#endif

#if( _SCYCL == 13 )
#define SYNCDELAY Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop
#endif

#if( _SCYCL == 14 )
#define SYNCDELAY Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop
#endif

#if( _SCYCL == 15 )
#define SYNCDELAY Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop
#endif

#if( _SCYCL == 16 )
#define SYNCDELAY Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop; \
                  Nop
#endif
