# LoRa MQTT Gateway

A lightweight LoRa gateway for Linux-based systems using the official Semtech SX126x driver, a custom Linux Hardware Abstraction Layer (HAL), and a modular software architecture.

The gateway receives **raw LoRa packets** from SX1262-based sensor nodes, parses the received payload, converts it into JSON and publishes the data to an MQTT broker for further processing.

The project is designed as a modular IoT gateway and can easily be extended with additional payload formats, sensors, MQTT topics or external MQTT clients.

---

# Features

* Raspberry Pi 4 compatible
* SX1262 LoRa transceiver
* Official Semtech SX126x driver
* Custom Linux GPIO HAL
* Custom Linux SPI HAL
* Linux implementation of the Semtech HAL
* Modular radio abstraction
* ASCII payload parser
* JSON packet generation
* MQTT publishing
* MQTT authentication support
* Docker Compose deployment
* Example Node-RED flow included

---

# Project Architecture

```text
                  +------------------------+
                  | Heltec LoRa Node       |
                  | ESP32 + SX1262         |
                  +-----------+------------+
                              |
                              | Raw LoRa
                              |
                              ▼
                  +------------------------+
                  | Raspberry Pi Gateway   |
                  | SX1262 Receiver        |
                  +-----------+------------+
                              |
                              ▼
                       radio_process()
                              |
                              ▼
                  parser_parse_payload()
                              |
                              ▼
                       sensor_data_t
                              |
                              ▼
                        json_create()
                              |
                              ▼
                       MQTT Publisher
                              |
                              ▼
                         MQTT Broker
                              |
                    +---------+---------+
                    |                   |
                    ▼                   ▼
               Node-RED          Any MQTT Client
               (example)         (Home Assistant,
                                 Grafana,
                                 custom software,
                                 MQTT Explorer, ...)
```

---

# Repository Structure

```text
lora_mqtt_gateway/
│
├── README.md
├── LICENSE
├── docker-compose.yaml
│
├── gateway/
│   ├── Dockerfile
│   ├── Makefile
│   ├── docs/
│   ├── hal/
│   ├── src/
│   └── sx126x_driver/
│
├── mosquitto/
│   └── mosquitto.conf
│
└── nodered/
    └── flows.json
```

---

# Software Architecture

The gateway software is organized into independent modules.

```text
gateway/
├── src/
│   ├── main.c
│   ├── radio.c
│   ├── parser.c
│   ├── json.c
│   └── mqtt.c
│
├── hal/
│   ├── gpio.c
│   ├── spi.c
│   └── sx126x_hal_linux.c
│
└── sx126x_driver/
    └── Official Semtech SX126x driver
```

## Module Overview

### radio.c

* Configures the SX1262
* Handles packet reception
* Reads RSSI, SNR and Signal RSSI

### parser.c

* Parses received ASCII payloads
* Converts payload strings into structured sensor data

### json.c

* Generates JSON messages
* Adds gateway information and timestamps

### mqtt.c

* Connects to the MQTT broker
* Publishes JSON payloads
* Supports authenticated MQTT connections
* Implements MQTT Last Will

### HAL

* Linux GPIO implementation
* Linux SPI implementation
* Linux HAL implementation for the Semtech driver

---

# Payload Format

The gateway expects ASCII payloads in the following format:

```text
node01;24.6;53.1
```

where

```text
Node ID ; Temperature ; Relative Humidity
```

Example payload

```text
node01;24.0;50.8
```

is converted into

```json
{
  "gateway": "pi4",
  "timestamp": "2026-07-06T20:15:42",
  "payload": {
    "node": "node01",
    "temperature": 24.0,
    "humidity": 50.8
  },
  "rssi": -98,
  "snr": -6,
  "signal_rssi": -106
}
```

The generated JSON object is published to the configured MQTT topic.

---

# Requirements

## Hardware

* Raspberry Pi 4 (or compatible Linux SBC)
* SX1262 LoRa transceiver
* SPI enabled

## Software

* Docker
* Docker Compose

## LoRa Node

The gateway is designed for **raw LoRa communication**.

**LoRaWAN is not supported.**

A compatible transmitter must use identical radio parameters:

* Frequency
* Bandwidth
* Spreading Factor
* Coding Rate
* Sync Word
* CRC configuration

The example node used during development is based on a **Heltec ESP32 + SX1262** board.

---

# Getting Started

Clone the repository

```bash
git clone https://github.com/<github_username>/lora_mqtt_gateway.git
cd lora_mqtt_gateway
```

Build and start all containers

```bash
docker compose up -d
```

Display the gateway logs

```bash
docker compose logs -f lora_mqtt_gateway
```

Stop all containers

```bash
docker compose down
```

---

# Included Services

The Docker Compose stack contains the following services:

* LoRa MQTT Gateway
* Eclipse Mosquitto MQTT Broker
* Node-RED example environment

Additionally, optional **InfluxDB** and **Grafana** containers are included.

These optional services are **not required** for operating the gateway and are **not covered** by this documentation. They are provided only as a starting point for users who wish to build their own storage or visualization pipeline.

---

# MQTT Authentication

The included Mosquitto broker is configured to require client authentication.

Every MQTT client connecting to the broker (for example the gateway, Node-RED or MQTT Explorer) must provide valid credentials.

Example credentials:

```text
Username: admin
Password: admin
```

The broker stores user credentials inside a Mosquitto password file.

Create the password file locally:

```bash
mosquitto_passwd -c mosquitto/passwordfile admin
```

The supplied credentials are intended for demonstration purposes only.

For production environments, create your own users and strong passwords.

---

# Node-RED Example

A ready-to-use Node-RED flow is included in the repository.

The example demonstrates how to

* connect to the MQTT broker
* authenticate using username and password
* subscribe to MQTT topics
* inspect incoming JSON payloads using Debug nodes

Import the flow via

```text
Menu → Import → nodered/flows.json
```

After importing the flow, open the MQTT broker configuration, enter your own MQTT credentials and deploy the flow.

---

# LoRa Configuration

Current receiver configuration

| Parameter        | Value         |
| ---------------- | ------------- |
| Frequency        | 868 MHz       |
| Modulation       | LoRa          |
| Bandwidth        | 250 kHz       |
| Spreading Factor | SF12          |
| Coding Rate      | 4/5           |
| Sync Word        | 0x12          |
| CRC              | Enabled       |
| RX Gain          | Boosted       |
| Receive Mode     | Continuous RX |

The transmitting node must use identical radio parameters.

---

# Third-Party Software

This project uses the official **Semtech SX126x driver**, licensed under the **Clear BSD License**.

The original driver source code and license are included in the `gateway/sx126x_driver` directory.

The following components were developed specifically for this project:

* Linux GPIO Hardware Abstraction Layer
* Linux SPI Hardware Abstraction Layer
* Linux implementation of the Semtech HAL
* Radio abstraction layer
* Payload parser
* JSON generator
* MQTT gateway application

The project intentionally stops at the MQTT layer.

Any MQTT-compatible software can be connected to the broker, such as Node-RED, Home Assistant, MQTT Explorer, Grafana or custom applications.

---

# License

The source code of this project is licensed under the MIT License.

The included Semtech SX126x driver remains licensed under the original **Clear BSD License** and is distributed under its original license terms.
