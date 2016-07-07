#ifndef _ZCM_TRANS_TIVA_UART_H
#define _ZCM_TRANS_TIVA_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "zcm/zcm.h"
#include "zcm/transport.h"

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"

#define zcm_trans_tiva_uart_create(PORT, UART_NUM, RX_PIN, TX_PIN, BAUD) \
    __zcm_trans_tiva_uart_create(GPIO_PORT ## PORT ## _BASE, \
                                 UART ## UART_NUM ## _BASE, \
                                 SYSCTL_PERIPH_GPIO ## PORT, \
                                 SYSCTL_PERIPH_UART ## UART_NUM, \
                                 GPIO_P ## PORT ## RX_PIN ## _U ## UART_NUM ## RX, \
                                 GPIO_P ## PORT ## TX_PIN ## _U ## UART_NUM ## TX, \
                                 GPIO_PIN_ ## RX_PIN, \
                                 GPIO_PIN_ ## TX_PIN, \
                                 BAUD)


zcm_trans_t* __zcm_trans_tiva_uart_create(uint32_t port_base,
                                          uint32_t uart_base,
                                          uint32_t sysctl_port_base,
                                          uint32_t sysctl_uart_base,
                                          uint32_t pinmap_rx,
                                          uint32_t pinmap_tx,
                                          uint32_t pinnum_rx,
                                          uint32_t pinnum_tx,
                                          uint32_t baud);
#ifdef __cplusplus
}
#endif

#endif /* _ZCM_TRANS_TIVA_UART_H */
