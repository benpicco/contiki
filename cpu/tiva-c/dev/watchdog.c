/*
 * Copyright (c) 2005, Swedish Institute of Computer Science
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
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 * @(#)$Id: watchdog.c,v 1.12 2010/11/12 15:54:41 nifi Exp $
 */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
 
#include "contiki.h"
#include "contiki-conf.h"

#include "sys/energest.h"
#include "dev/tivaware/inc/hw_memmap.h"
#include "dev/tivaware/inc/hw_types.h"
#include "dev/tivaware/inc/hw_ints.h"
#include "dev/tivaware/inc/hw_watchdog.h"

#include "dev/tivaware/driverlib/tm4c_gpio.h"
#include "dev/tivaware/driverlib/tm4c_interrupt.h"
#include "dev/tivaware/driverlib/tm4c_pin_map.h"
#include "dev/tivaware/driverlib/tm4c_sysctl.h"
#include "dev/tivaware/driverlib/tm4c_watchdog.h"

#include "dev/watchdog.h"
#include "dev/serial-line.h"
//void watchdog_isr(void);
//static int counter = 0;
#if 0
ISR(WDT, watchdog_interrupt)
{
#ifdef CONTIKI_TARGET_SKY
#if PRINT_STACK_ON_REBOOT
  uint8_t dummy;
  static uint8_t *ptr;
  static int i;

  ptr = &dummy;
  printstring("Watchdog reset");
  printstring("\nStack at $");
  hexprint(((int)ptr) >> 8);
  hexprint(((int)ptr) & 0xff);
  printstring(":\n");

  for(i = 0; i < 64; ++i) {
    hexprint(ptr[i]);
    printchar(' ');
    if((i & 0x0f) == 0x0f) {
      printchar('\n');
    }
  }
  printchar('\n');
#endif /* PRINT_STACK_ON_REBOOT */
#endif /* CONTIKI_TARGET_SKY */

  watchdog_reboot();
}
#endif

/*---------------------------------------------------------------------------*/
void
watchdog_init(void)
{
  /*Enable the peripherals for Watchdog 0*/
  SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);

  /*This function enables the capability of the watchdog timer to issue a reset
  to the processor after a second timeout condition.*/
  WatchdogResetEnable(WATCHDOG0_BASE);

  /*Enable reset generation from the watchdog timer.*/
  IntEnable(INT_WATCHDOG);
}
/*---------------------------------------------------------------------------*/
void
watchdog_start(void)
{
  /* This function checks to see if the watchdog timer is enabled. */
  if(WatchdogRunning(WATCHDOG0_BASE)==false)
  {
    WatchdogEnable(WATCHDOG0_BASE);
  }
}
/*---------------------------------------------------------------------------*/
/** \brief In watchdog mode, the WDT can not be stopped. This function is
 * defined here to satisfy API requirements.
 */
void
watchdog_stop(void)
{
  return;
}
/*---------------------------------------------------------------------------*/
void
watchdog_periodic(void)
{
  /* This function is called periodically to restart the watchdog timer. 
  The time based on sys clock */
  WatchdogReloadSet(WATCHDOG0_BASE, 120000000);
}
/*---------------------------------------------------------------------------*/
void
watchdog_lock(void)
{
  /*This function locks out write access to the watchdog timer configuration registers.*/
  WatchdogLock(WATCHDOG0_BASE);
}
/*---------------------------------------------------------------------------*/
void watchdog_unlock(void)
{
  /*This function enables write access to the watchdog timer configuration registers.*/
  WatchdogUnlock(WATCHDOG0_BASE);
}
/*---------------------------------------------------------------------------*/
void 
watchdog_isr(void)
{
  if(WatchdogIntStatus(WATCHDOG0_BASE, true)) 
  {
    WatchdogIntClear(WATCHDOG0_BASE);
  }
  
}
/*---------------------------------------------------------------------------*/





/******************************************************************************/

