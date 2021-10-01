#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "pins.h"

// === MODIFIED ===
#include "hardware/i2c.h"
#include "hardware/gpio.h"

#ifndef NDEBUG
#  include <stdio.h>
#endif

// === MODIFIED ===
static uint16_t TIMEOUT = 1000;

/*
 * Function twi_init
 * Desc     readys twi pins and sets twi bitrate
 * Input    none
 * Output   none
 */
void twi_init(void) {
  i2c_init(i2c1, TWI_FREQ);
  i2c_set_baudrate(i2c1, TWI_FREQ);
  gpio_set_function(PIN_WIRE_SDA, GPIO_FUNC_I2C);
  gpio_set_function(PIN_WIRE_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(PIN_WIRE_SDA);
  gpio_pull_up(PIN_WIRE_SCL);
}

/*
 * Function twi_disable
 * Desc     disables twi pins
 * Input    none
 * Output   none
 */
void twi_disable(void) { i2c_deinit(i2c0); }

/*
 * Function twi_readFrom
 * Desc     attempts to become twi bus master and read a
 *          series of bytes from a device on the bus
 * Input    address: 7bit i2c device address
 *          data: pointer to byte array
 *          length: number of bytes to read into array
 *          sendStop: Boolean indicating whether to send a stop at the end
 * Output   number of bytes read
 */
// === MODIFIED ===
bool twi_readFrom(uint8_t address, uint8_t *data, uint8_t length,
                  uint8_t sendStop) {
  int ret = i2c_read_blocking(i2c1, address, data, length, !sendStop);
  return ret > 0;
}
/*
 * Function twi_writeTo
 * Desc     attempts to become twi bus master and write a
 *          series of bytes to a device on the bus
 * Input    address: 7bit i2c device address
 *          data: pointer to byte array
 *          length: number of bytes in array
 *          wait: boolean indicating to wait for write or not
 *          sendStop: boolean indicating whether or not to send a stop at the
 * end Output   0 .. success 1 .. length too long for buffer 2 .. address send,
 * NACK received 3 .. data send, NACK received 4 .. other twi error (lost bus
 * arbitration, bus error, ..)
 */
bool twi_writeTo(uint8_t address, uint8_t *data, uint8_t length, uint8_t wait,
                 uint8_t sendStop) {
  uint8_t ret = i2c_write_blocking(i2c1, address, data, length, !sendStop);
  // i2c_write_blocking finishes when the write is sent but not when it is complete. Delaying 60us is enough to actually wait for the write.
  delay_us(60);
  return ret > 0;
}
