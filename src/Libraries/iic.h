#ifndef _IIC_H_
#define _IIC_H_
#include "common.h"




typedef enum {
    I2C_MASTER_ACK = 0x0,        /*!< I2C ack for each byte read */
    I2C_MASTER_NACK = 0x1,       /*!< I2C nack for each byte read */
    I2C_MASTER_LAST_NACK = 0x2,   /*!< I2C nack for the last byte*/

} i2c_ack_type_t;
uint8_t I2C_CHECK(uint8_t SlaveAddress);
uint8_t I2C_RecvByte(uint8_t ack);
uint8_t I2C_Read_Bytes(uint8_t SlaveAddress, uint8_t REG_Address,
                       uint8_t *REG_data, uint8_t data_len, i2c_ack_type_t ack);
uint8_t I2C_Read_BytesWithoutREG_Address(uint8_t SlaveAddress,
                                         uint8_t *REG_data, uint8_t data_len,
                                         i2c_ack_type_t ack);
uint8_t I2C_Read_Byte(uint8_t SlaveAddress, uint8_t REG_Address,
                      uint8_t *REG_data, uint8_t ack);
uint8_t I2C_Write_Bytes(uint8_t SlaveAddress, uint8_t REG_Address,
                        uint8_t *REG_data, uint8_t data_len);
uint8_t I2C_Write_BytesWithoutREG_Address(uint8_t SlaveAddress,
                        uint8_t *REG_data, uint8_t data_len);
uint8_t I2C_Write_Byte(uint8_t SlaveAddress, uint8_t REG_Address,
                       uint8_t REG_data);
uint8_t I2C_SendByte(uint8_t dat, uint8_t ack_en);
void I2C_Stop();
void I2C_Start();
#endif
