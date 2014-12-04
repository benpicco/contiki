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
/** \addtogroup tiva-c
 * @{
 *
 * \defgroup ek-tm4c123gxl EK-TM4C123GXL Peripherals
 *
 * Defines related to the EK-TM4C123GXL
 *
 * This file provides connectivity information on LEDs, Buttons, UART and
 * other peripherals.
 *
 * This file can be used as the basis to configure other platforms using other
 * Tiva C series MCUs.
 * @{
 *
 * \file
 * Header file with definitions related to the I/O connections on the TI
 * Tiva C EK-TM4C123GXL LaunchPad.
 *
 * \note   Do not include this file directly. It gets included by contiki-conf
 *         after all relevant directives have been set.
 */
#ifndef BOARD_H_
#define BOARD_H_

#include "dev/gpio.h"

#include "tivaware/inc/hw_memmap.h"
#include "tivaware/inc/hw_gpio.h"
#include "tivaware/inc/hw_sysctl.h"

#include "cpu.h"

#define dint() INTERRUPTS_ENABLE()
#define eint() INTERRUPTS_DISABLE()
/*---------------------------------------------------------------------------*/
/** \name EK-TM4C123GXL LED configuration
 *
 * The EK-TM4C123GXL has only a single RGB LED (D1).  For this LED, 
 * there are 3 GPIO pins connected, one for each color.  They can be turned 
 * on one at a time to show red, green or blue, or all together to show white.
 *
 * @{
 */
/*---------------------------------------------------------------------------*/
#define LEDS_D1_RED    1 /**< Red -> PF1 */
#define LEDS_D1_GREEN  2 /**< Green -> PF3 */
#define LEDS_D1_BLUE   4 /**< Blue -> PF2 */
#define LEDS_D1_WHITE  7

/* Notify various examples that we have LEDs */
#define PLATFORM_HAS_LEDS        1
/** @} */
/*---------------------------------------------------------------------------*/
/** \name UART configuration
 *
 * On the EK-TM4C123GXL, the UART (XDS back channel) is connected to the
 * following ports/pins
 * - RX:  PA0
 * - TX:  PA1
 *
 * @{
 */
#define UART_RX_PORT             GPIO_PORTA_BASE
#define UART_RX_PIN              GPIO_PIN_0

#define UART_TX_PORT             GPIO_PORTA_BASE
#define UART_TX_PIN              GPIO_PIN_1
/** @} */
/*---------------------------------------------------------------------------*/
/** \name EK-TM4C123GXL Button configuration
 *
 * Buttons on the EK-TM4C123GXL are connected as follows:
 * - BUTTON_SW1 -> PF4
 * - BUTTON_SW2 -> PF0
 * @{
 */
/** BUTTON_SW1 -> PF4 */
#define BUTTON_SW1_PIN         GPIO_PIN_4
#define BUTTON_SW1_PORT        GPIO_PORTF_BASE
#define BUTTON_SW1_PORT_NO     GPIO_F_NUM
#define BUTTON_SW1_PIN_NO      4
#define BUTTON_SW1_SYSCTL_PORT SYSCTL_PERIPH_GPIOF
#define BUTTON_SW1_INT         INT_GPIOF

/** BUTTON_SW2 -> PF0 */
#define BUTTON_SW2_PIN         GPIO_PIN_0
#define BUTTON_SW2_PORT        GPIO_PORTF_BASE
#define BUTTON_SW2_PORT_NO     GPIO_F_NUM
#define BUTTON_SW2_PIN_NO      0
#define BUTTON_SW2_SYSCTL_PORT SYSCTL_PERIPH_GPIOF
#define BUTTON_SW2_INT         INT_GPIOF

/* Notify various examples that we have Buttons */
#define PLATFORM_HAS_BUTTON      1
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name Device string used on startup
 * @{
 */
#define BOARD_STRING "TI Tiva C Series EK-TM4C123GXL"
/** @} */

#endif /* BOARD_H_ */

/**
 * @}
 * @}
 */
