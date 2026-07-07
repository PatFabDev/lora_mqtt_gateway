/*
 * radio.c
 *
 * SX1262 radio interface.
 */

#include "radio.h"

#include <stdio.h>
#include <stdbool.h>

#define RADIO_MAX_PAYLOAD_SIZE    256U

int radio_init(void)
{
    sx126x_status_t rc;

    /* Standby */
    rc = sx126x_set_standby(NULL, SX126X_STANDBY_CFG_RC);
    if (rc != SX126X_STATUS_OK)
    {
        return -1;
    }

    rc = sx126x_set_dio3_as_tcxo_ctrl(NULL, SX126X_TCXO_CTRL_3_3V, 100);
    if (rc != SX126X_STATUS_OK)
    {
        return -1;
    }

    /* LoRa Mode */
    rc = sx126x_set_pkt_type(NULL, SX126X_PKT_TYPE_LORA);
    if (rc != SX126X_STATUS_OK)
    {
        return -1;
    }

    /* Sync Word */
    rc = sx126x_set_lora_sync_word(NULL, 0x12);
    if (rc != SX126X_STATUS_OK)
    {
        return -1;
    }

    /* RF Frequency */
    rc = sx126x_set_rf_freq(NULL, 868000000);
    if (rc != SX126X_STATUS_OK)
    {
        return -1;
    }

    /* Modulation Parameters */
    sx126x_mod_params_lora_t mod =
    {
        .sf   = SX126X_LORA_SF12,
        .bw   = SX126X_LORA_BW_125,
        .cr   = SX126X_LORA_CR_4_5,
        .ldro = 1
    };

    rc = sx126x_set_lora_mod_params(NULL, &mod);
    if (rc != SX126X_STATUS_OK)
    {
        return -1;
    }

    /* Packet Parameters */
    sx126x_pkt_params_lora_t pkt =
    {
        .preamble_len_in_symb = 8,
        .header_type = SX126X_LORA_PKT_EXPLICIT,
        .pld_len_in_bytes = 5,
        .crc_is_on = true,
        .invert_iq_is_on = false
    };

    rc = sx126x_set_lora_pkt_params( NULL, &pkt);
    if (rc != SX126X_STATUS_OK)
    {
        return -1;
    }

    /* IRQ Mapping */
    rc = sx126x_set_dio_irq_params(NULL, SX126X_IRQ_RX_DONE, SX126X_IRQ_RX_DONE, 0, 0);
    if (rc != SX126X_STATUS_OK)
    {
        return -1;
    }

    /* RX Gain Boost */
    rc = sx126x_cfg_rx_boosted(NULL, true);
    if (rc != SX126X_STATUS_OK)
    {
        return -1;
    }

    /* Continuous RX */
    rc = sx126x_set_rx(NULL, 0);
    if (rc != SX126X_STATUS_OK)
    {
        return -1;
    }

    /* Print SX1262 Configuration after Init */
    printf("\n----------------------------------------\n");
    printf("SX1262 Configuration\n");
    printf("----------------------------------------\n");
    printf("Mode        : LoRa\n");
    printf("Frequency   : 868.000 MHz\n");
    printf("Sync Word   : 0x12\n");
    printf("SF          : SF12\n");
    printf("Bandwidth   : 250 kHz\n");
    printf("Coding Rate : 4/5\n");
    printf("RX Gain     : Boosted\n");
    printf("CRC         : Enabled\n");
    printf("Header      : Explicit\n");
    printf("----------------------------------------\n");
    printf("LoRa Receiver ready.\n");
    printf("----------------------------------------\n");
    printf("Waiting for packets...\n");

    return 0;
}

int radio_process(radio_packet_t *packet)
{
    sx126x_status_t rc;
    sx126x_irq_mask_t irq;
    sx126x_rx_buffer_status_t rx;
    sx126x_pkt_status_lora_t pkt_status;
    int dio1;

    /* Validate input parameter */
    if (packet == NULL)
    {
        return -1;
    }

    /* Check DIO1 interrupt status */
    dio1 = sx126x_dio1();

    if (dio1 < 0)
    {
        return -1;
    }

    if (dio1 == 0)
    {
        return 1;
    }

    /* Read IRQ status */
    rc = sx126x_get_irq_status(NULL, &irq);

    if (rc != SX126X_STATUS_OK)
    {
        return -1;
    }

    /* No packet received */
    if ((irq & SX126X_IRQ_RX_DONE) == 0)
    {
        return 1;
    }

    /* Read RX buffer information */
    rc = sx126x_get_rx_buffer_status(NULL, &rx);

    if (rc != SX126X_STATUS_OK)
    {
        return -1;
    }

    /* Read payload */
    rc = sx126x_read_buffer(
            NULL,
            rx.buffer_start_pointer,
            packet->payload,
            rx.pld_len_in_bytes);

    if (rc != SX126X_STATUS_OK)
    {
        return -1;
    }

    packet->payload_length = rx.pld_len_in_bytes;
    packet->payload[packet->payload_length] = '\0';

    /* Read packet status */
    rc = sx126x_get_lora_pkt_status(NULL, &pkt_status);

    if (rc != SX126X_STATUS_OK)
    {
        return -1;
    }

    packet->rssi        = pkt_status.rssi_pkt_in_dbm;
    packet->snr         = pkt_status.snr_pkt_in_db;
    packet->signal_rssi = pkt_status.signal_rssi_pkt_in_dbm;

    /* Clear RX interrupt */
    rc = sx126x_clear_irq_status(NULL, SX126X_IRQ_RX_DONE);

    if (rc != SX126X_STATUS_OK)
    {
        return -1;
    }

    /* Restart continuous RX */
    rc = sx126x_set_rx(NULL, 0);

    if (rc != SX126X_STATUS_OK)
    {
        return -1;
    }

    /*
    printf("\n----------------------------------------\n");
    printf("Data   : %s\n", packet->payload);
    printf("Length : %u\n", packet->payload_length);
    printf("RSSI   : %d dBm\n", packet->rssi);
    printf("SNR    : %d dB\n", packet->snr);
    printf("Signal : %d dBm\n", packet->signal_rssi);
    printf("----------------------------------------\n");
    */

    return 0;
}
