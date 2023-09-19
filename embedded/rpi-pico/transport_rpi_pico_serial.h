#ifndef _ZCM_TRANS_RPI_PICO_SERIAL_H
#define _ZCM_TRANS_RPI_PICO_SERIAL_H

#include "zcm/transport.h"

zcm_trans_t* zcm_trans_rpi_pico_serial_create(uint32_t baud,
                                              uint64_t (*timestamp_now)(void*),
                                              void* usr);

#endif /* _ZCM_TRANS_RPI_PICO_SERIAL_H */
