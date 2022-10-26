/*
 * crc32.c
 *
 *  Created on: Nov 30, 2020
 *      Author: dpenev
 */

#include <stdint.h>

/*
 *  Classical slow CRC32 algorithm working on the 8 bits bytes.
 *  Data stream are processed starting from least significant bit
 *
 *  data            - array of bytes. Only last significant byte is used.
 *  length          - amount of 8 bit bytes
 *  previousCrc32   - seed value for the CRC32
 *
 *  It takes 3897 CPU cycles on F28379
 */
const uint32_t Polynomial = 0xEDB88320;
uint32_t crc32_bitwise_bytes(const uint16_t* data, unsigned int length, uint32_t previousCrc32)
{
  uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  uint16_t* current = (uint16_t *)data;
  unsigned int j;
  while (length--)
  {
    crc ^= *current++;
    for (j = 0; j < 8; j++)
      if (crc & 1)
        crc = (crc >> 1) ^ Polynomial;
      else
        crc =  crc >> 1;
  }
  return ~crc; // same as crc ^ 0xFFFFFFFF
}

/*
 *  Classical slow CRC32 algorithm working on 16 bits words.
 *  Data stream are processed starting from least significant bit
 *
 *  data            - array of words.
 *  length          - amount of 16 bit words
 *  previousCrc32   - seed value for the CRC32 calculation
 *
 *  It takes 3658 CPU cycles on F28379
 */
uint32_t crc32_bitwise(const uint16_t* data, unsigned int length, uint32_t previousCrc32)
{
  uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  uint16_t* current = (uint16_t *)data;
  unsigned int j;
  while (length--)
  {
    crc ^= *current++;
    for (j = 0; j < 16; j++)
      if (crc & 1)
        crc = (crc >> 1) ^ Polynomial;
      else
        crc =  crc >> 1;
  }
  return ~crc; // same as crc ^ 0xFFFFFFFF
}

/*
 *  A bit faster CRC32 algorithm using 16 entry LUT, working on 16 bits words.
 *  Data stream are processed starting from least significant bit
 *
 *  data            - array of words.
 *  length          - amount of 16 bit words
 *  previousCrc32   - seed value for the CRC32 calculation
 *
 *  It takes 806 CPU cycles on F28379
 */
uint32_t crc32_halfbyte(const uint16_t* data, unsigned int length, uint32_t previousCrc32)
{
  uint32_t crc = ~previousCrc32;
  uint16_t* current = (uint16_t *)data;
  static uint32_t lut[16] =
  {
    0x00000000,0x1DB71064,0x3B6E20C8,0x26D930AC,0x76DC4190,0x6B6B51F4,0x4DB26158,0x5005713C,
    0xEDB88320,0xF00F9344,0xD6D6A3E8,0xCB61B38C,0x9B64C2B0,0x86D3D2D4,0xA00AE278,0xBDBDF21C
  };
  while (length--)
  {
    crc = lut[(crc ^  *current       ) & 0x0F] ^ (crc >> 4);
    crc = lut[(crc ^ (*current >> 4) ) & 0x0F] ^ (crc >> 4);
    crc = lut[(crc ^ (*current >> 8) ) & 0x0F] ^ (crc >> 4);
    crc = lut[(crc ^ (*current >> 12)) & 0x0F] ^ (crc >> 4);
    current++;
  }
  return ~crc;
}



