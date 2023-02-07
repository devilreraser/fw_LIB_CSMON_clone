#include "hal.h"


/* *****************************************************************************
 * HAL_GPIO_setup()
 **************************************************************************** */
void HAL_GPIO_setup(void)
{
    EALLOW;
    //  LED2 - red led (the middle led between LED3 and the power Supply green Led)
    GpioCtrlRegs.GPAPUD.bit.GPIO31 = 0;   // Enable pullup on GPIO31
    GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;  // GPIO31 = GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;  // GPIO31 = output
    GpioDataRegs.GPADAT.bit.GPIO31 = 0; // level

    //  LED3 - red led nearest to the micro USB
    GpioCtrlRegs.GPBPUD.bit.GPIO34 = 0;  // Enable pullup on GPIO34
    GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0; // GPIO34 = GPIO34
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;  // GPIO34 = output
    GpioDataRegs.GPBDAT.bit.GPIO34 = 0; // level
    EDIS;
}
