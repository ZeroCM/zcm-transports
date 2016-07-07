#include <assert.h>

#include "zcm/zcm.h"
#include "zcm/transport.h"
#include "zcm/transport/generic_serial_transport.h"

#include "transport_tiva_uart.h"

#include "driverlib/gpio.h"
#include "driverlib/uart.h"

static uint32_t uartPut(const uint8_t* data, uint32_t nData, void* usr)
{
    uint32_t uart_base = (uint32_t) usr;
    uint32_t ret = 0;
    while (ret < nData && UARTCharPutNonBlocking(uart_base, data[ret])) ret++;
    return ret;
}

static uint32_t uartGet(uint8_t* data, uint32_t nData, void* usr)
{
    uint32_t uart_base = (uint32_t) usr;
    uint32_t ret = 0;
    while (ret < nData && UARTCharsAvail(uart_base))
        data[ret++] = UARTCharGet(uart_base);
    return ret;
}

static uint32_t uart_validate_baud(uint32_t target_baud)
{
    static const uint8_t  nbaud_rates  = 8;
    static       uint32_t baud_rates[] = {9600, 19200, 38400, 57600, 115200,
                                          230400, 460800, 921600};
    static const uint32_t default_baud = 115200;

    uint32_t baud = default_baud;
    uint8_t i;
    for (i = 0; i < nbaud_rates; ++i) {
        if (target_baud == baud_rates[i]) {
            baud = target_baud;
            break;
        }
    }
    return baud;
}

zcm_trans_t* __zcm_trans_tiva_uart_create(uint32_t port_base,
                                          uint32_t uart_base,
                                          uint32_t sysctl_port_base,
                                          uint32_t sysctl_uart_base,
                                          uint32_t pinmap_rx,
                                          uint32_t pinmap_tx,
                                          uint32_t pinnum_rx,
                                          uint32_t pinnum_tx,
                                          uint32_t baud)
{
	SysCtlPeripheralEnable(sysctl_port_base);
	SysCtlPeripheralEnable(sysctl_uart_base);

	GPIOPinConfigure(pinmap_rx);
	GPIOPinConfigure(pinmap_tx);

	GPIOPinTypeUART(port_base, pinnum_rx | pinnum_tx);
    GPIOPadConfigSet(port_base, pinnum_rx, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    UARTConfigSetExpClk(uart_base, SysCtlClockGet(), uart_validate_baud(baud),
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    UARTFIFOEnable(uart_base);

	UARTEnable(uart_base);

	while (UARTCharsAvail(uart_base)) UARTCharGetNonBlocking(uart_base);

    assert(sizeof(void*) >= sizeof(uint32_t));
    return zcm_trans_generic_serial_create(uartGet, uartPut, (void*) uart_base);
}
