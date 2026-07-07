/*
 * radio.h
 *
 * SX1262 radio interface.
 * 
 * Hardware configuration:
 *
 * RXEN is permanently connected to 3.3 V.
 * TXEN is permanently connected to GND.
 *
 * Therefore the SX1262 always operates in receive mode.
 * No software control of RXEN/TXEN is required.
 */

#ifndef RADIO_H
#define RADIO_H

#include "../sx126x_driver/src/sx126x.h"
#include "../sx126x_driver/src/sx126x_regs.h"
#include "gpio.h"

#define RADIO_MAX_PAYLOAD_SIZE      256U

/**
 * @brief Received radio packet.
 */
typedef struct
{
    uint8_t payload[RADIO_MAX_PAYLOAD_SIZE];
    uint16_t payload_length;
    int16_t rssi;
    int8_t snr;
    int16_t signal_rssi;

} radio_packet_t;

/**
 * @brief Initialize the SX1262 radio.
 *
 * @return
 *         0 : Success
 *        -1 : Error
 */
int radio_init(void);

/**
 * @brief Process pending radio events.
 *
 * @param[out] packet Received radio packet.
 *
 * @return
 *         0 : Packet received
 *         1 : No packet available
 *        -1 : Error
 */
int radio_process(radio_packet_t *packet);

#endif /* RADIO_H */