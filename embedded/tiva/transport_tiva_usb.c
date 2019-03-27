#include "zcm/zcm.h"
#include "zcm/transport.h"
#include "zcm/transport/generic_serial_transport.h"
#include "transport_tiva_usb.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"

#include "usblib/usblib.h"
#include "usblib/usbcdc.h"
#include "usblib/usb-ids.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdcdc.h"
#include "usb_serial_structs.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"

#define ZCM_GENERIC_SERIAL_MTU (128)
#define ZCM_GENERIC_SERIAL_NMSGS (5)
#define ZCM_GENERIC_SERIAL_BUFFER_SIZE ((ZCM_GENERIC_SERIAL_NMSGS * ZCM_GENERIC_SERIAL_MTU) + \
                                        (ZCM_GENERIC_SERIAL_NMSGS * ZCM_CHANNEL_MAXLEN))


size_t usbPut(const uint8_t* data, size_t nData, void* usr)
{
    return USBBufferWrite((tUSBBuffer *)&g_sTxBuffer, data, nData);
}

size_t usbGet(uint8_t* data, size_t nData, void* usr)
{
    return USBBufferRead((tUSBBuffer *)&g_sRxBuffer, data, nData);
}


/*********************** TIVA USB FUNTIONS & INTERRUPTS ************************/

// This should only be called once
zcm_trans_t* zcm_trans_tiva_usb_create(uint64_t serial_num,
                                       uint64_t (*timestamp_now)(void* usr),
                                       void* usr)
{
    //
    // Configure the required pins for USB operation.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_5 | GPIO_PIN_4);

    char printBuf[17];
    snprintf(printBuf, 17, "%016" PRIx64, serial_num);

    g_pui8SerialNumberString[0] = 2 + (16 * 2); // Size byte
    g_pui8SerialNumberString[1] = USB_DTYPE_STRING;
    uint8_t i;
    for (i=0; i<16; ++i) {
        g_pui8SerialNumberString[2*i + 2] = printBuf[i];
        g_pui8SerialNumberString[2*i + 3] = 0;
    }

    //
    // Initialize the transmit and receive buffers.
    //
    USBBufferInit(&g_sTxBuffer);
    USBBufferInit(&g_sRxBuffer);

    //
    // Set the USB stack mode to Device mode without VBUS monitoring.
    //
    USBStackModeSet(0, eUSBModeForceDevice, 0);

    //
    // Pass our device information to the USB library and place the device
    // on the bus.
    //
    USBDCDCInit(0, &g_sCDCDevice);

    return zcm_trans_generic_serial_create(usbGet, usbPut, NULL,
                                           timestamp_now, usr,
                                           ZCM_GENERIC_SERIAL_MTU,
                                           ZCM_GENERIC_SERIAL_BUFFER_SIZE);
}

//*****************************************************************************
//
// Handles CDC driver notifications related to control and setup of the device.
//
// \param pvCBData is the client-supplied callback pointer for this channel.
// \param ui32Event identifies the event we are being notified about.
// \param ui32MsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the CDC driver to perform control-related
// operations on behalf of the USB host.  These functions include setting
// and querying the serial communication parameters, setting handshake line
// states and sending break conditions.
//
// \return The return value is event-specific.
//
//*****************************************************************************
uint32_t
ControlHandler(void *pvCBData, uint32_t ui32Event,
               uint32_t ui32MsgValue, void *pvMsgData)
{
    //
    // Which event are we being asked to process?
    //
    switch(ui32Event)
    {
        //
        // We are connected to a host and communication is now possible.
        //
        case USB_EVENT_CONNECTED:
            //
            // Flush our buffers.
            //
            USBBufferFlush(&g_sTxBuffer);
            USBBufferFlush(&g_sRxBuffer);

            break;

        //
        // The host has disconnected.
        //
        case USB_EVENT_DISCONNECTED:
            break;

        //
        // Return the current serial communication parameters.
        //
        case USBD_CDC_EVENT_GET_LINE_CODING:
//            GetLineCoding(pvMsgData);
            break;

        //
        // Set the current serial communication parameters.
        //
        case USBD_CDC_EVENT_SET_LINE_CODING:
//            SetLineCoding(pvMsgData);
            break;

        //
        // Set the current serial communication parameters.
        //
        case USBD_CDC_EVENT_SET_CONTROL_LINE_STATE:
//            SetControlLineState((uint16_t)ui32MsgValue);
            break;

        //
        // Send a break condition on the serial line.
        //
        case USBD_CDC_EVENT_SEND_BREAK:
//            SendBreak(true);
            break;

        //
        // Clear the break condition on the serial line.
        //
        case USBD_CDC_EVENT_CLEAR_BREAK:
//            SendBreak(false);
            break;

        //
        // Ignore SUSPEND and RESUME for now.
        //
        case USB_EVENT_SUSPEND:
        case USB_EVENT_RESUME:
            break;

        //
        // We don't expect to receive any other events.  Ignore any that show
        // up in a release build or hang in a debug build.
        //
        default:
#ifdef DEBUG
            while(1);
#else
            break;
#endif

    }

    return(0);
}

//*****************************************************************************
//
// Handles CDC driver notifications related to the transmit channel (data to
// the USB host).
//
// \param pvCBData is the client-supplied callback pointer for this channel.
// \param ui32Event identifies the event we are being notified about.
// \param ui32MsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the CDC driver to notify us of any events
// related to operation of the transmit data channel (the IN channel carrying
// data to the USB host).
//
// \return The return value is event-specific.
//
//*****************************************************************************
uint32_t
TxHandler(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue,
          void *pvMsgData)
{
    //
    // Which event have we been sent?
    //
    switch(ui32Event)
    {
        case USB_EVENT_TX_COMPLETE:
            //
            // Since we are using the USBBuffer, we don't need to do anything
            // here.
            //
            break;

        //
        // We don't expect to receive any other events.  Ignore any that show
        // up in a release build or hang in a debug build.
        //
        default:
#ifdef DEBUG
            while(1);
#else
            break;
#endif

    }
    return(0);
}

//*****************************************************************************
//
// Handles CDC driver notifications related to the receive channel (data from
// the USB host).
//
// \param pvCBData is the client-supplied callback data value for this channel.
// \param ui32Event identifies the event we are being notified about.
// \param ui32MsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the CDC driver to notify us of any events
// related to operation of the receive data channel (the OUT channel carrying
// data from the USB host).
//
// \return The return value is event-specific.
//
//*****************************************************************************
uint32_t
RxHandler(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue,
          void *pvMsgData)
{
    //
    // Which event are we being sent?
    //
    switch(ui32Event)
    {
        //
        // A new packet has been received.
        //
        case USB_EVENT_RX_AVAILABLE:
        {
            // Don't think I have to actually handle the data here, but it might
            // turn out to be the case that the usb drivers on the TIVA are finiky
            break;
        }

        //
        // We are being asked how much unprocessed data we have still to
        // process. We return 0 if the UART is currently idle or 1 if it is
        // in the process of transmitting something. The actual number of
        // bytes in the UART FIFO is not important here, merely whether or
        // not everything previously sent to us has been transmitted.
        //
        case USB_EVENT_DATA_REMAINING:
        {
            return(0);
        }

        //
        // We are being asked to provide a buffer into which the next packet
        // can be read. We do not support this mode of receiving data so let
        // the driver know by returning 0. The CDC driver should not be sending
        // this message but this is included just for illustration and
        // completeness.
        //
        case USB_EVENT_REQUEST_BUFFER:
        {
            return(0);
        }

        //
        // We don't expect to receive any other events.  Ignore any that show
        // up in a release build or hang in a debug build.
        //
        default:
#ifdef DEBUG
            while(1);
#else
            break;
#endif
    }

    return(0);
}
