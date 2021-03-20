#include "zcm/zcm.h"
#include "zcm/transport.h"
#include "zcm/transport/generic_serial_transport.h"

#include "transport_arduino_serial.h"

#define ZCM_GENERIC_SERIAL_MTU (128)
#define ZCM_GENERIC_SERIAL_NMSGS (5)
#define ZCM_GENERIC_SERIAL_BUFFER_SIZE ((ZCM_GENERIC_SERIAL_NMSGS * ZCM_GENERIC_SERIAL_MTU) + \
                                        (ZCM_GENERIC_SERIAL_NMSGS * ZCM_CHANNEL_MAXLEN))

static size_t uartPut(const uint8_t* data, size_t nData, void* usr)
{
    size_t ret = 0;
    while (ret < nData && Serial.write(data[ret])) ret++;
    return ret;
}

static size_t uartGet(uint8_t* data, size_t nData, void* usr)
{
    size_t ret = 0;
    while (ret < nData && Serial.available())
        data[ret++] = Serial.read();
    return ret;
}

zcm_trans_t* zcm_trans_arduino_serial_create(uint32_t baud,
                                             uint64_t (*timestamp_now)(void*),
                                             void*    usr)
{
    Serial.begin(baud);
	Serial.flush();
    return zcm_trans_generic_serial_create(uartGet, uartPut, NULL,
                                           timestamp_now, usr,
                                           ZCM_GENERIC_SERIAL_MTU,
                                           ZCM_GENERIC_SERIAL_BUFFER_SIZE);
}
