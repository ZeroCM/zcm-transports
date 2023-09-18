#include "transport_rpi_pico_usb.h"

#include "zcm/transport.h"
#include "zcm/transport/generic_serial_transport.h"
#include "zcm/zcm.h"

#include "pico/stdio.h"
#include "pico/stdlib.h"

#define ZCM_GENERIC_SERIAL_MTU (128)
#define ZCM_GENERIC_SERIAL_NMSGS (5)
#define ZCM_GENERIC_SERIAL_BUFFER_SIZE                                         \
    ((ZCM_GENERIC_SERIAL_NMSGS * ZCM_GENERIC_SERIAL_MTU) +                     \
     (ZCM_GENERIC_SERIAL_NMSGS * ZCM_CHANNEL_MAXLEN))

static size_t uartPut(const uint8_t* data, size_t nData, void* usr) {
    size_t ret = 0;
    while (ret < nData)
        putchar_raw(data[ret++]);

    return ret;
}

static size_t uartGet(uint8_t* data, size_t nData, void* usr) {
    size_t ret = 0;
    int c = 0;
    while (ret < nData) {
        c = getchar_timeout_us(0);
        if (c == PICO_ERROR_TIMEOUT) return ret;
        data[ret++] = (uint8_t)c;
    }

    return ret;
}

zcm_trans_t* zcm_trans_rpi_pico_usb_create(uint64_t (*timestamp_now)(void*),
                                           void* usr) {

    stdio_init_all();

    return zcm_trans_generic_serial_create(
        uartGet, uartPut, NULL, timestamp_now, usr, ZCM_GENERIC_SERIAL_MTU,
        ZCM_GENERIC_SERIAL_BUFFER_SIZE);
}
