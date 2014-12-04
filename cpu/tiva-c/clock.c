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
 * \addtogroup tiva-c
 * @{
 *
 * \defgroup tiva-c-clock Tiva C Clock
 *
 * Implementation of the clock module for Tiva C series
 *
 * To implement the clock functionality, we use the SysTick peripheral on the
 * cortex-M4f. We run the system clock at 120 or 80 MHz and we set the SysTick 
 * to give us 128 interrupts / sec
 * @{
 *
 * \file
 * Clock driver implementation for TI Tiva C series
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "contiki.h"
#include "cpu.h"
#include "contiki-conf.h"
#include "dev/tivaware/inc/hw_memmap.h"
#include "dev/tivaware/inc/hw_types.h"
#include "dev/tivaware/driverlib/tm4c_sysctl.h"
#include "dev/tivaware/driverlib/tm4c_systick.h"
#include "dev/tivaware/driverlib/tm4c_timer.h"

#include "sys/energest.h"

#include <stdint.h>
/*---------------------------------------------------------------------------*/
#if defined(TARGET_IS_BLIZZARD_RA1) || defined(TARGET_IS_BLIZZARD_RA3) || \
    defined(TARGET_IS_BLIZZARD_RB0) || defined(TARGET_IS_BLIZZARD_RB1)
#define RELOAD_VALUE ((80000000 / 128) - 1)  /** Fire 128 times / sec */    
#else
#define RELOAD_VALUE ((120000000 / 128) - 1)  /** Fire 128 times / sec */
#endif

static volatile clock_time_t count;
static volatile unsigned long secs = 0;
static volatile uint8_t second_countdown = CLOCK_SECOND;

/*---------------------------------------------------------------------------*/
/**
 * \brief Arch-specific implementation of clock_init for Tiva C
 *
 * We initialise the SysTick to fire 128 interrupts per second, giving us a
 * value of 128 for CLOCK_SECOND
 *
 * We also initialise GPT0:Timer A, which is used by clock_delay_usec().
 * We use 16-bit range (individual), count-down, one-shot, no interrupts.
 * The pre-scaled value yields a convenient 1us tick.
 */
void
clock_init(void)
{
  count = 0;

  /* Set SysTick period. */
  SysTickPeriodSet(RELOAD_VALUE);

  /* Enable the SysTick Interrupt */
  SysTickIntEnable();
  
  /* System tick enable */
  SysTickEnable();

  /*
   * Remove the clock gate to enable GPT0 and then initialise it
   * GPTO sub-timer A used for usec_delay.  
   */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  
  /* 
   * Configure Timer 0, sub-timer A as 16-bit one-shot down counter, 
   * no interrupts.
   */
  TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_ONE_SHOT);

#if defined(TARGET_IS_BLIZZARD_RA1) || defined(TARGET_IS_BLIZZARD_RA3) || \
    defined(TARGET_IS_BLIZZARD_RB0) || defined(TARGET_IS_BLIZZARD_RB1)
  /* Prescale by 80 (thus, value 79 in TAPR) */  
  TimerPrescaleSet(TIMER0_BASE, TIMER_A, 0x4f);
#else
  /* Select the timer clock source as the PIOSC */
  TimerClockSourceSet(TIMER0_BASE, TIMER_CLOCK_PIOSC);

  /* Prescale by 16 (thus, value 15 in TAPR) */
  TimerPrescaleSet(TIMER0_BASE, TIMER_A, 0x0f);
#endif
}
/*---------------------------------------------------------------------------*/
CCIF clock_time_t
clock_time(void)
{
  return count;
}
/*---------------------------------------------------------------------------*/
void
clock_set_seconds(unsigned long sec)
{
  secs = sec;
}
/*---------------------------------------------------------------------------*/
CCIF unsigned long
clock_seconds(void)
{
  return secs;
}
/*---------------------------------------------------------------------------*/
void
clock_wait(clock_time_t i)
{
  clock_time_t start;

  start = clock_time();
  while(clock_time() - start < (clock_time_t)i);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Arch-specific implementation of clock_delay_usec for Tiva C
 * \param len Delay \e len uSecs
 *
 * See clock_init() for GPT0 Timer A's configuration
 */
void
clock_delay_usec(uint16_t len)
{
  /* Clear the interrupt before polling the raw interrupt bit. */
  TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  
  /* Configure the timer */
  TimerLoadSet(TIMER0_BASE, TIMER_A, len);
  TimerEnable(TIMER0_BASE, TIMER_A);

  /* Poll raw interrupt status bit for completion. */
  while(TimerIntStatus(TIMER0_BASE, false) & TIMER_TIMA_TIMEOUT);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Obsolete delay function but we implement it here since some code
 * still uses it
 */
void
clock_delay(unsigned int i)
{
  clock_delay_usec(i);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief The clock Interrupt Service Routine. It polls the etimer process
 * if an etimer has expired. It also updates the software clock tick and
 * seconds counter since reset.
 */
void
clock_isr(void)
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ); 
  count++;
  if(etimer_pending()) {
    etimer_request_poll();
  }

  if(--second_countdown == 0) {
    secs++;
    second_countdown = CLOCK_SECOND;
  }
  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*---------------------------------------------------------------------------*/

/**
 * @}
 * @}
 */
