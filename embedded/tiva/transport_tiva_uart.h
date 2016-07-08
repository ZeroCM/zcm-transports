#ifndef _ZCM_TRANS_TIVA_UART_H
#define _ZCM_TRANS_TIVA_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "zcm/zcm.h"
#include "zcm/transport.h"

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"

// Example instantiation of a uart transport on UART2 with rx and tx pins
// PG4 and PG5 and baud rate 115200
//
// uint64_t timestamp_now(void* usr) { <return current time> }
// zcm_t* zcmUart = zcm_trans_tiva_uart_create(2, G, 4, G, 5, 115200, timestamp_now, NULL);
//

#define zcm_trans_tiva_uart_create(UART_NUM, RX_PORT, RX_PIN, TX_PORT, TX_PIN, \
                                   BAUD, TIME, USR) \
    __zcm_trans_tiva_uart_create(UART ## UART_NUM ## _BASE, \
                                 SYSCTL_PERIPH_UART ## UART_NUM, \
                                 GPIO_PORT ## RX_PORT ## _BASE, \
                                 SYSCTL_PERIPH_GPIO ## RX_PORT, \
                                 GPIO_P ## RX_PORT ## RX_PIN ## _U ## UART_NUM ## RX, \
                                 GPIO_PIN_ ## RX_PIN, \
                                 GPIO_PORT ## TX_PORT ## _BASE, \
                                 SYSCTL_PERIPH_GPIO ## TX_PORT, \
                                 GPIO_P ## TX_PORT ## TX_PIN ## _U ## UART_NUM ## TX, \
                                 GPIO_PIN_ ## TX_PIN, \
                                 BAUD, TIME, USR)


zcm_trans_t* __zcm_trans_tiva_uart_create(uint32_t uart_base,
                                          uint32_t sysctl_uart_base,
                                          uint32_t rx_port_base,
                                          uint32_t sysctl_rx_port_base,
                                          uint32_t pinmap_rx,
                                          uint32_t pinnum_rx,
                                          uint32_t tx_port_base,
                                          uint32_t sysctl_tx_port_base,
                                          uint32_t pinmap_tx,
                                          uint32_t pinnum_tx,
                                          uint32_t baud,
                                          uint64_t (*timestamp_now)(void*),
                                          void*    usr);
#ifdef __cplusplus
}
#endif

#endif /* _ZCM_TRANS_TIVA_UART_H */
