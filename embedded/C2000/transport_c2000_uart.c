
/*
 * DO NOT MODIFY THIS FILE BY HAND
 */
#undef ZCM_EMBEDDED
#define ZCM_EMBEDDED
#include <assert.h>

#include "zcm/zcm.h"
#include "zcm/transport.h"
#include "zcm/transport/generic_serial_transport.h"

#include "transport_c2000_uart.h"

#define ZCM_GENERIC_SERIAL_MTU (128)
#define ZCM_GENERIC_SERIAL_NMSGS (5)
#define ZCM_GENERIC_SERIAL_BUFFER_SIZE ((ZCM_GENERIC_SERIAL_NMSGS * ZCM_GENERIC_SERIAL_MTU) + \
                                        (ZCM_GENERIC_SERIAL_NMSGS * ZCM_CHANNEL_MAXLEN))

static uint32_t uartPut(const uint8_t* data, uint32_t nData, void* usr)
{
    SCI_Handle sci = (SCI_Handle) usr;
    uint32_t ret = 0;
    while (ret < nData && SCI_putDataNonBlocking(sci,data[ret])) ret++;
    return ret;
}

static uint32_t uartGet(uint8_t* data, uint32_t nData, void* usr)
{
    SCI_Handle sci = (SCI_Handle) usr;
    uint32_t ret = 0;
    while (ret < nData) {
        uint16_t success;
        data[ret] = SCI_getDataNonBlocking(sci, &success);
        if (!success) break;
        ret++;
    }
    return ret;
}

zcm_trans_t* zcm_trans_c2000_uart_create(SCI_Handle sci,
                                         uint64_t (*timestamp_now)(void*),
                                         void*    usr)
{
    return zcm_trans_generic_serial_create(uartGet, uartPut, sci,
                                           timestamp_now, usr,
                                           ZCM_GENERIC_SERIAL_MTU,
                                           ZCM_GENERIC_SERIAL_BUFFER_SIZE);
}
