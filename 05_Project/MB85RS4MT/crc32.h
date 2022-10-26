/*
 * crc32.h
 *
 *  Created on: Nov 30, 2020
 *      Author: dpenev
 */

#ifndef CRC32_H_
#define CRC32_H_

uint32_t crc32_bitwise(const uint16_t* data, unsigned int length, uint32_t previousCrc32);
uint32_t crc32_bitwise_bytes(const uint16_t* data, unsigned int length, uint32_t previousCrc32);
uint32_t crc32_halfbyte(const uint16_t* data, unsigned int length, uint32_t previousCrc32);

#endif /* CRC32_H_ */
