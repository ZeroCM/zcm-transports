#ifndef _ZCM_TRANS_ARDUINO_SERIAL_H
#define _ZCM_TRANS_ARDUINO_SERIAL_H

#include <Arduino.h>

#include "zcm/transport.h"

// Example instantiation of this transport with baud rate 115200
//
// uint64_t timestamp_now(void* usr) { <return current time> }
// zcm_t* zcmSerial = zcm_trans_arduino_serial_create(115200, timestamp_now, NULL);
//

zcm_trans_t* zcm_trans_arduino_serial_create(uint32_t baud,
                                             uint64_t (*timestamp_now)(void*),
                                             void*    usr);

#endif /* _ZCM_TRANS_ARDUINO_SERIAL_H */
