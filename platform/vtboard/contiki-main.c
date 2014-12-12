#include <stdint.h>
#include <stdio.h>

#include "contiki-conf.h"

#include <sys/autostart.h>

#include <dev/sys-ctrl.h>
#include <dev/serial-line.h>
#include <dev/uart.h>
#include <dev/leds.h>

#include <ip/uip.h>
#include <net/queuebuf.h>
#include <net/netstack.h>
#include <net/ipv6/uip-ds6.h>

#include "dev/cc2520/cc2520.h"

#include "dev/tivaware/driverlib/tm4c_gpio.h"
#include "dev/tivaware/driverlib/tm4c_pin_map.h"
#include "dev/tivaware/driverlib/tm4c_ssi.h"
#include "dev/tivaware/driverlib/tm4c_cpu.h"
#include "dev/tivaware/driverlib/tm4c_sysctl.h"

static void
print_processes(struct process * const processes[])
{
  /*  const struct process * const * p = processes;*/
  printf("Starting");
  while(*processes != NULL) {
    printf(" '%s'", (*processes)->name);
    processes++;
  }
  printf("\n");
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

  SSIConfigSetExpClk(SSI1_BASE, F_CPU, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 1000000, 8);

  SSIEnable(SSI1_BASE);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_0);
  GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_1);
  GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_2);
  GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_3);

  GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, PIN(6) | PIN(7));
}

uint32_t sys_clock = 0;

int main(void)
{
  sys_clock = sys_ctrl_init();

  uart_init();
  uart_set_input(serial_line_input_byte);
  printf("Initialising\n");
  
  gpio_init();
  leds_init();
  clock_init();
  process_init();
  process_start(&etimer_process, NULL);

  cc2520_init();
  
  uint8_t longaddr[8] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8};
  uint16_t shortaddr = 0xfefe;

  printf("MAC %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x ",
         longaddr[0], longaddr[1], longaddr[2], longaddr[3],
         longaddr[4], longaddr[5], longaddr[6], longaddr[7]);

  cc2520_set_pan_addr(0x2520, shortaddr, longaddr);
  
  cc2520_set_channel(RF_CHANNEL);


#if NETSTACK_CONF_WITH_IPV6
  /* memcpy(&uip_lladdr.addr, ds2411_id, sizeof(uip_lladdr.addr)); */
  memcpy(&uip_lladdr.addr, &longaddr, 8);

  /* Setup nullmac-like MAC for 802.15.4 */
/*   sicslowpan_init(sicslowmac_init(&cc2520_driver)); */
/*   printf(" %s channel %u\n", sicslowmac_driver.name, RF_CHANNEL); */

  /* Setup X-MAC for 802.15.4 */
  queuebuf_init();
  NETSTACK_RDC.init();
  NETSTACK_MAC.init();
  NETSTACK_NETWORK.init();

  printf("%s %s, channel check rate %lu Hz, radio channel %u\n",
         NETSTACK_MAC.name, NETSTACK_RDC.name,
         CLOCK_SECOND / (NETSTACK_RDC.channel_check_interval() == 0 ? 1:
                         NETSTACK_RDC.channel_check_interval()),
         RF_CHANNEL);

  process_start(&tcpip_process, NULL);

  printf("Tentative link-local IPv6 address ");
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

  if(!UIP_CONF_IPV6_RPL) {
    uip_ipaddr_t ipaddr;
    int i;
    uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
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

#else /* NETSTACK_CONF_WITH_IPV6 */

  NETSTACK_RDC.init();
  NETSTACK_MAC.init();
  NETSTACK_NETWORK.init();

  printf("%s %s, channel check rate %lu Hz, radio channel %u\n",
         NETSTACK_MAC.name, NETSTACK_RDC.name,
         CLOCK_SECOND / (NETSTACK_RDC.channel_check_interval() == 0? 1:
                         NETSTACK_RDC.channel_check_interval()),
         RF_CHANNEL);
#endif /* NETSTACK_CONF_WITH_IPV6 */

  autostart_start(autostart_processes);
  printf("Processes running\n");

  print_processes(autostart_processes);

  while(1) {
    do {
    } while(process_run() > 0);

    /* Idle! */
    /* Stop processor clock */
    /* asm("wfi"::); */ 
  }
  return 0;
}