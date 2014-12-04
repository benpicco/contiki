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
 * \defgroup tiva-c-gpio Tiva C General-Purpose I/O
 *
 * Driver for the Tiva C GPIO controller
 * @{
 *
 * \file
 * Header file with register and macro declarations for the Tiva C GPIO module
 */
#ifndef GPIO_H_
#define GPIO_H_

#include <stdbool.h>
#include <stdint.h>

#include "tivaware/driverlib/tm4c_gpio.h"

/**
 * \brief Type definition for callbacks invoked by the GPIO ISRs
 * \param port The port that triggered the GPIO interrupt. \e port is passed
 *        by its numeric representation (Port A:0, B:1 etc). Defines for
 *        these numeric representations are GPIO_x_NUM
 * \param pin The pin that triggered the interrupt, specified by number
 *        (0, 1, ..., 7)
 *
 * This is the prototype of a function pointer passed to
 * gpio_register_callback(). These callbacks are registered on a port/pin
 * basis. When a GPIO port generates an interrupt, if a callback has been
 * registered for the port/pin combination, the ISR will invoke it. The ISR
 * will pass the port/pin as arguments in that call, so that a developer can
 * re-use the same callback for multiple port/pin combinations
 */
typedef void (* gpio_callback_t)(uint8_t port, uint8_t pin);
/** @} */
/*---------------------------------------------------------------------------*/
/** \name Numeric representation of the four GPIO ports
 * @{
 */
#define GPIO_NUM_PORTS          18
 
#define GPIO_A_NUM              0  /**< GPIO_A: 0 */
#define GPIO_B_NUM              1  /**< GPIO_B: 1 */
#define GPIO_C_NUM              2  /**< GPIO_C: 2 */
#define GPIO_D_NUM              3  /**< GPIO_D: 3 */
#define GPIO_E_NUM              4  /**< GPIO_E: 4 */
#define GPIO_F_NUM              5  /**< GPIO_F: 5 */
#define GPIO_G_NUM              6  /**< GPIO_G: 6 */
#define GPIO_H_NUM              7  /**< GPIO_H: 7 */
#define GPIO_J_NUM              8  /**< GPIO_J: 8 */
#define GPIO_K_NUM              9  /**< GPIO_K: 9 */
#define GPIO_L_NUM              10 /**< GPIO_L: 10 */
#define GPIO_M_NUM              11 /**< GPIO_M: 11 */
#define GPIO_N_NUM              12 /**< GPIO_N: 12 */
#define GPIO_P_NUM              13 /**< GPIO_P: 13 */
#define GPIO_Q_NUM              14 /**< GPIO_Q: 14 */
#define GPIO_R_NUM              15 /**< GPIO_R: 15 */
#define GPIO_S_NUM              16 /**< GPIO_S: 16 */
#define GPIO_T_NUM              17 /**< GPIO_T: 17 */

/** @} */
/*---------------------------------------------------------------------------*/
/** \brief Initialise the GPIO module */
void gpio_init();

/**
 * \brief Register GPIO callback
 * \param f Pointer to a function to be called when \a pin of \a port
 *          generates an interrupt
 * \param port Associate \a f with this port. \e port must be specified with
 *        its numeric representation (Port A:0, B:1 etc). Defines for these
 *        numeric representations are GPIO_x_NUM
 * \param pin Associate \a f with this pin, which is specified by number
 *        (0, 1, ..., 7)
 */
void gpio_register_callback(gpio_callback_t f, uint8_t port, uint8_t pin);

#endif /* GPIO_H_ */

/**
 * @}
 * @}
 */
