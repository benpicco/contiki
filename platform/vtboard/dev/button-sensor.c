/*
 * Copyright (c) 2012, Texas Instruments Incorporated - http://www.ti.com/
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * \addtogroup ek-ek-tm4c123gxl-button-sensor
 * @{
 *
 * \file
 *  Driver for the EK-TM4C123GXL buttons
 */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "contiki.h"

#include "dev/gpio.h"
#include "dev/button-sensor.h"
#include "sys/timer.h"

#include "tivaware/inc/hw_memmap.h"
#include "tivaware/inc/hw_types.h"
#include "tivaware/inc/hw_ints.h"
#include "tivaware/inc/hw_gpio.h"
#include "tivaware/driverlib/tm4c_gpio.h"
#include "tivaware/driverlib/tm4c_interrupt.h"
#include "tivaware/driverlib/tm4c_sysctl.h"

static struct timer debouncetimer;
/*---------------------------------------------------------------------------*/
/**
 * \brief Common initialiser for all buttons
 * \param port_base GPIO port's register offset
 * \param pin_mask Pin mask corresponding to the button's pin
 */
static void
config(uint32_t port_base, uint32_t pin_mask)
{
  /* If the pin is shared with NMI, it must be unlocked */
  if((port_base == GPIO_PORTF_BASE) && (pin_mask == GPIO_PIN_0))
  {
    HWREG(GPIO_PORTF_BASE | GPIO_O_LOCK) = 0x4C4F434B;
    HWREG(GPIO_PORTF_BASE | GPIO_O_CR) = 0x1;
  }
  
  /* Configure as input */
  GPIOPinTypeGPIOInput(port_base, pin_mask);
  
  /* Enable weak pull-up */
  GPIOPadConfigSet(port_base, pin_mask, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
  
  /* Set as falling edge detect */
  GPIOIntTypeSet(port_base, pin_mask, GPIO_FALLING_EDGE);
  
  /* Enable interrupt in GPIO peripheral */
  GPIOIntEnable(port_base, pin_mask);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Callback registered with the GPIO module. Gets fired with a button
 * port/pin generates an interrupt
 * \param port The port number that generated the interrupt
 * \param pin The pin number that generated the interrupt. This is the pin
 * absolute number (i.e. 0, 1, ..., 7), not a mask
 */
static void
btn_callback(uint8_t port, uint8_t pin)
{
  if(!timer_expired(&debouncetimer)) {
    return;
  }

  timer_set(&debouncetimer, CLOCK_SECOND / 8);
  
  if((port == GPIO_F_NUM) && (pin == BUTTON_SW1_PIN_NO)) {
      printf("SW1\n");
      sensors_changed(&button_sw1_sensor);
  }
  else if((port == GPIO_F_NUM) && (pin == BUTTON_SW2_PIN_NO)) {
      printf("SW2\n");
      sensors_changed(&button_sw2_sensor);
  }
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Init function for the SW1 button.
 *
 * Parameters are ignored. They have been included because the prototype is
 * dictated by the core sensor api. The return value is also not required by
 * the API but otherwise ignored.
 *
 * \param type ignored
 * \param value ignored
 * \return ignored
 */
static int
config_sw1(int type, int value)
{
  /* Enable clock to peripheral */
  SysCtlPeripheralEnable(BUTTON_SW1_SYSCTL_PORT);
  
  /* Configure pin */
  config(BUTTON_SW1_PORT, BUTTON_SW1_PIN);
  
  /* Enable interrupt in NVIC */
  IntEnable(BUTTON_SW1_INT);

  /* Register callback */
  gpio_register_callback(btn_callback, BUTTON_SW1_PORT_NO, BUTTON_SW1_PIN_NO);
  
  return 1;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Init function for the SW2 button.
 *
 * Parameters are ignored. They have been included because the prototype is
 * dictated by the core sensor api. The return value is also not required by
 * the API but otherwise ignored.
 *
 * \param type ignored
 * \param value ignored
 * \return ignored
 */ 
static int
config_sw2(int type, int value)
{  
  /* Enable clock to peripheral */
  SysCtlPeripheralEnable(BUTTON_SW2_SYSCTL_PORT);
  
  /* Configure pin */
  config(BUTTON_SW2_PORT, BUTTON_SW2_PIN);
  
  /* Enable interrupt in NVIC */
  IntEnable(BUTTON_SW2_INT);

  /* Register callback */
  gpio_register_callback(btn_callback, BUTTON_SW2_PORT_NO, BUTTON_SW2_PIN_NO);
  
  return 1;
}

/*---------------------------------------------------------------------------*/
void
button_sensor_init()
{
  timer_set(&debouncetimer, 0);
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(button_sw1_sensor, BUTTON_SENSOR, NULL, config_sw1, NULL);
SENSORS_SENSOR(button_sw2_sensor, BUTTON_SENSOR, NULL, config_sw2, NULL);

/** @} */
