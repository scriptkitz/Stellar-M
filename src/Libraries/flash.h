#ifndef FLASH_H
#define FLASH_H

#include <MSP430.h>

#include <common.h>
#define PRICETAG_DEVICE
void flash_init();
void flash_deinit();


void flash_send_byte(unsigned char data);
unsigned char flash_read_byte();

void flash_read_id(uint8_t *buf);

void flash_read_data(uint8_t *buf, uint32_t addr, uint16_t len);

void flash_sector_erase(uint32_t addr);
void flash_chip_erase();

void flash_write_data(uint32_t addr, uint8_t *buf, uint16_t len);
void flash_sleep(void);
#endif
