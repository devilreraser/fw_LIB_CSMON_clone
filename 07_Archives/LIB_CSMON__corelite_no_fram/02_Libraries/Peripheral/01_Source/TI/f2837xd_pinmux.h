//*****************************************************************************
//
//  f2837xd_pinmux.h - Created using TI Pinmux 4.0.1543  on 7/1/2020 at 4:15:20 PM.
//
//*****************************************************************************
//
// Copyright (C) 2017 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the
//   distribution.
//
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************
//
//  These values will provide the functionality requested when written into
//  the registers for which the #defines are named.  For example, using the
//  C2000Ware device support header files, use the defines like in this
//  sample function:
//
//  void samplePinMuxFxn(void)
//  {
//      EALLOW;
//      //
//      // Write generated values to mux registers
//      //
//      GpioCtrlRegs.GPAMUX1.all  = GPAMUX1_VALUE;
//      GpioCtrlRegs.GPAMUX2.all  = GPAMUX2_VALUE;
//      GpioCtrlRegs.GPBMUX1.all  = GPBMUX1_VALUE;
//        . . .
//      EDIS;
//  }
//
//  NOTE:  These GPIO control registers are only available on CPU1.
//
//*****************************************************************************

//
// Port A mux register values
//
// Pin D7 (GPIO5) to CANRXA (mode 6)
// Pin C7 (GPIO4) to CANTXA (mode 6)
// Pin F3 (GPIO21) to CANRXB (mode 3)
// Pin F2 (GPIO20) to CANTXB (mode 3)
// Pin U11 (GPIO31) to EM1WEn (mode 2)
// Pin B2 (GPIO10) to EQEP1A (mode 5)
// Pin C1 (GPIO11) to EQEP1B (mode 5)
// Pin C2 (GPIO12) to EQEP1S (mode 5)
// Pin D1 (GPIO13) to EQEP1I (mode 5)
// Pin B6 (GPIO7) to GPIO7 (mode 0)
// Pin K1 (GPIO26) to GPIO26 (mode 0)
// Pin L1 (GPIO27) to GPIO27 (mode 0)
// Pin C8 (GPIO0) to GPIO0 (mode 0)
// Pin D8 (GPIO1) to GPIO1 (mode 0)
// Pin G2 (GPIO8) to GPIO8 (mode 0)
// Pin D3 (GPIO15) to GPIO15 (mode 0)
// Pin A7 (GPIO2) to SDAB (mode 6)
// Pin B7 (GPIO3) to OUTPUTXBAR2 (mode 2)
// Pin D2 (GPIO14) to OUTPUTXBAR3 (mode 6)
// Pin V11 (GPIO28) to OUTPUTXBAR5 (mode 5)
// Pin W11 (GPIO29) to OUTPUTXBAR6 (mode 5)
// Pin T11 (GPIO30) to OUTPUTXBAR7 (mode 5)
// Pin K3 (GPIO24) to SPISIMOB (mode 6)
// Pin K2 (GPIO25) to SPISOMIB (mode 6)
// Pin J4 (GPIO22) to SPICLKB (mode 6)
// Pin K4 (GPIO23) to SPISTEB (mode 6)
// Pin E1 (GPIO16) to SPISIMOA (mode 1)
// Pin E2 (GPIO17) to SPISOMIA (mode 1)
// Pin E3 (GPIO18) to SPICLKA (mode 1)
// Pin E4 (GPIO19) to SPISTEA (mode 1)
#define GPAMUX1_MASK		0xfff3cfff
#define GPAMUX2_MASK		0xffffffff
#define GPAMUX1_VALUE		0x25500aa0
#define GPAMUX2_VALUE		0x950aaf55
#define GPAGMUX1_VALUE		0x15500510
#define GPAGMUX2_VALUE		0x15055000

//
// Port B mux register values
//
// Pin U14 (GPIO34) to EM1CS2n (mode 2)
// Pin U16 (GPIO37) to EM1OEn (mode 2)
// Pin T16 (GPIO38) to EM1A0 (mode 2)
// Pin W17 (GPIO39) to EM1A1 (mode 2)
// Pin V17 (GPIO40) to EM1A2 (mode 2)
// Pin U17 (GPIO41) to EM1A3 (mode 2)
// Pin K18 (GPIO44) to EM1A4 (mode 2)
// Pin K19 (GPIO45) to EM1A5 (mode 2)
// Pin E19 (GPIO46) to EM1A6 (mode 2)
// Pin E18 (GPIO47) to EM1A7 (mode 2)
// Pin R16 (GPIO48) to EM1A8 (mode 2)
// Pin R17 (GPIO49) to EM1A9 (mode 2)
// Pin R18 (GPIO50) to EM1A10 (mode 2)
// Pin R19 (GPIO51) to EM1A11 (mode 2)
// Pin P16 (GPIO52) to EM1A12 (mode 2)
// Pin J16 (GPIO63) to EM2D5 (mode 3)
// Pin J17 (GPIO62) to EM2D6 (mode 3)
// Pin L16 (GPIO61) to EM2D7 (mode 3)
// Pin M17 (GPIO60) to EM2D8 (mode 3)
// Pin M16 (GPIO59) to EM2D9 (mode 3)
// Pin N17 (GPIO58) to EM2D10 (mode 3)
// Pin N18 (GPIO57) to EM2D11 (mode 3)
// Pin N16 (GPIO56) to EM2D12 (mode 3)
// Pin P19 (GPIO55) to EM2D13 (mode 3)
// Pin P18 (GPIO54) to EM2D14 (mode 3)
// Pin P17 (GPIO53) to EM2D15 (mode 3)
// Pin V16 (GPIO36) to GPIO36 (mode 0)
// Pin U13 (GPIO32) to SDAA (mode 1)
// Pin T13 (GPIO33) to SCLA (mode 1)
// Pin T14 (GPIO35) to SCLB (mode 6)
// Pin C19 (GPIO43) to SCIRXDA (mode 15)
// Pin D19 (GPIO42) to SCITXDA (mode 15)
#define GPBMUX1_MASK		0xffffffff
#define GPBMUX2_MASK		0xffffffff
#define GPBMUX1_VALUE		0xaafaa8a5
#define GPBMUX2_VALUE		0xfffffeaa
#define GPBGMUX1_VALUE		0x00f00040
#define GPBGMUX2_VALUE		0x00000000

//
// Port C mux register values
//
// Pin A4 (GPIO92) to EM1BA1 (mode 3)
// Pin C11 (GPIO86) to EM1A13 (mode 2)
// Pin D11 (GPIO87) to EM1A14 (mode 2)
// Pin C6 (GPIO88) to EM1A15 (mode 2)
// Pin D6 (GPIO89) to EM1A16 (mode 2)
// Pin A5 (GPIO90) to EM1A17 (mode 2)
// Pin B5 (GPIO91) to EM1A18 (mode 2)
// Pin B11 (GPIO85) to EM1D0 (mode 2)
// Pin C14 (GPIO83) to EM1D1 (mode 2)
// Pin B14 (GPIO82) to EM1D2 (mode 2)
// Pin A14 (GPIO81) to EM1D3 (mode 2)
// Pin D15 (GPIO80) to EM1D4 (mode 2)
// Pin C15 (GPIO79) to EM1D5 (mode 2)
// Pin B15 (GPIO78) to EM1D6 (mode 2)
// Pin A15 (GPIO77) to EM1D7 (mode 2)
// Pin C16 (GPIO76) to EM1D8 (mode 2)
// Pin D16 (GPIO75) to EM1D9 (mode 2)
// Pin C17 (GPIO74) to EM1D10 (mode 2)
// Pin A16 (GPIO73) to EM1D11 (mode 2)
// Pin B16 (GPIO72) to EM1D12 (mode 2)
// Pin B17 (GPIO71) to EM1D13 (mode 2)
// Pin A17 (GPIO70) to EM1D14 (mode 2)
// Pin B18 (GPIO69) to EM1D15 (mode 2)
// Pin C18 (GPIO68) to EM2D0 (mode 3)
// Pin B19 (GPIO67) to EM2D1 (mode 3)
// Pin K17 (GPIO66) to EM2D2 (mode 3)
// Pin K16 (GPIO65) to EM2D3 (mode 3)
// Pin L17 (GPIO64) to EM2D4 (mode 3)
// Pin B4 (GPIO93) to GPIO93 (mode 0)
// Pin A3 (GPIO94) to GPIO94 (mode 0)
// Pin B3 (GPIO95) to GPIO95 (mode 0)
// Pin A11 (GPIO84) to GPIO84 (mode 0)
#define GPCMUX1_MASK		0xffffffff
#define GPCMUX2_MASK		0xffffffff
#define GPCMUX1_VALUE		0xaaaaabff
#define GPCMUX2_VALUE		0x03aaa8aa
#define GPCGMUX1_VALUE		0x00000000
#define GPCGMUX2_VALUE		0x00000000

//
// Port D mux register values
//
// Pin V12 (GPIO115) to EM2CS0n (mode 3)
// Pin T12 (GPIO118) to EM2CLK (mode 3)
// Pin U15 (GPIO120) to EM2WEn (mode 3)
// Pin N4 (GPIO113) to EM2CAS (mode 3)
// Pin N3 (GPIO114) to EM2RAS (mode 3)
// Pin M4 (GPIO111) to EM2BA0 (mode 3)
// Pin M3 (GPIO112) to EM2BA1 (mode 3)
// Pin F1 (GPIO98) to EM2A0 (mode 3)
// Pin G1 (GPIO99) to EM2A1 (mode 3)
// Pin H1 (GPIO100) to EM2A2 (mode 3)
// Pin H2 (GPIO101) to EM2A3 (mode 3)
// Pin H3 (GPIO102) to EM2A4 (mode 3)
// Pin J1 (GPIO103) to EM2A5 (mode 3)
// Pin J2 (GPIO104) to EM2A6 (mode 3)
// Pin J3 (GPIO105) to EM2A7 (mode 3)
// Pin L2 (GPIO106) to EM2A8 (mode 3)
// Pin L3 (GPIO107) to EM2A9 (mode 3)
// Pin L4 (GPIO108) to EM2A10 (mode 3)
// Pin N2 (GPIO109) to EM2A11 (mode 3)
// Pin A2 (GPIO97) to GPIO97 (mode 0)
// Pin C3 (GPIO96) to GPIO96 (mode 0)
// Pin T8 (GPIO122) to SPISIMOC (mode 6)
// Pin U8 (GPIO123) to SPISOMIC (mode 6)
// Pin V8 (GPIO124) to SPICLKC (mode 6)
// Pin T9 (GPIO125) to SPISTEC (mode 6)
#define GPDMUX1_MASK		0xcfffffff
#define GPDMUX2_MASK		0x0ff330ff
#define GPDMUX1_VALUE		0xcffffff0
#define GPDMUX2_VALUE		0x0aa330ff
#define GPDGMUX1_VALUE		0x00000000
#define GPDGMUX2_VALUE		0x05500000

//
// Port E mux register values
//
// Pin E17 (GPIO145) to EPWM1A (mode 1)
// Pin D18 (GPIO146) to EPWM1B (mode 1)
// Pin D17 (GPIO147) to EPWM2A (mode 1)
// Pin D14 (GPIO148) to EPWM2B (mode 1)
// Pin A13 (GPIO149) to EPWM3A (mode 1)
// Pin B13 (GPIO150) to EPWM3B (mode 1)
// Pin C13 (GPIO151) to EPWM4A (mode 1)
// Pin D13 (GPIO152) to EPWM4B (mode 1)
// Pin A12 (GPIO153) to EPWM5A (mode 1)
// Pin B12 (GPIO154) to EPWM5B (mode 1)
// Pin C12 (GPIO155) to EPWM6A (mode 1)
// Pin D12 (GPIO156) to EPWM6B (mode 1)
// Pin B10 (GPIO157) to EPWM7A (mode 1)
// Pin C10 (GPIO158) to EPWM7B (mode 1)
// Pin D10 (GPIO159) to EPWM8A (mode 1)
// Pin V10 (GPIO131) to GPIO131 (mode 0)
// Pin W18 (GPIO132) to GPIO132 (mode 0)
// Pin G18 (GPIO133) to GPIO133 (mode 0)
// Pin V18 (GPIO134) to GPIO134 (mode 0)
// Pin F18 (GPIO143) to GPIO143 (mode 0)
// Pin F17 (GPIO144) to GPIO144 (mode 0)
// Pin L19 (GPIO142) to GPIO142 (mode 0)
// Pin M18 (GPIO141) to GPIO141 (mode 0)
// Pin T19 (GPIO138) to SCIRXDB (mode 6)
// Pin T18 (GPIO137) to SCITXDB (mode 6)
// Pin N19 (GPIO139) to SCIRXDC (mode 6)
// Pin M19 (GPIO140) to SCITXDC (mode 6)
#define GPEMUX1_MASK		0xfffc3fc0
#define GPEMUX2_MASK		0xffffffff
#define GPEMUX1_VALUE		0x02a80000
#define GPEMUX2_VALUE		0x55555554
#define GPEGMUX1_VALUE		0x01540000
#define GPEGMUX2_VALUE		0x00000000

//
// Port F mux register values
//
// Pin B9 (GPIO160) to EPWM8B (mode 1)
// Pin C9 (GPIO161) to EPWM9A (mode 1)
// Pin D9 (GPIO162) to EPWM9B (mode 1)
// Pin A8 (GPIO163) to EPWM10A (mode 1)
// Pin B8 (GPIO164) to EPWM10B (mode 1)
// Pin C5 (GPIO165) to EPWM11A (mode 1)
// Pin D5 (GPIO166) to EPWM11B (mode 1)
// Pin C4 (GPIO167) to EPWM12A (mode 1)
// Pin D4 (GPIO168) to EPWM12B (mode 1)
#define GPFMUX1_MASK		0x0003ffff
#define GPFMUX1_VALUE		0x00015555
#define GPFGMUX1_VALUE		0x00000000

//
// Port B analog mode register values
//
// Pin C19 (GPIO43) to SCIRXDA (mode 15)
// Pin D19 (GPIO42) to SCITXDA (mode 15)
#define GPBAMSEL_MASK		0x00000c00
#define GPBAMSEL_VALUE		0x00000000

//
// Input X-BAR register values
//
// Pin U10 (GPIO130) to INPUTXBAR1 (mode XBAR1)
// Pin T10 (GPIO129) to INPUTXBAR2 (mode XBAR2)
// Pin W9 (GPIO128) to INPUTXBAR3 (mode XBAR3)
// Pin V9 (GPIO127) to INPUTXBAR4 (mode XBAR4)
// Pin U9 (GPIO126) to INPUTXBAR5 (mode XBAR5)
// Pin T15 (GPIO119) to INPUTXBAR6 (mode XBAR6)
// Pin U12 (GPIO117) to INPUTXBAR8 (mode XBAR8)
// Pin M2 (GPIO110) to INPUTXBAR12 (mode XBAR12)
#define INPUT1SELECT_VALUE	0x00000082
#define INPUT2SELECT_VALUE	0x00000081
#define INPUT3SELECT_VALUE	0x00000080
#define INPUT4SELECT_VALUE	0x0000007f
#define INPUT5SELECT_VALUE	0x0000007e
#define INPUT6SELECT_VALUE	0x00000077
#define INPUT8SELECT_VALUE	0x00000075
#define INPUT12SELECT_VALUE	0x0000006e

//*****************************************************************************
//
// Function prototype for function to write values above into their
// corresponding registers. This function is found in f2837xd_pinmux.c. Its use
// is completely optional.
//
//*****************************************************************************
extern void GPIO_setPinMuxConfig(void);
