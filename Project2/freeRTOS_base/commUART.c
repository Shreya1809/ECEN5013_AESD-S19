/*
 * uart3.c
 *
 *  Created on: Apr 29, 2019
 *      Author: Shreya
 */

#include "includes.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "driverlib/debug.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/rom_map.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/interrupt.h"
#include "FreeRTOS.h"
#include "semphr.h"

SemaphoreHandle_t   recvUARTBytesSem;


#ifndef UART3_RX_BUFFER_SIZE
#define UART3_RX_BUFFER_SIZE     1024
#endif
#ifndef UART3_TX_BUFFER_SIZE
#define UART3_TX_BUFFER_SIZE     1024
#endif

extern uint32_t g_ui32SysClock;

//static uint32_t g_ui32PortNum = 3;
static uint32_t g_ui32Base = UART3_BASE;

//static const uint32_t g_ui32UARTInt[4] =
//{
//    INT_UART0, INT_UART1, INT_UART2, INT_UART3
//};

static bool g_bDisableEcho = true;

//*****************************************************************************
//
// Output ring buffer.  Buffer is full if g_ui32UARTTxReadIndex is one ahead of
// g_ui32UARTTxWriteIndex.  Buffer is empty if the two indices are the same.
//
//*****************************************************************************
static unsigned char g_pcUARTTxBuffer[UART3_TX_BUFFER_SIZE];
static volatile uint32_t g_ui32UARTTxWriteIndex = 0;
static volatile uint32_t g_ui32UARTTxReadIndex = 0;

//*****************************************************************************
//
// Input ring buffer.  Buffer is full if g_ui32UARTTxReadIndex is one ahead of
// g_ui32UARTTxWriteIndex.  Buffer is empty if the two indices are the same.
//
//*****************************************************************************
static unsigned char g_pcUARTRxBuffer[UART3_RX_BUFFER_SIZE];
static volatile uint32_t g_ui32UARTRxWriteIndex = 0;
static volatile uint32_t g_ui32UARTRxReadIndex = 0;

//*****************************************************************************
//
// Macros to determine number of free and used bytes in the transmit buffer.
//
//*****************************************************************************
#define TX_BUFFER_USED          (GetBufferCount(&g_ui32UARTTxReadIndex,  \
                                                &g_ui32UARTTxWriteIndex, \
                                                UART_TX_BUFFER_SIZE))
#define TX_BUFFER_FREE          (UART_TX_BUFFER_SIZE - TX_BUFFER_USED)
#define TX_BUFFER_EMPTY         (IsBufferEmpty(&g_ui32UARTTxReadIndex,   \
                                               &g_ui32UARTTxWriteIndex))
#define TX_BUFFER_FULL          (IsBufferFull(&g_ui32UARTTxReadIndex,  \
                                              &g_ui32UARTTxWriteIndex, \
                                              UART_TX_BUFFER_SIZE))
#define ADVANCE_TX_BUFFER_INDEX(Index) \
                                (Index) = ((Index) + 1) % UART_TX_BUFFER_SIZE

//*****************************************************************************
//
// Macros to determine number of free and used bytes in the receive buffer.
//
//*****************************************************************************
#define RX_BUFFER_USED          (GetBufferCount(&g_ui32UARTRxReadIndex,  \
                                                &g_ui32UARTRxWriteIndex, \
                                                UART_RX_BUFFER_SIZE))
#define RX_BUFFER_FREE          (UART_RX_BUFFER_SIZE - RX_BUFFER_USED)
#define RX_BUFFER_EMPTY         (IsBufferEmpty(&g_ui32UARTRxReadIndex,   \
                                               &g_ui32UARTRxWriteIndex))
#define RX_BUFFER_FULL          (IsBufferFull(&g_ui32UARTRxReadIndex,  \
                                              &g_ui32UARTRxWriteIndex, \
                                              UART_RX_BUFFER_SIZE))
#define ADVANCE_RX_BUFFER_INDEX(Index) \
                                (Index) = ((Index) + 1) % UART_RX_BUFFER_SIZE


static bool
IsBufferFull(volatile uint32_t *pui32Read,
             volatile uint32_t *pui32Write, uint32_t ui32Size)
{
    uint32_t ui32Write;
    uint32_t ui32Read;

    ui32Write = *pui32Write;
    ui32Read = *pui32Read;

    return((((ui32Write + 1) % ui32Size) == ui32Read) ? true : false);
}


static bool
IsBufferEmpty(volatile uint32_t *pui32Read,
              volatile uint32_t *pui32Write)
{
    uint32_t ui32Write;
    uint32_t ui32Read;

    ui32Write = *pui32Write;
    ui32Read = *pui32Read;

    return((ui32Write == ui32Read) ? true : false);
}

static uint32_t
GetBufferCount(volatile uint32_t *pui32Read,
               volatile uint32_t *pui32Write, uint32_t ui32Size)
{
    uint32_t ui32Write;
    uint32_t ui32Read;

    ui32Write = *pui32Write;
    ui32Read = *pui32Read;

    return((ui32Write >= ui32Read) ? (ui32Write - ui32Read) :
           (ui32Size - (ui32Read - ui32Write)));
}

static void
UART2PrimeTransmit(uint32_t ui32Base)
{
    //
    // Do we have any data to transmit?
    //
    if(!TX_BUFFER_EMPTY)
    {
        //
        // Disable the UART interrupt.  If we don't do this there is a race
        // condition which can cause the read index to be corrupted.
        //
        IntDisable(INT_UART3);

        //
        // Yes - take some characters out of the transmit buffer and feed
        // them to the UART transmit FIFO.
        //
        while(MAP_UARTSpaceAvail(ui32Base) && !TX_BUFFER_EMPTY)
        {
            MAP_UARTCharPutNonBlocking(ui32Base,
                                      g_pcUARTTxBuffer[g_ui32UARTTxReadIndex]);
            ADVANCE_TX_BUFFER_INDEX(g_ui32UARTTxReadIndex);
        }

        //
        // Reenable the UART interrupt.
        //
        MAP_IntEnable(INT_UART3);
    }
}

//*****************************************************************************
//
//! Writes a string of characters to the UART output.
//!
//! \param pcBuf points to a buffer containing the string to transmit.
//! \param ui32Len is the length of the string to transmit.
//!
//! This function will transmit the string to the UART output.  The number of
//! characters transmitted is determined by the \e ui32Len parameter.  This
//! function does no interpretation or translation of any characters.  Since
//! the output is sent to a UART, any LF (/n) characters encountered will be
//! replaced with a CRLF pair.
//!
//! Besides using the \e ui32Len parameter to stop transmitting the string, if
//! a null character (0) is encountered, then no more characters will be
//! transmitted and the function will return.
//!
//! In non-buffered mode, this function is blocking and will not return until
//! all the characters have been written to the output FIFO.  In buffered mode,
//! the characters are written to the UART transmit buffer and the call returns
//! immediately.  If insufficient space remains in the transmit buffer,
//! additional characters are discarded.
//!
//! \return Returns the count of characters written.
//
//*****************************************************************************
static int
UART2Write(const char *pcBuf, uint32_t ui32Len)
{
    unsigned int uIdx;

    //
    // Check for valid arguments.
    //
    ASSERT(pcBuf != 0);
    ASSERT(g_ui32Base != 0);

    //
    // Send the characters
    //
    for(uIdx = 0; uIdx < ui32Len; uIdx++)
    {
        //
        // Send the character to the UART output.
        //
        if(!TX_BUFFER_FULL)
        {
            g_pcUARTTxBuffer[g_ui32UARTTxWriteIndex] = pcBuf[uIdx];
            ADVANCE_TX_BUFFER_INDEX(g_ui32UARTTxWriteIndex);
        }
        else
        {
            //
            // Buffer is full - discard remaining characters and return.
            //
            break;
        }
    }

    //
    // If we have anything in the buffer, make sure that the UART is set
    // up to transmit it.
    //
    if(!TX_BUFFER_EMPTY)
    {
        UART2PrimeTransmit(g_ui32Base);
        MAP_UARTIntEnable(g_ui32Base, UART_INT_TX);
    }

    //
    // Return the number of characters written.
    //
    return(uIdx);
}

//*****************************************************************************
//
//! A simple UART based get string function, with some line processing.
//!
//! \param pcBuf points to a buffer for the incoming string from the UART.
//! \param ui32Len is the length of the buffer for storage of the string,
//! including the trailing 0.
//!
//! This function will receive a string from the UART input and store the
//! characters in the buffer pointed to by \e pcBuf.  The characters will
//! continue to be stored until a termination character is received.  The
//! termination characters are CR, LF, or ESC.  A CRLF pair is treated as a
//! single termination character.  The termination characters are not stored in
//! the string.  The string will be terminated with a 0 and the function will
//! return.
//!
//! In both buffered and unbuffered modes, this function will block until
//! a termination character is received.  If non-blocking operation is required
//! in buffered mode, a call to UARTPeek() may be made to determine whether
//! a termination character already exists in the receive buffer prior to
//! calling UART2gets().
//!
//! Since the string will be null terminated, the user must ensure that the
//! buffer is sized to allow for the additional null character.
//!
//! \return Returns the count of characters that were stored, not including
//! the trailing 0.
//
//*****************************************************************************
static int
UART2gets(char *pcBuf, uint32_t ui32Len)
{
    uint32_t ui32Count = 0;
    int8_t cChar;

    //
    // Check the arguments.
    //
    ASSERT(pcBuf != 0);
    ASSERT(ui32Len != 0);
    ASSERT(g_ui32Base != 0);

    //
    // Adjust the length back by 1 to leave space for the trailing
    // null terminator.
    //
    ui32Len--;

    //
    // Process characters until a newline is received.
    //
    while(1)
    {
        //
        // Read the next character from the receive buffer.
        //
        if(!RX_BUFFER_EMPTY)
        {
            cChar = g_pcUARTRxBuffer[g_ui32UARTRxReadIndex];
            ADVANCE_RX_BUFFER_INDEX(g_ui32UARTRxReadIndex);

            //
            // See if a newline or escape character was received.
            //
            if((cChar == '\r') || (cChar == '\n') || (cChar == 0x1b))
            {
                //
                // Stop processing the input and end the line.
                //
                break;
            }

            //
            // Process the received character as long as we are not at the end
            // of the buffer.  If the end of the buffer has been reached then
            // all additional characters are ignored until a newline is
            // received.
            //
            if(ui32Count < ui32Len)
            {
                //
                // Store the character in the caller supplied buffer.
                //
                pcBuf[ui32Count] = cChar;

                //
                // Increment the count of characters received.
                //
                ui32Count++;
            }
        }
    }

    //
    // Add a null termination to the string.
    //
    pcBuf[ui32Count] = 0;

    //
    // Return the count of int8_ts in the buffer, not counting the trailing 0.
    //
    return(ui32Count);
}

//*****************************************************************************
//
//! Read a single character from the UART, blocking if necessary.
//!
//! This function will receive a single character from the UART and store it at
//! the supplied address.
//!
//! In both buffered and unbuffered modes, this function will block until a
//! character is received.  If non-blocking operation is required in buffered
//! mode, a call to UARTRxAvail() may be made to determine whether any
//! characters are currently available for reading.
//!
//! \return Returns the character read.
//
//*****************************************************************************
static unsigned char
UART2getc(void)
{
    unsigned char cChar;

    //
    // Wait for a character to be received.
    //
    while(RX_BUFFER_EMPTY)
    {
        xSemaphoreTake(recvUARTBytesSem, portMAX_DELAY);
        //
        // Block waiting for a character to be received (if the buffer is
        // currently empty).
        //
    }

    //
    // Read a character from the buffer.
    //
    cChar = g_pcUARTRxBuffer[g_ui32UARTRxReadIndex];
    ADVANCE_RX_BUFFER_INDEX(g_ui32UARTRxReadIndex);

    //
    // Return the character to the caller.
    //
    return(cChar);
}

static int
UART2RxBytesAvail(void)
{
    return(RX_BUFFER_USED);
}

static int
UART2TxBytesFree(void)
{
    return(TX_BUFFER_FREE);
}

static int
UART2Peek(unsigned char ucChar)
{
    int iCount;
    int iAvail;
    uint32_t ui32ReadIndex;

    //
    // How many characters are there in the receive buffer?
    //
    iAvail = (int)RX_BUFFER_USED;
    ui32ReadIndex = g_ui32UARTRxReadIndex;

    //
    // Check all the unread characters looking for the one passed.
    //
    for(iCount = 0; iCount < iAvail; iCount++)
    {
        if(g_pcUARTRxBuffer[ui32ReadIndex] == ucChar)
        {
            //
            // We found it so return the index
            //
            return(iCount);
        }
        else
        {
            //
            // This one didn't match so move on to the next character.
            //
            ADVANCE_RX_BUFFER_INDEX(ui32ReadIndex);
        }
    }

    //
    // If we drop out of the loop, we didn't find the character in the receive
    // buffer.
    //
    return(-1);
}

static void
UART2FlushRx(void)
{
    uint32_t ui32Int;

    //
    // Temporarily turn off interrupts.
    //
    ui32Int = MAP_IntMasterDisable();

    //
    // Flush the receive buffer.
    //
    g_ui32UARTRxReadIndex = 0;
    g_ui32UARTRxWriteIndex = 0;

    //
    // If interrupts were enabled when we turned them off, turn them
    // back on again.
    //
    if(!ui32Int)
    {
        MAP_IntMasterEnable();
    }
}

static void
UART2FlushTx(bool bDiscard)
{
    uint32_t ui32Int;

    //
    // Should the remaining data be discarded or transmitted?
    //
    if(bDiscard)
    {
        //
        // The remaining data should be discarded, so temporarily turn off
        // interrupts.
        //
        ui32Int = MAP_IntMasterDisable();

        //
        // Flush the transmit buffer.
        //
        g_ui32UARTTxReadIndex = 0;
        g_ui32UARTTxWriteIndex = 0;

        //
        // If interrupts were enabled when we turned them off, turn them
        // back on again.
        //
        if(!ui32Int)
        {
            MAP_IntMasterEnable();
        }
    }
    else
    {
        //
        // Wait for all remaining data to be transmitted before returning.
        //
        while(!TX_BUFFER_EMPTY)
        {
        }
    }
}

void
UART2IntHandler(void)
{
    uint32_t ui32Ints;
    int8_t cChar;
    int32_t i32Char;
    static bool bLastWasCR = false;

    //
    // Get and clear the current interrupt source(s)
    //
    ui32Ints = MAP_UARTIntStatus(g_ui32Base, true);
    MAP_UARTIntClear(g_ui32Base, ui32Ints);

    //
    // Are we being interrupted because the TX FIFO has space available?
    //
    if(ui32Ints & UART_INT_TX)
    {
        //
        // Move as many bytes as we can into the transmit FIFO.
        //
        UART2PrimeTransmit(g_ui32Base);

        //
        // If the output buffer is empty, turn off the transmit interrupt.
        //
        if(TX_BUFFER_EMPTY)
        {
            MAP_UARTIntDisable(g_ui32Base, UART_INT_TX);
        }
    }

    //
    // Are we being interrupted due to a received character?
    //
    if(ui32Ints & (UART_INT_RX | UART_INT_RT))
    {
        //
        // Get all the available characters from the UART.
        //
        while(MAP_UARTCharsAvail(g_ui32Base))
        {
            //
            // Read a character
            //
            i32Char = MAP_UARTCharGetNonBlocking(g_ui32Base);
            //
            // If there is space in the receive buffer, put the character
            // there, otherwise throw it away.
            //
            if(!RX_BUFFER_FULL)
            {
                //
                // Store the new character in the receive buffer
                //
                g_pcUARTRxBuffer[g_ui32UARTRxWriteIndex] =
                    (unsigned char)(i32Char & 0xFF);
                ADVANCE_RX_BUFFER_INDEX(g_ui32UARTRxWriteIndex);
                static BaseType_t xHigherPriorityTaskWoken;
                xSemaphoreGiveFromISR(recvUARTBytesSem, &xHigherPriorityTaskWoken );
            }
        }

        //
        // If we wrote anything to the transmit buffer, make sure it actually
        // gets transmitted.
        //
//        UART2PrimeTransmit(g_ui32Base);
//        MAP_UARTIntEnable(g_ui32Base, UART_INT_TX);
    }
}


void UART2_config(uint32_t baudRate)
{
    //
    // Enable GPIO port A which is used for UART0 pins.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Configure the pin muxing for UART0 functions on port A0 and A1.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PA4_U3RX);
    GPIOPinConfigure(GPIO_PA5_U3TX);

    //
    // Enable UART0 so that we can configure the clock.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Select the alternate (UART) function for these pins.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    //
    // Initialize the UART for console I/O.
    //
//    UARTStdioConfig(2, baudRate, g_ui32SysClock);


        //
        // Check to make sure the UART peripheral is present.
        //
        if(!MAP_SysCtlPeripheralPresent(SYSCTL_PERIPH_UART3))
        {
            return;
        }

        //
        // Select the base address of the UART.
        //
        g_ui32Base = UART3_BASE;

        //
        // Enable the UART peripheral for use.
        //
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);

        //
        // Configure the UART for 115200, n, 8, 1
        //
        MAP_UARTConfigSetExpClk(g_ui32Base, g_ui32SysClock, baudRate,
                                (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE |
                                 UART_CONFIG_WLEN_8));

    #ifdef UART_BUFFERED
        //
        // Set the UART to interrupt whenever the TX FIFO is almost empty or
        // when any character is received.
        //
        MAP_UARTFIFOLevelSet(g_ui32Base, UART_FIFO_TX1_8, UART_FIFO_RX1_8);

        //
        // Flush both the buffers.
        //
        UARTFlushRx();
        UARTFlushTx(true);

        //
        // Remember which interrupt we are dealing with.
        //
//        if(g_ui32PortNum == 0)
//            g_ui32PortNum = ui32PortNum;

        recvUARTBytesSem = xSemaphoreCreateCounting(UART3_RX_BUFFER_SIZE, 0);

        //
        // We are configured for buffered output so enable the master interrupt
        // for this UART and the receive interrupts.  We don't actually enable the
        // transmit interrupt in the UART itself until some data has been placed
        // in the transmit buffer.
        //
        MAP_UARTIntDisable(g_ui32Base, 0xFFFFFFFF);
        MAP_UARTIntEnable(g_ui32Base, UART_INT_RX | UART_INT_RT);
        MAP_IntEnable(INT_UART3);
    #endif

        //
        // Enable the UART operation.
        //
        MAP_UARTEnable(g_ui32Base);
}

void UART3_config(uint32_t baudrate)
{

    // Enable the GPIO Peripheral used by the UART.
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // Enable UART0
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_UART3))
    {
    }

    // Configure GPIO Pins for UART mode.
    MAP_GPIOPinConfigure(GPIO_PA4_U3RX);
    MAP_GPIOPinConfigure(GPIO_PA5_U3TX);
    MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    UARTConfigSetExpClk(UART3_BASE, g_ui32SysClock, baudrate,
                                (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE |
                                 UART_CONFIG_WLEN_8));
    UARTEnable(UART3_BASE);
}

int UART2_putData(char *data, size_t len)
{
    return UART2Write(data,len);
}

int UART2_getData(char *data, size_t len)
{
    int i = 0;
    while(i<len)
    {
        data[i] = UART2getc();
        i++;
    }
    return i;
}

size_t UART3_putData(char *data, size_t len)
{
    while(UARTBusy(UART3_BASE));
    size_t i = 0;
    while(len--)
    {
        UARTCharPut(UART3_BASE, *data++);
        i++;
    }
    return i;
}


size_t UART3_getData(uint8_t *data, size_t len)
{
    if(!UARTCharsAvail(UART3_BASE))
        return 0;
    int i = 0;
    while(i<len)
    {
        int32_t c = UARTCharGet(UART3_BASE);
        if(c != -1)
        {
            data[i] = c;
            i++;
        }
        else return i;
    }
    return i;
}

size_t UART3_putString(const char *str)
{
    size_t i = 0;
    while(*str)
    {
//        if(*str == '\n')
//            UARTCharPut(UART3_BASE, '\r');
        UARTCharPut(UART3_BASE, *str++);
        i++;
    }
    return i;
}

