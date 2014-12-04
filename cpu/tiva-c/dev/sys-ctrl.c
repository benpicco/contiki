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
 * \addtogroup tiva-c-sys-ctrl
 * @{
 *
 * \file
 * Implementation of the Tiva C System Control driver
 */
#include <stdbool.h>
#include <stdint.h> 
 
#include "cpu.h"
#include "contiki-conf.h"
#include "dev/sys-ctrl.h"
#include "dev/tivaware/driverlib/tm4c_sysctl.h"
#include "dev/tivaware/driverlib/tm4c_fpu.h"

#include <stdint.h>

/*---------------------------------------------------------------------------*/
uint32_t
sys_ctrl_init()
{
#if defined(TARGET_IS_BLIZZARD_RA1) || defined(TARGET_IS_BLIZZARD_RA3) || \
    defined(TARGET_IS_BLIZZARD_RB0) || defined(TARGET_IS_BLIZZARD_RB1)
    
  FPULazyStackingEnable();
  FPUEnable();
  
  /*
   * Desired Clock configuration:
   * System Clock: 80 MHz
   */
  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                 SYSCTL_XTAL_16MHZ); 

  return(SysCtlClockGet());
#else
  /*
   * Desired Clock configuration:
   * System Clock: 120 MHz
   */
  return(SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                             SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000));
#endif
}
/*---------------------------------------------------------------------------*/
void
sys_ctrl_reset()
{
  SysCtlReset();
}

/**
 * @}
 * @}
 */
