/*
 * ethernet.c
 *
 *  Created on: Apr 24, 2019
 *      Author: Shreya
 *      Reference : enet_uip.c - Sample WebServer Application for Ethernet Demo, dk-tm4c129x
 */

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_emac.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/emac.h"
#include "driverlib/flash.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "utils/uartstdio.h"
#include "logger_task.h"
#include "ethernet.h"
#include "FreeRTOS.h"
#include "task.h"
#include "NetworkInterface.h"
#include "logger.h"

//*****************************************************************************
//
// Ethernet DMA descriptors.
//
// Although uIP uses a single buffer, the MAC hardware needs a minimum of
// 3 receive descriptors to operate.
//
//*****************************************************************************
#define NUM_TX_DESCRIPTORS 3
#define NUM_RX_DESCRIPTORS 3
tEMACDMADescriptor g_psRxDescriptor[NUM_TX_DESCRIPTORS];
tEMACDMADescriptor g_psTxDescriptor[NUM_RX_DESCRIPTORS];

uint32_t g_ui32RxDescIndex;
uint32_t g_ui32TxDescIndex;

//*****************************************************************************
//
// Transmit and receive buffers.
//
//*****************************************************************************
#define RX_BUFFER_SIZE 1536
#define TX_BUFFER_SIZE 1536
uint8_t g_pui8RxBuffer[RX_BUFFER_SIZE];
uint8_t g_pui8TxBuffer[TX_BUFFER_SIZE];

//*****************************************************************************
//
// A system tick counter, incremented every SYSTICKMS.
//
//*****************************************************************************
volatile uint32_t g_ui32TickCounter = 0;
extern uint32_t g_ui32SysClock;
extern TaskHandle_t xEthernetTaskHandle;

//const uint8_t pui8MACArray[6] ={0x00, 0xbc, 0xde, 0xef, 0x12, 0x4a};
const uint8_t pui8MACArray[6] ={0x00, 0x1a, 0xb6, 0x03, 0x2b, 0xde};

//*****************************************************************************
//
// Initialize the transmit and receive DMA descriptors.  We apparently need
// a minimum of 3 descriptors in each chain.  This is overkill since uIP uses
// a single, common transmit and receive buffer so we tag each descriptor
// with the same buffer and will make sure we only hand the DMA one descriptor
// at a time.
//
//*****************************************************************************
void InitDescriptors(uint32_t ui32Base)
{
    uint32_t ui32Loop;

    //
    // Initialize each of the transmit descriptors.  Note that we leave the OWN
    // bit clear here since we have not set up any transmissions yet.
    //
    for(ui32Loop = 0; ui32Loop < NUM_TX_DESCRIPTORS; ui32Loop++)
    {
        g_psTxDescriptor[ui32Loop].ui32Count =
                (DES1_TX_CTRL_SADDR_INSERT |
                        (TX_BUFFER_SIZE << DES1_TX_CTRL_BUFF1_SIZE_S));
        g_psTxDescriptor[ui32Loop].pvBuffer1 = g_pui8TxBuffer;
        g_psTxDescriptor[ui32Loop].DES3.pLink =
                (ui32Loop == (NUM_TX_DESCRIPTORS - 1)) ?
                        g_psTxDescriptor : &g_psTxDescriptor[ui32Loop + 1];
        g_psTxDescriptor[ui32Loop].ui32CtrlStatus =
                (DES0_TX_CTRL_LAST_SEG | DES0_TX_CTRL_FIRST_SEG |
                        DES0_TX_CTRL_INTERRUPT | DES0_TX_CTRL_CHAINED |
                        DES0_TX_CTRL_IP_ALL_CKHSUMS);
    }

    //
    // Initialize each of the receive descriptors.  We clear the OWN bit here
    // to make sure that the receiver doesn't start writing anything
    // immediately.
    //
    for(ui32Loop = 0; ui32Loop < NUM_RX_DESCRIPTORS; ui32Loop++)
    {
        g_psRxDescriptor[ui32Loop].ui32CtrlStatus = 0;
        g_psRxDescriptor[ui32Loop].ui32Count =
                (DES1_RX_CTRL_CHAINED |
                        (RX_BUFFER_SIZE << DES1_RX_CTRL_BUFF1_SIZE_S));
        g_psRxDescriptor[ui32Loop].pvBuffer1 = g_pui8RxBuffer;
        g_psRxDescriptor[ui32Loop].DES3.pLink =
                (ui32Loop == (NUM_RX_DESCRIPTORS - 1)) ?
                        g_psRxDescriptor : &g_psRxDescriptor[ui32Loop + 1];
    }

    //
    // Set the descriptor pointers in the hardware.
    //
    EMACRxDMADescriptorListSet(ui32Base, g_psRxDescriptor);
    EMACTxDMADescriptorListSet(ui32Base, g_psTxDescriptor);

    //
    // Start from the beginning of both descriptor chains.  We actually set
    // the transmit descriptor index to the last descriptor in the chain
    // since it will be incremented before use and this means the first
    // transmission we perform will use the correct descriptor.
    //
    g_ui32RxDescIndex = 0;
    g_ui32TxDescIndex = NUM_TX_DESCRIPTORS - 1;

    //
    // Mark the receive descriptors as available to the DMA to start
    // the receive processing.
    //
    for(ui32Loop = 0; ui32Loop < NUM_RX_DESCRIPTORS; ui32Loop++)
    {
        g_psRxDescriptor[ui32Loop].ui32CtrlStatus |= DES0_RX_CTRL_OWN;
    }
}


void Ethernet_Init(void)
{
    uint8_t ui8PHYAddr;
    uint32_t ui32PHYConfig;

    LOG_DEBUG(MAIN_TASK,NULL,"Using Internal PHY.");
    ui32PHYConfig = (EMAC_PHY_TYPE_INTERNAL | EMAC_PHY_INT_MDIX_EN |
                     EMAC_PHY_AN_100B_T_FULL_DUPLEX);
    ui8PHYAddr = 0;

    //
    // Enable and reset the Ethernet modules.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EMAC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EPHY0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_EMAC0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_EPHY0);

    //Configures pin(s) for use by the Ethernet peripheral as LED signals. check!!!!
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinConfigure(GPIO_PF0_EN0LED0);
    GPIOPinConfigure(GPIO_PF4_EN0LED1);
    GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_4);
    GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_0);

    //
    // Wait for the MAC to be ready.
    //
    LOG_DEBUG(MAIN_TASK,NULL, "Waiting for MAC to be ready...");
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_EMAC0))
    {
    }

    //
    // Configure for use with the internal PHY.
    //
    EMACPHYConfigSet(EMAC0_BASE, ui32PHYConfig);
    LOG_DEBUG(MAIN_TASK,NULL, "MAC ready.");

    //
    // Reset the MAC.
    //
    EMACReset(EMAC0_BASE);

    //
    // Initialize the MAC and set the DMA mode.
    //
    EMACInit(EMAC0_BASE, g_ui32SysClock,
                 EMAC_BCONFIG_MIXED_BURST | EMAC_BCONFIG_PRIORITY_FIXED, 4, 4,
                 0);

    //using internal phy setup configurations example line 788
    EMACConfigSet(EMAC0_BASE,
                          (EMAC_CONFIG_FULL_DUPLEX | EMAC_CONFIG_CHECKSUM_OFFLOAD |
                           EMAC_CONFIG_7BYTE_PREAMBLE | EMAC_CONFIG_IF_GAP_96BITS |
                           EMAC_CONFIG_USE_MACADDR0 |
                           EMAC_CONFIG_SA_FROM_DESCRIPTOR |
                           EMAC_CONFIG_BO_LIMIT_1024),
                          (EMAC_MODE_RX_STORE_FORWARD |
                           EMAC_MODE_TX_STORE_FORWARD |
                           EMAC_MODE_TX_THRESHOLD_64_BYTES |
                           EMAC_MODE_RX_THRESHOLD_64_BYTES), 0);

    //
       // Initialize the Ethernet DMA descriptors.
       //
       InitDescriptors(EMAC0_BASE);

       //
       // Program the hardware with its MAC address (for filtering).
       //
       EMACAddrSet(EMAC0_BASE, 0, pui8MACArray);

       //
       // Wait for the link to become active.  If we are using an external
       // RMII PHY, we already waited for link to be acquired prior to
       // configuring the MAC so don't wait again here.
       //
       LOG_DEBUG(MAIN_TASK,NULL, "Waiting for Link");
       while((EMACPHYRead(EMAC0_BASE, ui8PHYAddr, EPHY_BMSR) &
              EPHY_BMSR_LINKSTAT) == 0)
       {
       }
       LOG_DEBUG(MAIN_TASK,NULL, "Link Established");

       //
       // Set MAC filtering options.  We receive all broadcast and multicast
       // packets along with those addressed specifically for us.
       //
       EMACFrameFilterSet(EMAC0_BASE, (EMAC_FRMFILTER_SADDR |
               EMAC_FRMFILTER_PASS_MULTICAST |
               EMAC_FRMFILTER_PASS_NO_CTRL));

       //
       // Clear any pending interrupts.
       //
       EMACIntClear(EMAC0_BASE, EMACIntStatus(EMAC0_BASE, false));

       //
       // Enable the Ethernet MAC transmitter and receiver.
       //
       EMACTxEnable(EMAC0_BASE);
       EMACRxEnable(EMAC0_BASE);

       IntPrioritySet(INT_EMAC0, 0xE0);

       //
       // Enable the Ethernet interrupt.
       //
       IntEnable(INT_EMAC0);

       //
       // Enable the Ethernet RX Packet interrupt source.
       //
       EMACIntEnable(EMAC0_BASE, EMAC_INT_RECEIVE);


       //
       // Mark the first receive descriptor as available to the DMA to start
       // the receive processing.
       //
       //g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus |= DES0_RX_CTRL_OWN;


}

//*****************************************************************************
//
// The interrupt handler for the Ethernet interrupt.
//
//*****************************************************************************
void EthernetIntHandler(void)
{
    uint32_t ui32Temp;

    //
    // Read and Clear the interrupt.
    //
    ui32Temp = EMACIntStatus(EMAC0_BASE, true);
    EMACIntClear(EMAC0_BASE, ui32Temp);

    //
    // Check to see if an RX Interrupt has occurred.
    //
    if(ui32Temp & EMAC_INT_RECEIVE)
    {
        //
        // Indicate that a packet has been received.
        //
        BaseType_t xHighPriorityTask = pdFALSE;
        vTaskNotifyGiveFromISR(xEthernetTaskHandle, &xHighPriorityTask);
        portYIELD_FROM_ISR(xHighPriorityTask);
    }

    //
    // Has the DMA finished transferring a packet to the transmitter?
    //
    if(ui32Temp & EMAC_INT_TRANSMIT)
    {
        //
        // Indicate that a packet has been sent.
        //
    }
}

//*****************************************************************************
//
// Transmit a packet from the supplied buffer.
//
//*****************************************************************************
int32_t Ethernet_PacketTransmit(uint32_t ui32Base, uint8_t *pui8Buf, int32_t i32BufLen)
{
    //int_fast32_t i32Loop;

    //
    // Wait for the previous packet to be transmitted.
    //
    while(g_psTxDescriptor[g_ui32TxDescIndex].ui32CtrlStatus &
          DES0_TX_CTRL_OWN)
    {
        //
        // Spin and waste time.
        //
    }

    //
    // Move to the next descriptor.
    //
    g_ui32TxDescIndex++;
    if(g_ui32TxDescIndex == NUM_TX_DESCRIPTORS)
    {
        g_ui32TxDescIndex = 0;
    }

    //
    // Fill in the packet size and tell the transmitter to start work.
    //
    g_psTxDescriptor[g_ui32TxDescIndex].ui32Count = (uint32_t)i32BufLen;
    g_psTxDescriptor[g_ui32TxDescIndex].pvBuffer1 = pui8Buf;
    g_psTxDescriptor[g_ui32TxDescIndex].ui32CtrlStatus =
        (DES0_TX_CTRL_LAST_SEG | DES0_TX_CTRL_FIRST_SEG |
         DES0_TX_CTRL_INTERRUPT | DES0_TX_CTRL_IP_ALL_CKHSUMS |
         DES0_TX_CTRL_CHAINED | DES0_TX_CTRL_OWN);

    //
    // Tell the DMA to re acquire the descriptor now that we've filled it in.
    //
    EMACTxDMAPollDemand(EMAC0_BASE);

    //
    // Return the number of bytes sent.
    //
    return(i32BufLen);
}

//*****************************************************************************
//
// Read a packet from the DMA receive buffer into the uIP packet buffer.
//
//*****************************************************************************
int32_t Ethernet_PacketReceive(uint32_t ui32Base, uint8_t *pui8Buf, int32_t i32BufLen)
{
    int_fast32_t i32FrameLen;// i32Loop;

    //
    // Check the arguments.
    //
    ASSERT(ui32Base == EMAC0_BASE);
    ASSERT(pui8Buf != 0);
    ASSERT(i32BufLen > 0);

    //
    // By default, we assume we got a bad frame.
    //
    i32FrameLen = 0;

    //
    // Make sure that we own the receive descriptor.
    //
    if(!(g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus & DES0_RX_CTRL_OWN))
    {
        //
        // We own the receive descriptor so check to see if it contains a valid
        // frame.  Look for a descriptor error, indicating that the incoming
        // packet was truncated or, if this is the last frame in a packet,
        // the receive error bit.
        //
        if(!(g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus &
             DES0_RX_STAT_ERR))
        {
            //
            // We have a valid frame so copy the content to the supplied
            // buffer. First check that the "last descriptor" flag is set.  We
            // sized the receive buffer such that it can always hold a valid
            // frame so this flag should never be clear at this point but...
            //
            if(g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus &
               DES0_RX_STAT_LAST_DESC)
            {
                i32FrameLen =
                    ((g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus &
                      DES0_RX_STAT_FRAME_LENGTH_M) >>
                     DES0_RX_STAT_FRAME_LENGTH_S);

                //
                // Sanity check.  This shouldn't be required since we sized the
                // uIP buffer such that it's the same size as the DMA receive
                // buffer but, just in case...
                //
                if(i32FrameLen > i32BufLen)
                {
                    i32FrameLen = i32BufLen;
                }

                //
                // Copy the data from the DMA receive buffer into the provided
                // frame buffer.
                //
//                for(i32Loop = 0; i32Loop < i32FrameLen; i32Loop++)
//                {
//                    pui8Buf[i32Loop] = g_pui8RxBuffer[i32Loop];
//                }
                memcpy(pui8Buf, g_psRxDescriptor[g_ui32RxDescIndex].pvBuffer1, i32FrameLen);
            }
        }

        //
        // Mark the next descriptor in the ring as available for the receiver
        // to write into.
        //
        g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus = DES0_RX_CTRL_OWN;

        //
        // Move on to the next descriptor in the chain.
        //
        g_ui32RxDescIndex++;
        if(g_ui32RxDescIndex == NUM_RX_DESCRIPTORS)
        {
            g_ui32RxDescIndex = 0;
        }

    }

    //
    // Return the Frame Length
    //
    return(i32FrameLen);
}

int32_t Ethernet_FrameLength(void)
{
    int_fast32_t i32FrameLen;
    //
    // By default, we assume we got a bad frame.
    //
    i32FrameLen = 0;
    //
    // Make sure that we own the receive descriptor.
    //
    if(!(g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus & DES0_RX_CTRL_OWN))
    {
        //
        // We own the receive descriptor so check to see if it contains a valid
        // frame.
        //
        if(!(g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus & DES0_RX_STAT_ERR))
        {
            //
            // We have a valid frame. First check that the "last descriptor"
            // flag is set. We sized the receive buffer such that it can
            // always hold a valid frame so this flag should never be clear at
            // this point but...
            //
            if(g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus & DES0_RX_STAT_LAST_DESC)
            {
                //
                // What size is the received frame?
                //
                i32FrameLen = ((g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus &
                        DES0_RX_STAT_FRAME_LENGTH_M) >> DES0_RX_STAT_FRAME_LENGTH_S);

                return (i32FrameLen);
            }
        }
    }

    return 0;       // Bad Frame
}
