#include "dbg_ctrl.h"
#include "hal.h"


void DBG_CTRL_ControlProcessOnEnter(void)
{
    HAL_ControlProcessDbgPinTurnOn();
}


void DBG_CTRL_ControlProcessOnExit(void)
{
    HAL_ControlProcessDbgPinTurnOn();
}
