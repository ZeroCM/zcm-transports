#include "transport_rpi_pico_serial.h"

#include "zcm/transport.h"
#include "zcm/transport/generic_serial_transport.h"
#include "zcm/zcm.h"

#include "hardware/uart.h"
#include "pico/stdlib.h"
#include <stdio.h>

#define UART_ID uart0
#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define ZCM_GENERIC_SERIAL_MTU (128)
#define ZCM_GENERIC_SERIAL_NMSGS (5)
#define ZCM_GENERIC_SERIAL_BUFFER_SIZE                                         \
    ((ZCM_GENERIC_SERIAL_NMSGS * ZCM_GENERIC_SERIAL_MTU) +                     \
     (ZCM_GENERIC_SERIAL_NMSGS * ZCM_CHANNEL_MAXLEN))

static bool useUartHardware = false;

static size_t uartPut(const uint8_t* data, size_t nData, void* usr) {
    size_t ret = 0;
    while (ret < nData) {
        if (useUartHardware)
            uart_putc(UART_ID, data[ret++]);
        else
            putchar(data[ret++]);
    }

    return ret;
}

static size_t uartGet(uint8_t* data, size_t nData, void* usr) {
    size_t ret = 0;
    while (ret < nData && uart_is_readable(UART_ID))
        if (useUartHardware)
            data[ret++] = uart_getc(UART_ID);
        else
            data[ret++] = getchar();

    return ret;
}

zcm_trans_t* zcm_trans_rpi_pico_serial_create(bool useUsbSerial, uint32_t baud,
                                              uint64_t (*timestamp_now)(void*),
                                              void* usr) {
    // RRR (Bendes): this is gross. The whole point of the put/get void* is
    // for you to pass in context if needed. You shouldn't need this at all
    // either way
    useUartHardware = !useUsbSerial;

    // RRR (Bendes): Get rid of all these switches. Make 2 transports
    if (useUartHardware) {
        // Set up our UART with the required speed.
        uart_init(UART_ID, baud);

        // Set the TX and RX pins by using the function select on the GPIO
        // Set datasheet for more information on function select
        gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
        gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    } else {
        stdio_init_all();
    }

    return zcm_trans_generic_serial_create(
        uartGet, uartPut, NULL, timestamp_now, usr, ZCM_GENERIC_SERIAL_MTU,
        ZCM_GENERIC_SERIAL_BUFFER_SIZE);
}

