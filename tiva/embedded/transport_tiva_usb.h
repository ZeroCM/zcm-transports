#ifndef _ZCM_TRANS_TIVA_USB_H
#define _ZCM_TRANS_TIVA_USB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "zcm/zcm.h"
#include "zcm/transport.h"

#include "stdint.h"

zcm_trans_t* zcm_trans_tiva_usb_create(uint64_t serial_num,
                                       uint64_t (*timestamp_now)(void* usr),
                                       void* usr);

#ifdef __cplusplus
}
#endif

#endif /* _ZCM_TRANS_TIVA_USB_H */
