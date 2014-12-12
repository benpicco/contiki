
#ifndef __PLATFORM_CONF_H__
#define __PLATFORM_CONF_H__

/* CPU target speed in Hz; works fine at 8, 16, 18 MHz but not higher. */
#define F_CPU 80000000uL

/* Our clock resolution, this is the same as Unix HZ. */
#define CLOCK_CONF_SECOND 128UL

#define BAUD2UBR(baud) ((F_CPU/baud))

#define CCIF
#define CLIF

#define HAVE_STDINT_H
#include "board.h"

#define CONCAT2(a, b)		(a##b)
#define CONCAT3(a, b, c)	(a##b##c)
#define PORT(port)			CONCAT3(GPIO_PORT, port, _BASE)
#define PIN(pin) 			CONCAT2(GPIO_PIN_, pin)
#define PERIPH(port)		CONCAT2(SYSCTL_PERIPH_, port)
#define PERIPH_GPIO(port)	CONCAT2(SYSCTL_PERIPH_GPIO, port)

#define BASE(module) 		CONCAT2(module, _BASE)

/* Types for clocks and uip_stats */
typedef unsigned short uip_stats_t;
typedef unsigned long clock_time_t;
//typedef unsigned long off_t;

/* DCO speed resynchronization for more robust UART, etc. */
/* Not needed from MSP430x5xx since it make use of the FLL */
#define DCOSYNCH_CONF_ENABLED 0
#define DCOSYNCH_CONF_PERIOD 30

#define ROM_ERASE_UNIT_SIZE  512
#define XMEM_ERASE_UNIT_SIZE (64*1024L)

#define CFS_CONF_OFFSET_TYPE    long

/* Use the first 64k of external flash for node configuration */
#define NODE_ID_XMEM_OFFSET     (0 * XMEM_ERASE_UNIT_SIZE)

/* Use the second 64k of external flash for codeprop. */
#define EEPROMFS_ADDR_CODEPROP  (1 * XMEM_ERASE_UNIT_SIZE)

#define CFS_XMEM_CONF_OFFSET    (2 * XMEM_ERASE_UNIT_SIZE)
#define CFS_XMEM_CONF_SIZE      (1 * XMEM_ERASE_UNIT_SIZE)

#define CFS_RAM_CONF_SIZE 4096


/*
 * SPI bus configuration for the vtboard
 */

#define SPI_BASE  SSI1_BASE

#define GPIO_PIN_REG(port, pin)	(HWREG(port + (GPIO_O_DATA + (pin << 2))))

/* SPI input/output registers. */
#define SPI_TXBUF HWREG(SPI_BASE + SSI_O_DR)
#define SPI_RXBUF HWREG(SPI_BASE + SSI_O_DR)

/* USART0 Tx ready? */
#define SPI_WAITFOREOTx()		while((HWREG(SPI_BASE + SSI_O_SR) & SSI_SR_BSY))
/* USART0 Rx ready? */
#define SPI_WAITFOREORx()		while(!(HWREG(SPI_BASE + SSI_O_SR) & SSI_SR_RNE))
/* USART0 Tx buffer ready? */
#define SPI_WAITFORTxREADY()	while(!(HWREG(SPI_BASE + SSI_O_SR) & SSI_SR_TNF))
/* Clear Rx buffer */
#define SPI_FLUSH()				while((HWREG(SPI_BASE + SSI_O_SR) & SSI_SR_RNE)) { SPI_RXBUF; }

/*
 * SPI bus - CC2520 pin configuration.
 */

#define CC2520_CONF_SYMBOL_LOOP_COUNT 2604      /* 326us msp430X @ 16MHz */

/* Pin status.CC2520 */
#define CC2520_FIFOP_IS_1 (!!(GPIO_PIN_REG(PORT(E), PIN(2))))
#define CC2520_FIFO_IS_1  (!!(GPIO_PIN_REG(PORT(E), PIN(1))))
#define CC2520_CCA_IS_1   (!!(GPIO_PIN_REG(PORT(E), PIN(3))))
#define CC2520_SFD_IS_1   (!!(GPIO_PIN_REG(PORT(E), PIN(3))))

/* The CC2520 reset pin. */
#define SET_RESET_INACTIVE()   (GPIO_PIN_REG(PORT(D), PIN(6)) = PIN(6))
#define SET_RESET_ACTIVE()     (GPIO_PIN_REG(PORT(D), PIN(6)) = 0)

/* CC2520 voltage regulator enable pin. */
#define SET_VREG_ACTIVE()       (GPIO_PIN_REG(PORT(D), PIN(7)) = PIN(7))
#define SET_VREG_INACTIVE()     (GPIO_PIN_REG(PORT(D), PIN(7)) = 0)

/* CC2520 rising edge trigger for external interrupt 0 (FIFOP). */
#define CC2520_FIFOP_INT_INIT() do {                  \
    GPIOIntTypeSet(PORT(E), PIN(2), GPIO_RISING_EDGE);  \
    IntEnable(INT_GPIOE);                         \
  } while(0)

/* FIFOP on external interrupt 0. */
/* FIFOP on external interrupt 0. */
#define CC2520_ENABLE_FIFOP_INT()          do { HWREG(PORT(E) + GPIO_O_IM) |= PIN(2); } while (0)
#define CC2520_DISABLE_FIFOP_INT()         do { HWREG(PORT(E) + GPIO_O_IM) &= ~PIN(2); } while (0)
#define CC2520_CLEAR_FIFOP_INT()           do { HWREG(PORT(E) + GPIO_O_ICR) = PIN(2); } while (0)

/*
 * Enables/disables CC2520 access to the SPI bus (not the bus).
 * (Chip Select)
 */

 /* ENABLE CSn (active low) */
#define CC2520_SPI_ENABLE()     do { SPI_FLUSH(); GPIO_PIN_REG(PORT(F), PIN(3)) = 0; } while (0)
 /* DISABLE CSn (active low) */
#define CC2520_SPI_DISABLE()    do { SPI_WAITFOREOTx(); GPIO_PIN_REG(PORT(F), PIN(3)) = PIN(3); } while(0)
#define CC2520_SPI_IS_ENABLED() GPIOPinRead(PORT(F), PIN(3))

#define NULLRDC_CONF_ACK_WAIT_TIME                RTIMER_SECOND / 500
#define NULLRDC_CONF_AFTER_ACK_DETECTED_WAIT_TIME RTIMER_SECOND / 250

#define NETSTACK_CONF_RADIO   cc2520_driver
#define NETSTACK_RADIO_MAX_PAYLOAD_LEN 125

signed char cc11xx_read_rssi(void);
void cc11xx_channel_set(uint8_t c);
#define MULTICHAN_CONF_SET_CHANNEL(x) cc11xx_channel_set(x)
#define MULTICHAN_CONF_READ_RSSI(x) cc11xx_read_rssi()

// #include "mist-conf-const.h"
// #define MIST_CONF_NETSTACK (MIST_CONF_NULLRDC | MIST_CONF_AES)

#undef QUEUEBUF_CONF_NUM
#define QUEUEBUF_CONF_NUM        8
#undef NBR_TABLE_CONF_MAX_NEIGHBORS
#define NBR_TABLE_CONF_MAX_NEIGHBORS     20
#undef UIP_CONF_DS6_ROUTE_NBU
#define UIP_CONF_DS6_ROUTE_NBU   20

#endif /* __PLATFORM_CONF_H__ */
