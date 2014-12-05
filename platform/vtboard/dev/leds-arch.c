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
 * \addtogroup tiva-c-ek-tm4c123xl
 * @{
 *
 * \defgroup ek-tm4c123xl-leds EK-TM4C123XL LED driver
 *
 * LED driver implementation for the TI Tiva C EK-TM4C123XL LaunchPad
 * @{
 *
 * \file
 * LED driver implementation for the TI Tiva C EK-TM4C123XL LaunchPad
 */
#include <stdbool.h>
#include <stdint.h>

#include "contiki.h"

#include "dev/leds.h"

#include "dev/board.h"
#include "tivaware/inc/hw_gpio.h"
#include "tivaware/inc/hw_memmap.h"
#include "tivaware/inc/hw_types.h"
#include "tivaware/driverlib/tm4c_gpio.h"
#include "tivaware/driverlib/tm4c_pin_map.h"
#include "tivaware/driverlib/tm4c_sysctl.h"

/* LED mask only allows for D3 and D4 to be used as general purpose LEDs */
#define LED_GPIO_DATA_MASK  (GPIO_PIN_0 | GPIO_PIN_1)
/*---------------------------------------------------------------------------*/
void
leds_arch_init(void)
{ 
  /* Turn on GPIO clock for peripheral controlling RGB LED and D1 */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  
  /* Configure pins as general purpose outputs */
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
  
  /* Turn LEDs off */
  leds_off(LEDS_D1_RED | LEDS_D1_GREEN | LEDS_D1_BLUE);
}
/*---------------------------------------------------------------------------*/
unsigned char
leds_arch_get(void)
{
  uint32_t temp = 0;
    
  /* Read D1 red */
  temp |= GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1) >> 1;
  
  /* Read D1 green */
  temp |= GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3) >> 2;
  
  /* Read D1 blue */
  temp |= GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2);
  
  return temp;
}
/*---------------------------------------------------------------------------*/
void
leds_arch_set(unsigned char leds)
{  
  /* Write D1 red */
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 
              (leds & LEDS_D1_RED) ? GPIO_PIN_1 : 0);  
              
  /* Write D1 green */
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 
              (leds & LEDS_D1_GREEN) ? GPIO_PIN_3 : 0);   
              
  /* Write D1 blue */
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 
              (leds & LEDS_D1_BLUE) ? GPIO_PIN_2 : 0);              
}
/*---------------------------------------------------------------------------*/

/**
 * @}
 * @}
 */
