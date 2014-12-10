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
 * \addtogroup platform
 * @{
 *
 * \defgroup ek-tm4c1294xl The EK-TM4C1294XL LaunchPad Kit platform
 *
 * The EK-TM4C1294XL is the latest product in the Tiva C LaunchPad family.
 * @{
 *
 * \file
 *   Main module for the ek-tm4c1294xl platform
 */
/*---------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "contiki.h"

#include "dev/button-sensor.h"
#if CC11xx_CC1101 || CC11xx_CC1120
#include "cc11xx.h"
#endif /* CC11xx_CC1101 || CC11xx_CC1120 */

//#include "dev/flash.h"
#include "dev/leds.h"
#include "sys/timer.h"
#include "dev/gpio.h"
#include "dev/serial-line.h"
#include "dev/sys-ctrl.h"
#include "dev/slip.h"//IP packet send
#include "dev/uart1.h"
#include "dev/uart.h"
#include "dev/watchdog.h"
//#include "dev/xmem.h"
//#include "lib/random.h"
#include "lib/sensors.h"

#include "dev/tivaware/driverlib/tm4c_gpio.h"
#include "dev/tivaware/driverlib/tm4c_pin_map.h"
#include "dev/tivaware/driverlib/tm4c_ssi.h"
#include "dev/tivaware/driverlib/tm4c_cpu.h"
#include "dev/tivaware/driverlib/tm4c_sysctl.h"

#include "net/mac/frame802154.h"
#include "net/netstack.h"
#include "net/rime/rime.h"
#include "net/rpl/rpl.h"
#include "sys/autostart.h"

#include "node-id.h"
//#include "lcd.h"
//#include "duty-cycle-scroller.h"

#include "lpm.h"

#include "cpu.h"

#if WITH_UIP6
#include "net/uip-ds6.h"
#endif /* WITH_UIP6 */


#if NETSTACK_AES_KEY_DEFAULT
#warning Using default AES key "thingsquare mist", change it in project-conf.h like this:
#warning #define NETSTACK_AES_KEY {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}
#endif /* NETSTACK_AES_KEY */

/*---------------------------------------------------------------------------*/
#define DEBUG 1

#if STARTUP_CONF_VERBOSE
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#if UART_CONF_ENABLE
#define PUTS(s) puts(s)
#else
#define PUTS(s)
#endif

/*---------------------------------------------------------------------------*/
uint32_t sys_clock = 0;
uint32_t state = 0;

static struct etimer et;

SENSORS(&button_sensor);

/*---------------------------------------------------------------------------*/
#ifndef RF_CHANNEL
#define RF_CHANNEL              10
#endif
/*---------------------------------------------------------------------------*/
static void
set_rime_addr(void)
{
  linkaddr_t addr;
  uint8_t i;
  memset(&addr, 0, sizeof(linkaddr_t));
#if UIP_CONF_IPV6
  memcpy(addr.u8, node_mac, sizeof(addr.u8));
#else
  if(node_id == 0) {
    for(i = 0; i < sizeof(linkaddr_t); ++i) {
      addr.u8[i] = node_mac[7 - i];
    }
  } else {
    addr.u8[0] = node_id & 0xff;
    addr.u8[1] = node_id >> 8;
  }
#endif
  linkaddr_set_node_addr(&addr);
  printf("Rime addr ");
  for(i = 0; i < sizeof(addr.u8) - 1; i++) {
    printf("%u.", addr.u8[i]);
  }
  printf("%u\n", addr.u8[i]);
}

void splx(int saved) {
  ;
}

int splhigh(void) {
  return 0;
}

void GPIOPinUnlockGPIO(uint32_t ui32Port, uint8_t ui8Pins) {
  HWREG(ui32Port + GPIO_O_LOCK) = GPIO_LOCK_KEY;    // Unlock the port
  HWREG(ui32Port + GPIO_O_CR) |= ui8Pins;       // Unlock the Pin
  HWREG(ui32Port + GPIO_O_LOCK) = 0;          // Lock the port
}

void cc2520_arch_init(void) {
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

  GPIOPinUnlockGPIO(GPIO_PORTD_BASE, GPIO_PIN_7);
  GPIOPinUnlockGPIO(GPIO_PORTF_BASE, GPIO_PIN_0);

  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);

  GPIOPinConfigure(GPIO_PF0_SSI1RX);
  GPIOPinConfigure(GPIO_PF1_SSI1TX);
  GPIOPinConfigure(GPIO_PF2_SSI1CLK);

  GPIOPinTypeSSI(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_1| GPIO_PIN_0);

  SSIConfigSetExpClk(SSI1_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 1000000, 8);

  SSIEnable(SSI1_BASE);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_0);
  GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_1);
  GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_2);
  GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_3);

  GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, PIN(6) | PIN(7));
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Main routine for the ek-tm4c1294xl platform
 */
int
main(void)
{
  /*
   * Initalize hardware.
   */
  sys_clock = sys_ctrl_init();
  clock_init();
  /*Disable EA*/
  dint();
  /*Init Watchdog*/
  watchdog_init();//Need to check the watchdog on 123gxl

  /*Enable EA*/
  //eint();

  rtimer_init();

  lpm_init();
#if 1
	SysCtlPeripheralClockGating(true);
	SysCtlPeripheralDeepSleepEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlDeepSleepPowerSet(SYSCTL_DSLPPWRCFG_FLASHPM_SLP);
	SysCtlDeepSleepPowerSet(SYSCTL_DSLPPWRCFG_SRAMPM_LP);
#endif

  gpio_init();

  leds_init();

  button_sensor_init();
  /*
   * Character I/O Initialization.
   * When the UART receives a character it will call serial_line_input_byte to
   * notify the core. The same applies for the USB driver.
   *
   * If slip-arch is also linked in afterwards (e.g. if we are a border router)
   * it will overwrite one of the two peripheral input callbacks. Characters
   * received over the relevant peripheral will be handled by
   * slip_input_byte instead
   */
#if UART_CONF_ENABLE
  uart_init();
  uart_set_input(serial_line_input_byte);
#endif
  //serial_line_init();

  /*Enable EA*/
  eint();
  /*
   * Hardware initialization done!
   */
  PUTS(CONTIKI_VERSION_STRING);
  PUTS(BOARD_STRING);
  /* Restore node id if such has been stored in external mem */
#ifdef NODEID
  node_id = NODEID;
#ifdef BURN_NODEID
  node_id_burn(node_id);
  node_id_restore(); /* also configures node_mac[] */
#endif /* BURN_NODEID */
#else
  node_id_restore(); /* also configures node_mac[] */
#endif /* NODE_ID */

  /* for setting "hardcoded" IEEE 802.15.4 MAC addresses */
#ifdef MAC_1
  {
    uint8_t ieee[] = { MAC_1, MAC_2, MAC_3, MAC_4, MAC_5, MAC_6, MAC_7, MAC_8 };
    memcpy(node_mac, ieee, sizeof(uip_lladdr.addr));
  }
#endif

  /*
   * Initialize Contiki and our processes.
   */
  process_init();
  process_start(&sensors_process, NULL);
  button_sensor_init();

  process_start(&etimer_process, NULL);

  ctimer_init();

  set_rime_addr();

  /*Need to add random address*/
  //random_init(node_id);
  printf("finish addr seting\n");

  NETSTACK_RADIO.init();
#if CC11xx_CC1101 || CC11xx_CC1120
  printf("Starting up cc11xx radio at channel %d\n", RF_CHANNEL);
  cc11xx_channel_set(RF_CHANNEL);
#endif /* CC11xx_CC1101 || CC11xx_CC1120 */

  NETSTACK_RADIO.on();

  if(node_id > 0) {
    printf("Node id %u.\n", node_id);
  } else {
    printf("Node id not set.\n");
  }


#if WITH_UIP6
  memcpy(&uip_lladdr.addr, node_mac, sizeof(uip_lladdr.addr));
  /* Setup nullmac-like MAC for 802.15.4 */

  queuebuf_init();

  netstack_init();

  printf("%s/%s %lu %u\n",
         NETSTACK_RDC.name,
         NETSTACK_MAC.name,
         CLOCK_SECOND / (NETSTACK_RDC.channel_check_interval() == 0 ? 1:
                         NETSTACK_RDC.channel_check_interval()),
         RF_CHANNEL);

  process_start(&tcpip_process, NULL);

  printf("IPv6 ");
  {
    uip_ds6_addr_t *lladdr;
    int i;
    lladdr = uip_ds6_get_link_local(-1);
    for(i = 0; i < 7; ++i) {
      printf("%02x%02x:", lladdr->ipaddr.u8[i * 2],
             lladdr->ipaddr.u8[i * 2 + 1]);
    }
    printf("%02x%02x\n", lladdr->ipaddr.u8[14], lladdr->ipaddr.u8[15]);
  }

  if(1) {
    uip_ipaddr_t ipaddr;
    int i;
    uip_ip6addr(&ipaddr, 0xfc00, 0, 0, 0, 0, 0, 0, 0);
    uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
    uip_ds6_addr_add(&ipaddr, 0, ADDR_TENTATIVE);
    printf("Tentative global IPv6 address ");
    for(i = 0; i < 7; ++i) {
      printf("%02x%02x:",
             ipaddr.u8[i * 2], ipaddr.u8[i * 2 + 1]);
    }
    printf("%02x%02x\n",
           ipaddr.u8[7 * 2], ipaddr.u8[7 * 2 + 1]);
  }

#else /* WITH_UIP6 */

  netstack_init();

  printf("%s %lu %u\n",
         NETSTACK_RDC.name,
         CLOCK_SECOND / (NETSTACK_RDC.channel_check_interval() == 0? 1:
                         NETSTACK_RDC.channel_check_interval()),
         RF_CHANNEL);
#endif /* WITH_UIP6 */

#if !WITH_UIP6
  uart1_set_input(serial_line_input_byte);
  serial_line_init();
#endif

#ifdef NETSTACK_AES_H
#ifndef NETSTACK_AES_KEY
#error Please define NETSTACK_AES_KEY!
#endif /* NETSTACK_AES_KEY */
  {
    const uint8_t key[] = NETSTACK_AES_KEY;
    netstack_aes_set_key(key);
  }
  /*printf("AES encryption is enabled: '%s'\n", NETSTACK_AES_KEY);*/
  printf("AES encryption is enabled\n");
#else /* NETSTACK_AES_H */
  printf("Warning: AES encryption is disabled\n");
#endif /* NETSTACK_AES_H */

#if TIMESYNCH_CONF_ENABLED
  timesynch_init();
  timesynch_set_authority_level(rimeaddr_node_addr.u8[0]);
#endif /* TIMESYNCH_CONF_ENABLED */

#if CC11xx_CC1101 || CC11xx_CC1120
  printf("cc11xx radio at channel %d\n", RF_CHANNEL);
  cc11xx_channel_set(RF_CHANNEL);
#endif /* CC11xx_CC1101 || CC11xx_CC1120 */
  NETSTACK_RADIO.on();

  /*  process_start(&sensors_process, NULL);
      SENSORS_ACTIVATE(button_sensor);*/

  energest_init();
  ENERGEST_ON(ENERGEST_TYPE_CPU);

//  rpl_init();
  /*Watch dog configuration*/
  watchdog_periodic();
  watchdog_start();

  autostart_start(autostart_processes);

  //duty_cycle_scroller_start(CLOCK_SECOND * 2);
#if IP64_CONF_UIP_FALLBACK_INTERFACE_SLIP && WITH_SLIP
  /* Start the SLIP */
  printf("Initiating SLIP: my IP is 172.16.0.2...\n");
  slip_arch_init(0);
  {
    uip_ip4addr_t ipv4addr, netmask;

    uip_ipaddr(&ipv4addr, 172, 16, 0, 2);
    uip_ipaddr(&netmask, 255, 255, 255, 0);
    ip64_set_ipv4_address(&ipv4addr, &netmask);
  }
  uart1_set_input(slip_input_byte);
#endif /* IP64_CONF_UIP_FALLBACK_INTERFACE_SLIP */
  /*
   * This is the scheduler loop.
   */
  while(1) 
  {
    int r;
    do {
      /* Reset watchdog. */
      watchdog_periodic();
      r = process_run();
    } while(r > 0);
  }
}

/*---------------------------------------------------------------------------*/

/**
 * @}
 * @}
 */
