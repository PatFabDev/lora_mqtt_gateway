CC=gcc

CFLAGS=\
-std=c11 \
-D_DEFAULT_SOURCE \
-g \
-Wall \
-Wextra \
-I./hal \
-I./src \
-I./sx126x_driver/src \
-I/usr/include

SRC=\
hal/gpio.c \
hal/spi.c \
hal/sx126x_hal_linux.c \
src/json.c \
src/main.c \
src/mqtt.c \
src/parser.c \
src/radio.c \
sx126x_driver/src/sx126x.c \
sx126x_driver/src/sx126x_driver_version.c

TARGET=lora_mqtt_gateway

LIBS=\
-lgpiod \
-lmosquitto

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)