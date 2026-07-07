# Pin Mapping

This project was developed and tested with the **Waveshare Core1262-868M** LoRa module connected to a Raspberry Pi via the SPI interface.

## Wiring

| Waveshare Core1262-868M | Raspberry Pi |
|--------------------------|--------------|
| VCC                      | 3.3V         |
| GND                      | GND          |
| SCK                      | GPIO11 (SCLK)|
| MISO                     | GPIO9 (MISO) |
| MOSI                     | GPIO10 (MOSI)|
| NSS                      | GPIO8 (CE0)  |
| BUSY                     | GPIO23       |
| DIO1                     | GPIO24       |
| RESET                    | GPIO25       |
| RXEN                     | 3.3V         |
| TXEN                     | GND          |

> **Note**
>
> This pin assignment matches the Linux HAL implementation contained in this repository. If you use different GPIO pins, make sure to update the corresponding definitions in the HAL source code.

## References

- [Waveshare Core1262-868M Wiki](https://www.waveshare.com/wiki/Core1262-868M)
- [Raspberry Pi GPIO Documentation](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#gpio)