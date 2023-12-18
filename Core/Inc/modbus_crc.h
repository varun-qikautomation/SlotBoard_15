/*
 * modbus_crc.h
 *
 *  Created on: Sep 16, 2022
 *      Author: arunr
 */

#ifndef INC_MODBUS_CRC_H_
#define INC_MODBUS_CRC_H_
#include "stdint.h"

uint16_t crc16(uint8_t *buffer, uint16_t buffer_length);
uint8_t crc8(const uint8_t *data, uint8_t len);

#endif /* INC_MODBUS_CRC_H_ */
