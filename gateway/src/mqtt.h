/*
 * mqtt.h
 *
 * MQTT client interface.
 */

#ifndef MQTT_H
#define MQTT_H

#include <stdbool.h>

/* MQTT broker configuration */
#define MQTT_HOST           "mosquitto"
#define MQTT_PORT           1883

/* MQTT client */
#define MQTT_CLIENT_ID      "pi4_gateway"

/* Authentication */
#define MQTT_USERNAME       "admin"
#define MQTT_PASSWORD       "admin"

/* Topics */
#define MQTT_TOPIC_DATA     "lora/nodes"
#define MQTT_TOPIC_STATUS   "lora/gateway/pi4/status"

/* Publish options */
#define MQTT_QOS            1
#define MQTT_RETAIN         false
#define MQTT_KEEP_ALIVE     60

/* Last Will */
#define MQTT_ONLINE_MESSAGE "online"
#define MQTT_WILL_MESSAGE   "offline"
#define MQTT_WILL_QOS       MQTT_QOS
#define MQTT_WILL_RETAIN    true

/**
 * @brief Initialize the MQTT client.
 *
 * Connects to the configured MQTT broker.
 *
 * @return
 *         0 : Success
 *        -1 : Error
 */
int mqtt_init(void);

/**
 * @brief Publish a JSON message.
 *
 * @param[in] json JSON string to publish.
 *
 * @return
 *         0 : Success
 *        -1 : Error
 */
int mqtt_publish(const char *json);

/**
 * @brief Disconnect from the MQTT broker and release resources.
 */
void mqtt_deinit(void);

#endif /* MQTT_H */