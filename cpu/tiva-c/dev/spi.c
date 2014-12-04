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
 * \addtogroup tiva-c-spi
 * @{
 *
 * \file
 * Implementation of the Tiva C SPI driver
 */
/*
 * This is SPI initialization code for the Tiva C architecture.
 *
 */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
 
#include "contiki.h"
#include "sys/energest.h"
#include "dev/tivaware/inc/hw_memmap.h"
#include "dev/tivaware/inc/hw_types.h"
#include "dev/tivaware/inc/hw_ints.h"
#include "dev/tivaware/inc/hw_ssi.h"
//#include "dev/tivaware/inc/hw_uart.h"
#include "dev/tivaware/driverlib/tm4c_gpio.h"
#include "dev/tivaware/driverlib/tm4c_interrupt.h"
#include "dev/tivaware/driverlib/tm4c_pin_map.h"
#include "dev/tivaware/driverlib/tm4c_sysctl.h"
//#include "dev/tivaware/driverlib/tm4c_uart.h"
#include "dev/tivaware/driverlib/tm4c_ssi.h"
//#include "dev/uart.h"
#include "dev/spi.h"



/* DK-TM4C129X
 * SSI0 peripheral
 * GPIO Port A peripheral (for SSI0 pins)
 * SSI0Clk - PA2
 * SSI0Rx  - PA4
 * SSI0Tx  - PA5
 * SSI0Fss - PJ3
 */

/* EK-TM4C123GXL
 * SSI2 peripheral
 * GPIO Port B peripheral (for SSI2 pins)
 * SSI2Clk - PB4
 * SSI2Rx  - PB6
 * SSI2Tx  - PB7
 * SSI2Fss - PE0//PB5 Conflict
 */

/* EK-TM4C1294XL
 * SSI2 peripheral
 * GPIO Port D peripheral (for SSI2 pins)
 * SSI2Clk - PD3
 * SSI2Rx  - PD0
 * SSI2Tx  - PD1
 * SSI2Fss - PH3//PD2 Conflict
 */

/*
 * Initialize SPI bus.
 */
void
spi_init(void)
{
#if defined(TARGET_IS_BLIZZARD_RA1) || defined(TARGET_IS_BLIZZARD_RA3) || \
    defined(TARGET_IS_BLIZZARD_RB0) || defined(TARGET_IS_BLIZZARD_RB1)
    uint32_t pui32DataRx;
    //
    // The SSI0 peripheral must be enabled for use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
    //
    // For this example SSI2 is used with PortB[7:4].  The actual port and pins
    // used may be different on your part, consult the data sheet for more
    // information.  GPIO port B needs to be enabled so these pins can be used.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    //
    // Configure the pin muxing for SSI0 functions on port A2, A3, A4, and A5.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PB4_SSI2CLK);
    //GPIOPinConfigure(GPIO_PB5_SSI2FSS);
    GPIOPinConfigure(GPIO_PB6_SSI2RX);
    GPIOPinConfigure(GPIO_PB7_SSI2TX);
    //
    // Configure the GPIO settings for the SSI pins.  This function also gives
    // control of these pins to the SSI hardware.  Consult the data sheet to
    // see which functions are allocated per pin.
    // The pins are assigned as follows:
    //      PB7 - SSI2Tx
    //      PB6 - SSI2Rx
    //      PB5 - SSI0Fss
    //      PB4 - SSI0CLK
    // TODO: change this to select the port/pin you are using.
    //
    //GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 |
      //             GPIO_PIN_4);
   GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_7 | GPIO_PIN_6 |GPIO_PIN_4);

    //
    // Configure and enable the SSI port for SPI master mode.  Use SSI2,
    // system clock supply, idle clock level low and active low clock in
    // freescale SPI mode, master mode, 1MHz SSI frequency, and 8-bit data.
    // For SPI mode, you can set the polarity of the SSI clock when the SSI
    // unit is idle.  You can also configure what clock edge you want to
    // capture data on.  Please reference the datasheet for more information on
    // the different SPI modes.
    //
    //Use the 80MHz Sys Clock SPI rate is 1MHz
    SSIConfigSetExpClk(SSI2_BASE, 80000000, SSI_FRF_MOTO_MODE_3,
                       SSI_MODE_MASTER, 1000000, 8);
    //
    // Enable the SSI2 module.
    //
    SSIEnable(SSI2_BASE);

    //
    // Read any residual data from the SSI port.  This makes sure the receive
    // FIFOs are empty, so we don't read any unwanted junk.  This is done here
    // because the SPI SSI mode is full-duplex, which allows you to send and
    // receive at the same time.  The SSIDataGetNonBlocking function returns
    // "true" when data was returned, and "false" when no data was returned.
    // The "non-blocking" function checks if there is any data in the receive
    // FIFO and does not "hang" if there isn't.
    //
    while(SSIDataGetNonBlocking(SSI2_BASE, &pui32DataRx))
    {
    }

#elif defined(PART_TM4C129XNCZAD)
 
    uint32_t pui32DataRx;

    //
    // The SSI0 peripheral must be enabled for use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);

    //
    // For this example SSI0 is used with PortA[5:2].  The actual port and pins
    // used may be different on your part, consult the data sheet for more
    // information.  GPIO port A needs to be enabled so these pins can be used.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    //
    // Configure the pin muxing for SSI0 functions on port A2, A3, A4, and A5.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA3_SSI0FSS);
    GPIOPinConfigure(GPIO_PA4_SSI0XDAT0);
    GPIOPinConfigure(GPIO_PA5_SSI0XDAT1);

    //
    // Configure the GPIO settings for the SSI pins.  This function also gives
    // control of these pins to the SSI hardware.  Consult the data sheet to
    // see which functions are allocated per pin.
    // The pins are assigned as follows:
    //      PA5 - SSI0Tx
    //      PA4 - SSI0Rx
    //      PA3 - SSI0Fss
    //      PA2 - SSI0CLK
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 |
                   GPIO_PIN_2); 
    //
    // Configure and enable the SSI port for SPI master mode.  Use SSI0,
    // system clock supply, idle clock level low and active low clock in
    // freescale SPI mode, master mode, 1MHz SSI frequency, and 8-bit data.
    // For SPI mode, you can set the polarity of the SSI clock when the SSI
    // unit is idle.  You can also configure what clock edge you want to
    // capture data on.  Please reference the datasheet for more information on
    // the different SPI modes.
    //
    //Use the 120MHz Sys Clock SPI rate is 1MHz
    SSIConfigSetExpClk(SSI0_BASE, 120000000, SSI_FRF_MOTO_MODE_3,
                       SSI_MODE_MASTER, 1000000, 8);

    //
    // Enable the SSI0 module.
    //
    SSIEnable(SSI0_BASE);

    //
    // Read any residual data from the SSI port.  This makes sure the receive
    // FIFOs are empty, so we don't read any unwanted junk.  This is done here
    // because the SPI SSI mode is full-duplex, which allows you to send and
    // receive at the same time.  The SSIDataGetNonBlocking function returns
    // "true" when data was returned, and "false" when no data was returned.
    // The "non-blocking" function checks if there is any data in the receive
    // FIFO and does not "hang" if there isn't.
    //
    while(SSIDataGetNonBlocking(SSI0_BASE, &pui32DataRx))
    {
    }

#elif defined(PART_TM4C1294NCPDT)
    uint32_t pui32DataRx;
    //
    // The SSI0 peripheral must be enabled for use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
    //
    // For this example SSI2 is used with PortB[7:4].  The actual port and pins
    // used may be different on your part, consult the data sheet for more
    // information.  GPIO port B needs to be enabled so these pins can be used.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    //
    // Configure the pin muxing for SSI0 functions on port A2, A3, A4, and A5.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PD3_SSI2CLK);
    //GPIOPinConfigure(GPIO_PB5_SSI2FSS);
    GPIOPinConfigure(GPIO_PD0_SSI2XDAT1);
    GPIOPinConfigure(GPIO_PD1_SSI2XDAT0);
    //
    // Configure the GPIO settings for the SSI pins.  This function also gives
    // control of these pins to the SSI hardware.  Consult the data sheet to
    // see which functions are allocated per pin.
    // The pins are assigned as follows:
    //      PD1 - SSI2Tx
    //      PD0 - SSI2Rx
    //      PH3 - SSI0Fss
    //      PD3 - SSI0CLK
    // TODO: change this to select the port/pin you are using.
    //
    //GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 |
      //             GPIO_PIN_4);
   GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 |GPIO_PIN_3);

    //
    // Configure and enable the SSI port for SPI master mode.  Use SSI2,
    // system clock supply, idle clock level low and active low clock in
    // freescale SPI mode, master mode, 1MHz SSI frequency, and 8-bit data.
    // For SPI mode, you can set the polarity of the SSI clock when the SSI
    // unit is idle.  You can also configure what clock edge you want to
    // capture data on.  Please reference the datasheet for more information on
    // the different SPI modes.
    //
    //Use the 80MHz Sys Clock SPI rate is 1MHz
    SSIConfigSetExpClk(SSI2_BASE, 120000000, SSI_FRF_MOTO_MODE_3,
                       SSI_MODE_MASTER, 1000000, 8);
    //
    // Enable the SSI2 module.
    //
    SSIEnable(SSI2_BASE);

    //
    // Read any residual data from the SSI port.  This makes sure the receive
    // FIFOs are empty, so we don't read any unwanted junk.  This is done here
    // because the SPI SSI mode is full-duplex, which allows you to send and
    // receive at the same time.  The SSIDataGetNonBlocking function returns
    // "true" when data was returned, and "false" when no data was returned.
    // The "non-blocking" function checks if there is any data in the receive
    // FIFO and does not "hang" if there isn't.
    //
    while(SSIDataGetNonBlocking(SSI2_BASE, &pui32DataRx))
    {
    }
#endif
}

/* Write one character to SPI */
void 
spi_write(unsigned char txData)
{
  uint32_t ui32sendData=0x00000000;
  uint32_t ui32dummyData;
  ui32sendData += txData;
  //
  // Wait until SSI0 is done transferring all the data in the transmit FIFO.
  //
  while(SSIBusy(SSI_BASE))
  {
  }
  //
  // Send the data using the "blocking" put function.  This function
  // will wait until there is room in the send FIFO before returning.
  // This allows you to assure that all the data you send makes it into
  // the send FIFO.
  //
  SSIDataPut(SSI_BASE, ui32sendData);
  //
  // Wait until SSI0 is done transferring all the data in the transmit FIFO.
  // Wait for TX ready
  while(SSIBusy(SSI_BASE))
  {
  }
  //
  // Read any residual data from the SSI port.  This makes sure the receive
  // FIFOs are empty, so we don't read any unwanted junk.  This is done here
  // because the SPI SSI mode is full-duplex, which allows you to send and
  // receive at the same time.  The SSIDataGetNonBlocking function returns
  // "true" when data was returned, and "false" when no data was returned.
  // The "non-blocking" function checks if there is any data in the receive
  // FIFO and does not "hang" if there isn't.
  //
  while(SSIDataGetNonBlocking(SSI_BASE, &ui32dummyData))
  {
  }
}
/* Write one character to SPI - will not wait for end useful for multiple writes with wait after final */
void 
spi_write_fast(unsigned char txData)
{
  uint32_t ui32sendData=0x00000000;
  uint32_t ui32dummyData;
  ui32sendData += txData;
  //
  // Wait until SSI0 is done transferring all the data in the transmit FIFO.
  //
  while(SSIBusy(SSI_BASE))
  {
  }
  //
  // Send the data using the "blocking" put function.  This function
  // will wait until there is room in the send FIFO before returning.
  // This allows you to assure that all the data you send makes it into
  // the send FIFO.
  //
  SSIDataPut(SSI_BASE, ui32sendData);
  //
  // Read any residual data from the SSI port.  This makes sure the receive
  // FIFOs are empty, so we don't read any unwanted junk.  This is done here
  // because the SPI SSI mode is full-duplex, which allows you to send and
  // receive at the same time.  The SSIDataGetNonBlocking function returns
  // "true" when data was returned, and "false" when no data was returned.
  // The "non-blocking" function checks if there is any data in the receive
  // FIFO and does not "hang" if there isn't.
  //
  while(SSIDataGetNonBlocking(SSI_BASE, &ui32dummyData))
  {
  }
}

/* Read one character from SPI */
void 
spi_read(unsigned char *rxData)
{
  uint32_t ui32sendData=0x00000000;
  uint32_t ui32dummyData;
  // Wait until SSI0 is done transferring all the data in the transmit FIFO.
  //
  while(SSIBusy(SSI_BASE))
  {
  }
  //
  // Send the data using the "blocking" put function.  This function
  // will wait until there is room in the send FIFO before returning.
  // This allows you to assure that all the data you send makes it into
  // the send FIFO.
  //
  SSIDataPut(SSI_BASE, ui32sendData);
  while(!((HWREG(SSI_BASE + SSI_O_SR) & SSI_SR_TFE) && ((HWREG(SSI_BASE + SSI_O_SR) & SSI_SR_RNE))));
  //
  // Wait until SSI0 is done transferring all the data in the transmit FIFO.
  // Wait for TX ready
  while(SSIBusy(SSI_BASE))
  {
  }
  //
  // Receive the data using the "blocking" Get function. This function
  // will wait until there is data in the receive FIFO before returning.
  //
  SSIDataGet(SSI_BASE, &ui32sendData);//Need to check with Jonas??????????
  while(SSIBusy(SSI_BASE))
  {
  }

  while(SSIDataGetNonBlocking(SSI_BASE, &ui32dummyData))
  {
  }
  // Since we are using 8-bit data, mask off the MSB.
  //
  ui32sendData &= 0x00FF;
  // transfer to unsigned char
  *rxData = (unsigned char)ui32sendData;
  //printf("spi_read: %d\n",*rxData);
}

/* Flush the SPI read register */
void 
spi_flush(void)
{
  uint32_t ui32rxDummyData;
  //
  // Read any residual data from the SSI port.  This makes sure the receive
  // FIFOs are empty, so we don't read any unwanted junk.  This is done here
  // because the SPI SSI mode is full-duplex, which allows you to send and
  // receive at the same time.  The SSIDataGetNonBlocking function returns
  // "true" when data was returned, and "false" when no data was returned.
  // The "non-blocking" function checks if there is any data in the receive
  // FIFO and does not "hang" if there isn't.
  //
  while(SSIDataGetNonBlocking(SSI_BASE, &ui32rxDummyData))
  {
  }
}

void
spi_Tx_test(uint32_t length, uint32_t *Txbuf)
{
   uint32_t ui32Index;
   for(ui32Index = 0; ui32Index < length; ui32Index++)
   {
      //
      // Send the data using the "blocking" put function.  This function
      // will wait until there is room in the send FIFO before returning.
      // This allows you to assure that all the data you send makes it into
      // the send FIFO.
      //
      SSIDataPut(SSI_BASE, *(Txbuf+ui32Index));
   }
}

void
spi_Tx_TestDebug(void)
{
   uint32_t ui32Index=0x00000018; 
   SSIDataPut(SSI_BASE, ui32Index);
}

uint8_t
spi_Rx_test(void)
{
   uint32_t rxData;
   uint8_t rxDataByte;
   //for(ui32Index = 0; ui32Index < NUM_SSI_DATA; ui32Index++)
   //{
      //
      // Receive the data using the "blocking" Get function. This function
      // will wait until there is data in the receive FIFO before returning.
      //
      SSIDataGet(SSI_BASE, &rxData);

      //
      // Since we are using 8-bit data, mask off the MSB.
      //
      rxData &= 0x00FF;
   //}
    rxDataByte = rxData;
    return rxDataByte;
}
/** @} */
