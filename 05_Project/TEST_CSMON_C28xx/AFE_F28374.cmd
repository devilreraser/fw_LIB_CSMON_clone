#ifdef CLA_BLOCK_INCLUDED
// Define a size for the CLA scratchpad area that will be used
// by the CLA compiler for local symbols and temps
// Also force references to the special symbols that mark the
// scratchpad are. 
CLA_SCRATCHPAD_SIZE = 0x100;
--undef_sym=__cla_scratchpad_end
--undef_sym=__cla_scratchpad_start
#endif //CLA_BLOCK_INCLUDED
MEMORY
{
PAGE 0 :
   /* BEGIN is used for the "boot to SARAM" bootloader mode   */
   BEGIN           	: origin = 0x000000, length = 0x000002
   BEGIN_FLASH     	: origin = 0x080000, length = 0x000002
   RAMM0           	: origin = 0x000122, length = 0x0002DE
   RAMLS_PROG      	: origin = 0x008000, length = 0x000800

   RESET           	: origin = 0x3FFFC0, length = 0x000002
   /* Flash sectors */
   FLASHA_N           : origin = 0x080002, length = 0x01FFFE	/* on-chip Flash */ 

PAGE 1 :
   BOOT_RSVD       : origin = 0x000002, length = 0x000120     /* Part of M0, BOOT rom will use this for stack */
   RAMM1           : origin = 0x000400, length = 0x000400     /* on-chip RAM block M1 */
   RAMLSGS_DATA          : origin = 0x008800, length = 0x00F800

   //RAMGS_IPCBuffCPU1   : origin = 0x01A000, length = 0x001000
   //RAMGS_IPCBuffCPU2   : origin = 0x01B000, length = 0x001000 

   CLA1_MSGRAMLOW   : origin = 0x001480, length = 0x000080
   CLA1_MSGRAMHIGH  : origin = 0x001500, length = 0x000080

   CPU2TOCPU1RAM   : origin = 0x03F800, length = 0x000400
   CPU1TOCPU2RAM   : origin = 0x03FC00, length = 0x000400

   #ifdef CPU1
   //EXTERN_RAM  : origin = 0x00140000, length = 0x000A0000
   //EVENT_RAM   : origin = 0x001E0000, length = 0x00010000
   //PARA_RAM    : origin = 0x001F0000, length = 0x00010000
   #endif
}


SECTIONS
{
   /* Allocate program areas: */
   .cinit              : > FLASHA_N      PAGE = 0, ALIGN(4)
   .pinit              : > FLASHA_N,     PAGE = 0, ALIGN(4)
   .text               : > FLASHA_N      PAGE = 0, ALIGN(4)
   codestart           : > BEGIN_FLASH       PAGE = 0, ALIGN(4)
   ramfuncs            : LOAD = FLASHA_N,
                         RUN = RAMLS_PROG,
                         LOAD_START(_RamfuncsLoadStart),
                         LOAD_SIZE(_RamfuncsLoadSize),
                         LOAD_END(_RamfuncsLoadEnd),
                         RUN_START(_RamfuncsRunStart),
                         RUN_SIZE(_RamfuncsRunSize),
                         RUN_END(_RamfuncsRunEnd),
                         PAGE = 0, ALIGN(4)

   .TI.ramfunc         : LOAD = FLASHA_N,
                         RUN = RAMLS_PROG,
                         LOAD_START(_TI_ramfuncLoadStart),
                         LOAD_SIZE(_TI_ramfuncLoadSize),
                         LOAD_END(_TI_ramfuncLoadEnd),
                         RUN_START(_TI_ramfuncRunStart),
                         RUN_SIZE(_TI_ramfuncRunSize),
                         RUN_END(_TI_ramfuncRunEnd),
                         PAGE = 0, ALIGN(4)

   /* Initalized sections go in Flash */
   .econst             : > FLASHA_N      PAGE = 0, ALIGN(4)
   .switch             : > FLASHA_N      PAGE = 0, ALIGN(4)
   /* Allocate IQmath areas: */
   IQmath           	  : LOAD = FLASHA_N, RUN = RAMLS_PROG, START(_IQmathLoadStart), END(_IQmathLoadEnd), RUN_START(_IQmathRunStart), PAGE = 0
   /*IQmath			: > FLASHA_N, PAGE = 0, ALIGN(4)*/            /* Math Code */
   IQmathTables		  : > FLASHA_N, PAGE = 0, ALIGN(4)
   
   #ifdef CLA_BLOCK_INCLUDED
   /* CLA specific sections */
   Cla1Prog            : LOAD = FLASHA_N,
                         RUN = RAMLS_CLA_PROG,
                         LOAD_START(_Cla1funcsLoadStart),
                         LOAD_END(_Cla1funcsLoadEnd),
                         RUN_START(_Cla1funcsRunStart),
                         LOAD_SIZE(_Cla1funcsLoadSize),
                         PAGE = 0, ALIGN(4)
   #endif //CLA_BLOCK_INCLUDED

   .stack           : > RAMM1,     PAGE = 1
   .ebss            : > RAMLSGS_DATA,    PAGE = 1
   .reset           : > RESET,     PAGE = 0, TYPE = DSECT /* not used, */
   .esysmem         : > RAMLSGS_DATA,    PAGE = 1

   #ifdef CPU1
   .extram			   : > RAMLSGS_DATA,		PAGE = 1
   .tsm				   : > RAMLSGS_DATA,		PAGE = 1
   .pararam 		   : > RAMLSGS_DATA,		   PAGE = 1
   .eventram 		   : > RAMLSGS_DATA,		   PAGE = 1
   #endif
   
   #ifdef CLA_BLOCK_INCLUDED
       /* CLA C compiler sections */
       //
       // Must be allocated to memory the CLA has write access to
       //
       Cla1DataRam0		: > RAMLS_CLA_DATA, PAGE=1

       Cla1ToCpuMsgRAM  : > CLA1_MSGRAMLOW,   PAGE = 1
       CpuToCla1MsgRAM  : > CLA1_MSGRAMHIGH,  PAGE = 1
       CLAscratch       :
                         { *.obj(CLAscratch)
                         . += CLA_SCRATCHPAD_SIZE;
                         *.obj(CLAscratch_end) } >  RAMLS_CLA_DATA,  PAGE = 1

       .scratchpad      : > RAMLS_CLA_DATA,       PAGE = 1
       .bss_cla		    : > RAMLS_CLA_DATA,       PAGE = 1
       .const_cla	    :  LOAD = FLASHA_N,
                           RUN = RAMLS_CLA_DATA,
                           RUN_START(_Cla1ConstRunStart),
                           LOAD_START(_Cla1ConstLoadStart),
                           LOAD_SIZE(_Cla1ConstLoadSize),
                           PAGE = 1
   #endif //CLA_BLOCK_INCLUDED
   #ifdef CPU1  
       /* The following section definitions are required when using the IPC API Drivers */ 
        GROUP : > CPU1TOCPU2RAM, PAGE = 1 
        {
            CPU1TOCPU2GSRAM
            PUTBUFFER 
            PUTWRITEIDX 
            GETREADIDX 
            WRITEFLAG1CPU1
	         WRITEFLAG2CPU1
            READFLAG1CPU1
            READFLAG2CPU1
        }
        GROUP : > CPU2TOCPU1RAM, PAGE = 1
        {
            CPU2TOCPU1GSRAM
            GETBUFFER :    TYPE = DSECT
            GETWRITEIDX :  TYPE = DSECT
            PUTREADIDX :   TYPE = DSECT
            WRITEFLAG1CPU2: TYPE = DSECT
            WRITEFLAG2CPU2: TYPE = DSECT
            READFLAG1CPU2:  TYPE = DSECT
            READFLAG2CPU2:  TYPE = DSECT
        }

   #else
       /* The following section definitions are required when using the IPC API Drivers */ 
        GROUP : > CPU2TOCPU1RAM, PAGE = 1 
        {
            CPU2TOCPU1GSRAM
            PUTBUFFER 
            PUTWRITEIDX 
            GETREADIDX 
            WRITEFLAG1CPU2
            WRITEFLAG2CPU2
            READFLAG1CPU2			
            READFLAG2CPU2			
        }
        GROUP : > CPU1TOCPU2RAM, PAGE = 1
        {
            CPU1TOCPU2GSRAM
            GETBUFFER :    TYPE = DSECT
            GETWRITEIDX :  TYPE = DSECT
            PUTREADIDX :   TYPE = DSECT
            WRITEFLAG1CPU1: TYPE = DSECT
            WRITEFLAG2CPU1: TYPE = DSECT
            READFLAG1CPU1 : TYPE = DSECT
            READFLAG2CPU1 : TYPE = DSECT
        }
   #endif 
   #if 0
        GROUP : > RAMGS_IPCBuffCPU1, PAGE = 1 
        {
            CPU1TOCPU2GSRAM
        }
        GROUP : > RAMGS_IPCBuffCPU2, PAGE = 1 
        {
            CPU2TOCPU1GSRAM
        }
   #endif
}

/*
//===========================================================================
// End of file.
//===========================================================================
*/
