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
 * \addtogroup tiva-c-uart
 * @{
 *
 * \file
 * Implementation of the Tiva C UART driver
 */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
 
#include "contiki.h"
#include "sys/energest.h"
#include "dev/tivaware/inc/hw_memmap.h"
#include "dev/tivaware/inc/hw_types.h"
#include "dev/tivaware/inc/hw_ints.h"
#include "dev/tivaware/inc/hw_uart.h"
#include "dev/tivaware/driverlib/tm4c_gpio.h"
#include "dev/tivaware/driverlib/tm4c_interrupt.h"
#include "dev/tivaware/driverlib/tm4c_pin_map.h"
#include "dev/tivaware/driverlib/tm4c_sysctl.h"
#include "dev/tivaware/driverlib/tm4c_uart.h"
#include "dev/uart.h"

#include "dev/leds.h"

extern uint32_t sys_clock;

static int (* input_handler)(unsigned char c);
/*---------------------------------------------------------------------------*/
static void
reset(void)
{
  /* 
   * Make sure the UART is disabled before trying to configure it.  This also
   * flushes the FIFOs.
   */
  UARTDisable(UART0_BASE);

  /* Clear error status */
  UARTRxErrorClear(UART0_BASE);

  /* UART Enable */
  UARTEnable(UART0_BASE);
}
/*---------------------------------------------------------------------------*/
void
uart_init(void)
{
  /* Enable clock for GPIO port A */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  
  /* Enable clock for UART0 */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

  /* Configure the IO mux to map UART0 signals to PA0/PA1 */
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  
  /* Configure the pins as UART */
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1); 

  /*
   * UART Interrupt Masks:
   * Acknowledge RX and RX Timeout
   * Acknowledge Framing, Overrun and Break Errors
   */
  UARTIntEnable(UART0_BASE, (UART_INT_RX | UART_INT_RT | UART_INT_OE | 
                UART_INT_BE | UART_INT_FE));

  /* Set FIFO levels */
  UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX4_8, UART_FIFO_RX1_8);

  /* Make sure the UART is disabled before trying to configure it */
  UARTDisable(UART0_BASE);

  /* Baud Rate Generation */
  UARTConfigSetExpClk(UART0_BASE, sys_clock, UART_CONF_BAUD_RATE,
                      (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE |
                       UART_CONFIG_WLEN_8));

  /* Enable UART0 Interrupts */
  IntEnable(INT_UART0);
  
  /* UART Enable */
  UARTEnable(UART0_BASE);
}
/*---------------------------------------------------------------------------*/
void
uart_set_input(int (* input)(unsigned char c))
{
  input_handler = input;
}
/*----------------------------------------------------------------u-----------*/
void
uart_write_byte(uint8_t b)
{
  if((HWREG(UART0_BASE | UART_O_CTL) & 0x101) != 0x101)
  {
    leds_on(1);
  }
  
  /* Write data.  Block if the TX FIFO is full */
  UARTCharPut(UART0_BASE, b);
}
/*---------------------------------------------------------------------------*/
void
uart_isr(void)
{
  uint16_t mis;

  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  /* Store the current MIS and clear all flags early, except the RTM flag.
   * This will clear itself when we read out the entire FIFO contents */
  mis = (uint16_t)UARTIntStatus(UART0_BASE, true); 
  
  HWREG(UART0_BASE | UART_O_ICR) = 0x0000FFBF;

  if(mis & (UART_INT_RX | UART_INT_RT)) {
    if(input_handler != NULL) {
      input_handler((unsigned char)UARTCharGetNonBlocking(UART0_BASE));
    } else {
      /* To prevent an Overrun Error, we need to flush the FIFO even if we
       * don't have an input_handler. Use mis as a data trash can */
      mis = (uint16_t)UARTCharGetNonBlocking(UART0_BASE);
      }
  } else if(mis & (UART_INT_OE | UART_INT_BE | UART_INT_FE)) {
    /* ISR triggered due to some error condition */
    reset();
  }

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}

/** @} */
