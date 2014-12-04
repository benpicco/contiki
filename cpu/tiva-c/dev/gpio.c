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
 * \addtogroup Tiva C tiva-c-gpio
 * @{
 *
 * \file
 * Implementation of the Tiva C GPIO controller
 */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "contiki.h"

#include "sys/energest.h"
#include "dev/leds.h"

#include "lpm.h"
#include "dev/gpio.h"

#include "tivaware/inc/hw_memmap.h"
#include "tivaware/inc/hw_types.h"
#include "tivaware/inc/hw_gpio.h"
#include "tivaware/inc/hw_sysctl.h"

/**
 * \brief Pointer to a function to be called when a GPIO interrupt is detected.
 * Callbacks for Port A, Pins[0:7] are stored in positions [0:7] of this
 * buffer, Port B callbacks in [8:15] and so on.
 */
static gpio_callback_t gpio_callbacks[GPIO_NUM_PORTS * 8];
/*---------------------------------------------------------------------------*/
void
gpio_register_callback(gpio_callback_t f, uint8_t port, uint8_t pin)
{
  gpio_callbacks[(port << 3) + pin] = f;
}
/*---------------------------------------------------------------------------*/
/** \brief Run through all registered GPIO callbacks and invoke those
 * associated with the \a port and the pins specified by \a mask
 * \param mask Search callbacks associated with pins specified by this mask
 * \param port Search callbacks associated with this port. Here, port is
 * specified as a number between 0 and 3. Port A: 0, Port B: 1 etc */
void
notify(uint8_t mask, uint8_t port)
{
  uint8_t i;
  gpio_callback_t *f = &gpio_callbacks[port << 3];

  for(i = 0; i < 8; i++) {
    if(mask & (1 << i)) {
      if((*f) != NULL) {
        (*f)(port, i);
      }
    }
    f++;
  }
}
/*---------------------------------------------------------------------------*/
/** \brief Interrupt service routine for Port A */
void
gpio_port_a_isr()
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  lpm_exit();

  notify(HWREG(GPIO_PORTA_BASE | GPIO_O_MIS), GPIO_A_NUM);

  HWREG(GPIO_PORTA_BASE | GPIO_O_ICR) = 0xFF;

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*---------------------------------------------------------------------------*/
/** \brief Interrupt service routine for Port B */
#if 0
void
gpio_port_b_isr()
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  lpm_exit();

  notify(HWREG(GPIO_PORTB_BASE | GPIO_O_MIS), GPIO_B_NUM);

  HWREG(GPIO_PORTB_BASE | GPIO_O_ICR) = 0xFF;

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
#endif
/*---------------------------------------------------------------------------*/
/** \brief Interrupt service routine for Port C */
void
gpio_port_c_isr()
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  lpm_exit();

  notify(HWREG(GPIO_PORTC_BASE | GPIO_O_MIS), GPIO_C_NUM);

  HWREG(GPIO_PORTC_BASE | GPIO_O_ICR) = 0xFF;

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*---------------------------------------------------------------------------*/
/** \brief Interrupt service routine for Port D */
#if 0
/*Define in cc1120-tiva-arch.c file as the PORT D Interrupt Handler*/
void
gpio_port_d_isr()
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  lpm_exit();

  notify(HWREG(GPIO_PORTD_BASE | GPIO_O_MIS), GPIO_D_NUM);

  HWREG(GPIO_PORTD_BASE | GPIO_O_ICR) = 0xFF;

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
#endif
/*---------------------------------------------------------------------------*/
/** \brief Interrupt service routine for Port E */
void
gpio_port_e_isr()
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  lpm_exit();

  notify(HWREG(GPIO_PORTE_BASE | GPIO_O_MIS), GPIO_E_NUM);

  HWREG(GPIO_PORTE_BASE | GPIO_O_ICR) = 0xFF;

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*---------------------------------------------------------------------------*/
/** \brief Interrupt service routine for Port F */
void
gpio_port_f_isr()
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  lpm_exit();

  notify(HWREG(GPIO_PORTF_BASE | GPIO_O_MIS), GPIO_F_NUM);

  HWREG(GPIO_PORTF_BASE | GPIO_O_ICR) = 0xFF;

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*---------------------------------------------------------------------------*/
/** \brief Interrupt service routine for Port G */
void
gpio_port_g_isr()
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  lpm_exit();

  notify(HWREG(GPIO_PORTG_BASE | GPIO_O_MIS), GPIO_G_NUM);

  HWREG(GPIO_PORTG_BASE | GPIO_O_ICR) = 0xFF;

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*---------------------------------------------------------------------------*/
/** \brief Interrupt service routine for Port H */
void
gpio_port_h_isr()
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  lpm_exit();

  notify(HWREG(GPIO_PORTH_BASE | GPIO_O_MIS), GPIO_H_NUM);

  HWREG(GPIO_PORTH_BASE | GPIO_O_ICR) = 0xFF;

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*---------------------------------------------------------------------------*/
/** \brief Interrupt service routine for Port J */
void
gpio_port_j_isr()
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  lpm_exit();
  
  notify(HWREG(GPIO_PORTJ_BASE | GPIO_O_MIS), GPIO_J_NUM);

  HWREG(GPIO_PORTJ_BASE | GPIO_O_ICR) = 0xFF;

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*---------------------------------------------------------------------------*/
/** \brief Interrupt service routine for Port K */
void
gpio_port_k_isr()
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  lpm_exit();

  notify(HWREG(GPIO_PORTK_BASE | GPIO_O_MIS), GPIO_K_NUM);

  HWREG(GPIO_PORTK_BASE | GPIO_O_ICR) = 0xFF;

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*---------------------------------------------------------------------------*/
/** \brief Interrupt service routine for Port L */
void
gpio_port_l_isr()
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  lpm_exit();

  notify(HWREG(GPIO_PORTL_BASE | GPIO_O_MIS), GPIO_L_NUM);

  HWREG(GPIO_PORTL_BASE | GPIO_O_ICR) = 0xFF;

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*---------------------------------------------------------------------------*/
#if 0
/** \brief Interrupt service routine for Port M */
void
gpio_port_m_isr()
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  lpm_exit();

  notify(HWREG(GPIO_PORTM_BASE | GPIO_O_MIS), GPIO_M_NUM);

  HWREG(GPIO_PORTM_BASE | GPIO_O_ICR) = 0xFF;

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
#endif
/*---------------------------------------------------------------------------*/
/** \brief Interrupt service routine for Port N */
void
gpio_port_n_isr()
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  lpm_exit();

  notify(HWREG(GPIO_PORTN_BASE | GPIO_O_MIS), GPIO_N_NUM);

  HWREG(GPIO_PORTN_BASE | GPIO_O_ICR) = 0xFF;

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*---------------------------------------------------------------------------*/
/** \brief Interrupt service routine for Port P */
void
gpio_port_p_isr()
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  lpm_exit();

  notify(HWREG(GPIO_PORTP_BASE | GPIO_O_MIS), GPIO_P_NUM);

  HWREG(GPIO_PORTP_BASE | GPIO_O_ICR) = 0xFF;

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*---------------------------------------------------------------------------*/
/** \brief Interrupt service routine for Port Q */
void
gpio_port_q_isr()
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  lpm_exit();

  notify(HWREG(GPIO_PORTQ_BASE | GPIO_O_MIS), GPIO_Q_NUM);

  HWREG(GPIO_PORTQ_BASE | GPIO_O_ICR) = 0xFF;

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*---------------------------------------------------------------------------*/
/** \brief Interrupt service routine for Port R */
void
gpio_port_r_isr()
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  lpm_exit();

  notify(HWREG(GPIO_PORTR_BASE | GPIO_O_MIS), GPIO_R_NUM);

  HWREG(GPIO_PORTR_BASE | GPIO_O_ICR) = 0xFF;

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*---------------------------------------------------------------------------*/
/** \brief Interrupt service routine for Port S */
void
gpio_port_s_isr()
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  lpm_exit();

  notify(HWREG(GPIO_PORTS_BASE | GPIO_O_MIS), GPIO_S_NUM);

  HWREG(GPIO_PORTS_BASE | GPIO_O_ICR) = 0xFF;

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*---------------------------------------------------------------------------*/
/** \brief Interrupt service routine for Port T */
void
gpio_port_t_isr()
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  lpm_exit();

  notify(HWREG(GPIO_PORTT_BASE | GPIO_O_MIS), GPIO_T_NUM);

  HWREG(GPIO_PORTT_BASE | GPIO_O_ICR) = 0xFF;

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*---------------------------------------------------------------------------*/
void
gpio_init()
{
  memset(gpio_callbacks, 0, sizeof(gpio_callbacks));
}
/** @} */
