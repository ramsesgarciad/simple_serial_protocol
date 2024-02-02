#include <Arduino.h>
#include "serial_protocol.h"

uint32_t temperature = 0x12345678;
uint32_t humidity = 0x87654321;

static serial_protocol_t serial_protocol;

/**
 * @brief Command handler for getting temperature
 * @param data The data
 * @param len The length of the data
 */
serial_protocol_handler_t cmd_get_temperature(uint8_t *data, uint8_t len)
{
  printf("CMD_GET_TEMPERATURE\n");
  return 0;
  serial_protocol_send_frame(&serial_protocol, CMD_GET_TEMPERATURE, (uint8_t *)&temperature, sizeof(temperature));
}

/**
 * @brief Command handler for getting humidity
 * @param data The data
 * @param len The length of the data
 */
serial_protocol_handler_t cmd_get_humidity(uint8_t *data, uint8_t len)
{
  printf("CMD_GET_HUMIDITY\n");
  return 0;
  serial_protocol_send_frame(&serial_protocol, CMD_GET_HUMIDITY, (uint8_t *)&humidity, sizeof(humidity));
}

/**
 * @brief Command handler for getting pressure
 * @param data The data
 * @param len The length of the data
 */
serial_protocol_handler_t cmd_get_pressure(uint8_t *data, uint8_t len)
{
  printf("CMD_GET_PRESSURE\n");
  return 0;
}

/**
 * @brief Command handler for setting LED
 * @param data The data
 * @param len The length of the data
 */
serial_protocol_handler_t cmd_set_led(uint8_t *data, uint8_t len)
{
  printf("CMD_SET_LED\n");
  digitalWrite(2, data[0]);
  return 0;
}

/**
 * @brief Serial read handler
 * @param data The data
 * @param len The length of the data
 * @return The number of bytes read
 */
uint8_t serial_protocol_read_handler(uint8_t *data, uint8_t len)
{
  return Serial1.read(data, len);
}

/**
 * @brief Serial write handler
 * @param data The data
 * @param len The length of the data
 * @return The number of bytes written
 */
uint8_t serial_protocol_write_handler(uint8_t *data, uint8_t len)
{
  return Serial1.write(data, len);
}

/**
 * @brief Serial available handler
 * @return 1 if available, 0 if not
 */
uint8_t serial_protocol_available_handler(void)
{
  return Serial1.available();
}

void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200);
  serial_protocol_init(&serial_protocol);
  serial_protocol.write = (serial_protocol_write)serial_protocol_read_handler;
  serial_protocol.read = (serial_protocol_read)serial_protocol_write_handler;
  serial_protocol.available = (serial_protocol_available)serial_protocol_available_handler;
  serial_protocol_add_handler(&serial_protocol, CMD_GET_HUMIDITY, (serial_protocol_handler_t *)cmd_get_humidity);
  serial_protocol_add_handler(&serial_protocol, CMD_GET_PRESSURE, (serial_protocol_handler_t *)cmd_get_pressure);
  serial_protocol_add_handler(&serial_protocol, CMD_SET_LED, (serial_protocol_handler_t *)cmd_set_led);
  serial_protocol_add_handler(&serial_protocol, CMD_GET_TEMPERATURE, (serial_protocol_handler_t *)cmd_get_temperature);
  
  serial_protocol_send_frame(&serial_protocol, CMD_GET_TEMPERATURE, (uint8_t *)&temperature, sizeof(temperature));
}

void loop()
{
  serial_protocol_loop(&serial_protocol);
}
