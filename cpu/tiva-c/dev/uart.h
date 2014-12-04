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
 * \defgroup tiva-c-uart Tiva C UART
 *
 * Driver for the Tiva C UART controller
 * @{
 *
 * \file
 * Header file for the Tiva C UART driver
 */
#ifndef UART_H_
#define UART_H_

#include "contiki.h"

#include <stdint.h>

/*---------------------------------------------------------------------------*/
/**
 * \name Baud rate defines
 *
 * Used in uart_init() to set the baud rate. 
 *
 * @{
 */
#if !(UART_CONF_BAUD_RATE==115200 || UART_CONF_BAUD_RATE==23400 || UART_CONF_BAUD_RATE==460800)
#error "Check the value of UART_CONF_BAUD_RATE in contiki-conf.h or project-conf.h"
#error "Supported values are 115200, 230400 and 460800."
#endif
/** @} */

/** \brief Initialises the UART controller, configures I/O control
 * and interrupts */
void uart_init(void);

/** \brief Sends a single character down the UART
 * \param b The character to transmit
 */
void uart_write_byte(uint8_t b);

/** \brief Assigns a callback to be called when the UART receives a byte
 * \param input A pointer to the function
 */
void uart_set_input(int (* input)(unsigned char c));

/** @} */

#endif /* UART_H_ */

/**
 * @}
 * @}
 */
