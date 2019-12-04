/*
 * DO NOT MODIFY THIS FILE BY HAND
 */
#undef ZCM_EMBEDDED
#define ZCM_EMBEDDED
#ifndef _ZCM_TRANS_TIVA_UART_H
#define _ZCM_TRANS_TIVA_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "zcm/zcm.h"
#include "zcm/transport.h"

#include "sw/drivers/sci/src/32b/f28x/f2806x/sci.h"


zcm_trans_t* zcm_trans_c2000_uart_create(SCI_Handle sci,
                                         uint64_t (*timestamp_now)(void*),
                                         void*    usr);

#ifdef __cplusplus
}
#endif

#endif /* _ZCM_TRANS_TIVA_UART_H */
