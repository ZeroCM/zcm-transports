#ifndef _ZCM_TRANS_RPI_PICO_USB_H
#define _ZCM_TRANS_RPI_PICO_USB_H

#include "zcm/transport.h"

zcm_trans_t* zcm_trans_rpi_pico_usb_create(uint64_t (*timestamp_now)(void*),
                                           void* usr);

#endif /* _ZCM_TRANS_RPI_PICO_USB_H */
