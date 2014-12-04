/*
 * Copyright (c) 2013, Texas Instruments Incorporated - http://www.ti.com/
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
 * \addtogroup tiva-c-ieee-addr
 * @{
 *
 * \file
 * Driver for Tiva C IEEE addresses
 */
#include "contiki-conf.h"
#include "net/rime/rimeaddr.h"
#include "dev/tivaware/driverlib/tm4c_flash.h"
#include "ieee-addr.h"

#include <stdint.h>
/*---------------------------------------------------------------------------*/
void
ieee_addr_cpy_to(uint8_t *dst, uint8_t len)
{
  uint32_t User0, User1;
  
  if(IEEE_ADDR_CONF_HARDCODED) {
    uint8_t ieee_addr_hc[8] = IEEE_ADDR_CONF_ADDRESS;
    memcpy(dst, &ieee_addr_hc[8 - len], len);
  } else if(IEEE_ADDR_CONF_IN_FLASH) {
    FlashUserGet(&User0, &User1);
    for(i = 0; i < len; i++) {
      switch(i) {
        case 0:
          dst[i] = ((User0 >>  0) & 0xff);
          break;
        case 1:
          dst[i] = ((User0 >>  8) & 0xff);
          break;
        case 2:
          dst[i] = ((User0 >>  16) & 0xff);
          break;
        case 3:
          dst[i] = ((User1 >>  0) & 0xff);
          break;
        case 4:
          dst[i] = ((User1 >>  8) & 0xff);
          break;
        case 5:
          dst[i] = ((User1 >>  16) & 0xff);
          break;
        default:
          dst[i] = 0;
          break;
      }
    }
  } 

#if IEEE_ADDR_NODE_ID
  dst[len - 1] = IEEE_ADDR_NODE_ID & 0xFF;
  dst[len - 2] = IEEE_ADDR_NODE_ID >> 8;
#endif
}

/** @} */
