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
 * \addtogroup tiva-c-rtimer
 * @{
 *
 * \file
 * Implementation of the arch-specific rtimer functions for Tiva C
 *
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "contiki.h"

#include "sys/energest.h"
#include "sys/rtimer.h"

#include "dev/tivaware/inc/hw_memmap.h"
#include "dev/tivaware/inc/hw_types.h"
#include "dev/tivaware/inc/hw_ints.h"
#include "dev/tivaware/driverlib/tm4c_hibernate.h"
#include "dev/tivaware/driverlib/tm4c_interrupt.h"
#include "dev/tivaware/driverlib/tm4c_sysctl.h"

#include "cpu.h"

#include <stdint.h>
/*---------------------------------------------------------------------------*/
static volatile rtimer_clock_t next_trigger;
extern uint32_t sys_clock;
/*---------------------------------------------------------------------------*/

/**
 * \brief Configure and run the RTC.
 */
void
rtimer_arch_init(void)
{
  /* Enable the clock to the hibernation module */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE);
  
  /* Configure the clock for the hibernation module */
  HibernateEnableExpClk(sys_clock);
  
  /* Set up the peripheral */
  HibernateClockConfig(HIBERNATE_OSC_LOWDRIVE);
  
  /* Enable the interrupt at the NVIC level */
  IntEnable(INT_HIBERNATE);
  
  /* Turn on the RTC */
  HibernateRTCEnable();
  
  return;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Schedules an rtimer task to be triggered at time t
 * \param t The time when the task will need executed. This is an absolute
 *          time, in other words the task will be executed AT time \e t,
 *          not IN \e t ticks
 */
void
rtimer_arch_schedule(rtimer_clock_t t)
{
  rtimer_clock_t now;

  INTERRUPTS_DISABLE();

  now = RTIMER_NOW();

  /*
   * New value must be 7 ticks in the future. 
   */
  if((int32_t)(t - now) < 7) {
    t = now + 7;
  }

  /* Set the match value */
  HibernateRTCMatchSet(0, (now + next_trigger) >> 15);
  HibernateRTCSSMatchSet(0, (now + next_trigger) & 0x7fff);
  
  /* Enable the match interrupt */
  HibernateIntEnable(HIBERNATE_INT_RTC_MATCH_0);

  INTERRUPTS_ENABLE();

  /* Store the value. The LPM module will query us for it */
  next_trigger = t;

  //nvic_interrupt_enable(NVIC_INT_SM_TIMER);
}
/*---------------------------------------------------------------------------*/
rtimer_clock_t
rtimer_arch_next_trigger()
{
  return next_trigger;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Returns the current real-time clock time
 * \return The current rtimer time in ticks
 */
rtimer_clock_t
rtimer_arch_now()
{
  rtimer_clock_t rv;

  /* Read the current RTC sub-seconds counter*/
  rv = (rtimer_clock_t)((HibernateRTCGet() << 15) | (HibernateRTCSSGet() & 0x7fff));

  return rv;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief The rtimer ISR
 *
 *        Interrupts are only turned on when we have an rtimer task to schedule
 *        Once the interrupt fires, the task is called and then interrupts no
 *        longer get acknowledged until the next task needs scheduled.
 */
void
rtimer_isr()
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  next_trigger = 0;

  /* Disable the match interrupt */
  HibernateIntDisable(HIBERNATE_INT_RTC_MATCH_0);

  rtimer_run_next();

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*---------------------------------------------------------------------------*/
/** @} */
