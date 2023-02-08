#include "hal.h"


/* *****************************************************************************
 * HAL_GPIO_setup()
 **************************************************************************** */
void HAL_GPIO_setup(void)
{
    EALLOW;

    GpioCtrlRegs.GPAMUX2.bit.HAL_GPIO(HAL_CONTROL_PROCESS_DBG_PIN) = 0;  // to be GPIO
    GpioCtrlRegs.GPADIR.bit.HAL_GPIO(HAL_CONTROL_PROCESS_DBG_PIN) = 1;  // as output
    GpioDataRegs.GPADAT.bit.HAL_GPIO(HAL_CONTROL_PROCESS_DBG_PIN) = 0; // level

    EDIS;

}
