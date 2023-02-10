#ifndef _HAL_H_
#define _HAL_H_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#include "gpio.h"


#define _HAL_GPIO_HELPER_(_gpio_number_) GPIO##_gpio_number_
#define HAL_GPIO(_gpio_number_) _HAL_GPIO_HELPER_(_gpio_number_)


#define HAL_EQEP1I_PIN 23     /* J17 at board corner before ground (pin 13 - second outside pin corner to middle) */
#define HAL_CONTROL_PROCESS_DBG_PIN 23


extern void HAL_GPIO_setup(void);


static inline HAL_ControlProcessDbgPinTurnOn()
{
    GPIO_writePin_2806x(HAL_CONTROL_PROCESS_DBG_PIN, 1);
}


static inline HAL_ControlProcessDbgPinTurnOff()
{
    GPIO_writePin_2806x(HAL_CONTROL_PROCESS_DBG_PIN, 0);
}


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _HAL_H_ */
