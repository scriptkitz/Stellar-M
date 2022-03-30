#include "iic.h"
//=====================================IIC端口定义=================================
#define  SCLPIN  BIT2
#define  SDAPIN  BIT3

#define  SCL1       P2OUT|=SCLPIN
#define  SCL0       P2OUT&= ~SCLPIN
#define  SCLOUT     P2DIR|=SCLPIN
#define  SDA1       P3OUT|=SDAPIN               //IIC数据引脚
#define  SDA0       P3OUT&= ~SDAPIN
#define  SDAIN      P3DIR&= ~SDAPIN
#define  SDAOUT     P3DIR|=SDAPIN
#define  SDADATA    (P3IN&SDAPIN)
#define  I2C_delay()  delay_us(2)


//================================I2C起始信号================================
void I2C_Start()
{

    SCLOUT;
    SDAOUT;
    SDA1;               //拉高数据线
    SCL1;               //拉高时钟线
    I2C_delay();         //延时
    SDA0;               //产生下降沿
    I2C_delay();
    SCL0;               //钳住I2C总线，准备发送或接收数据
}

//================================I2C停止信号================================
void I2C_Stop()
{
    SDAOUT;
    SCL0;
    SDA0;              //STOP:when CLK is high DATA change form low to high
    I2C_delay();
    SCL1;
    SDA1;               //产生上升沿
    I2C_delay();        //发送I2C总线结束信号
}
//================================I2C发送应答信号 入口参数:ack (0:ACK 1:NAK)================================
void I2C_SendACK(uint8_t ack)
{

    SCL0;
    SDAOUT;
    if (ack)
    {
        SDA1;
    }
    else
    {
        SDA0;

    }
    delay_us(2);
    SCL1;
    delay_us(2);
    SCL0;               //拉低时钟线


}
//================================I2C接收应答信号================================
/*
 0 应答 1无应答
 */
uint8_t I2C_RecvACK()
{

    uint8_t time = 0;
    SDAIN;
    SDA1;
    delay_us(1);
    SCL1;
    delay_us(1);

    while (SDADATA)
    {
        time++;
        if (time > 250)
        {
            I2C_Stop();
            return 1;
        }
    }

    SCL0;//时钟输出0

    return 0;
}
//================================向I2C总线发送一个字节数据================================
uint8_t I2C_SendByte(uint8_t dat, uint8_t ack_en)
{
    uint8_t i;
    SDAOUT;
    SCL0;//拉低时钟开始数据传输
    for (i = 0; i < 8; i++)    //8位计数器
    {

        if (dat & 0x80)
        {
            SDA1;
        }
        else
        {
            SDA0;
        }
        dat <<= 1;
        delay_us(2); //对TEA5767这三个延时都是必须的
        SCL1;
        delay_us(2);
        SCL0;
        delay_us(2);
    }
    if (ack_en == 1)
    {
        return I2C_RecvACK();
    }
    else
    {

        return 0;
    }
}
//================================从I2C总线接收一个字节数据 ack (0:ACK 1:NAK)================================
uint8_t I2C_RecvByte(uint8_t ack)
{
    uint8_t i, dat = 0;

    SDAIN;              //8位计数器

    for (i = 0; i < 8; i++)
    {
        SCL0;
        delay_us(2);
        SCL1;
        dat <<= 1;
        if (SDADATA)
        {
            dat++;
        }
        delay_us(1);
    }
    I2C_SendACK(ack);
    return dat;
}
//=======================向I2C设备写入一个字节数据======================================
uint8_t I2C_Write_Byte(uint8_t SlaveAddress, uint8_t REG_Address,
                       uint8_t REG_data)
{
    I2C_Start();                        //起始信号
    if (I2C_SendByte(SlaveAddress << 1, 1))    //发送设备地址+写信号 检测ACK信号
    {
        I2C_Stop();
        return 1;
    }

    if (I2C_SendByte(REG_Address, 1))
    {
        I2C_Stop();
        return 1;
    }

    if (I2C_SendByte(REG_data, 1))  //内部寄存器数据 检测ACK信号
    {
        I2C_Stop();
        return 1;
    }
    I2C_Stop();                         //发送停止信号
    return 0;
}

//=======================向I2C设备写入多个字节数据======================================
uint8_t I2C_Write_Bytes(uint8_t SlaveAddress, uint8_t REG_Address,
                        uint8_t *REG_data, uint8_t data_len)
{
    uint8_t i;
    I2C_Start();                        //起始信号
    if (I2C_SendByte(SlaveAddress << 1, 1))    //发送设备地址+写信号 检测ACK信号
    {
        I2C_Stop();
        return 1;
    }

    if (I2C_SendByte(REG_Address, 1))
    {
        I2C_Stop();
        return 1;
    }

    for (i = 0; i < data_len; i++)
    {
        if (I2C_SendByte(*(REG_data + i), 1))  //内部寄存器数据 检测ACK信号
        {
            I2C_Stop();
            return 1;
        }
    }
    I2C_Stop();                         //发送停止信号
    return 0;
}
//=======================向I2C设备写入多个字节数据======================================
uint8_t I2C_Write_BytesWithoutREG_Address(uint8_t SlaveAddress,
                        uint8_t *REG_data, uint8_t data_len)
{
    uint8_t i;
    I2C_Start();                        //起始信号
    if (I2C_SendByte(SlaveAddress << 1, 1))    //发送设备地址+写信号 检测ACK信号
    {
        I2C_Stop();
        return 1;
    }
    for (i = 0; i < data_len; i++)
    {
        if (I2C_SendByte(*(REG_data + i), 1))  //内部寄存器数据 检测ACK信号
        {
            I2C_Stop();
            return 1;
        }
    }
    I2C_Stop();                         //发送停止信号
    return 0;
}
//======================从I2C设备读取一个字节数据=====================================
uint8_t I2C_Read_Byte(uint8_t SlaveAddress, uint8_t REG_Address,
                      uint8_t *REG_data, uint8_t ack)
{

    I2C_Start();                        //起始信号
    if (I2C_SendByte(SlaveAddress << 1, 1))    //发送设备地址+写信号 检测ACK信号
    {
        I2C_Stop();
        return 1;
    }
    if (I2C_SendByte(REG_Address, 1))   //发送存储单元地址,从0开始 检测ACK信号
    {
        I2C_Stop();
        return 1;
    }
    I2C_Start();                        //起始信号
    if (I2C_SendByte((SlaveAddress << 1) | 0x01, 1))   //发送设备地址+读信号 检测ACK信号
    {
        I2C_Stop();
        return 1;
    }
    *REG_data = I2C_RecvByte(ack);            //读出寄存器数据
    I2C_Stop();                         //停止信号

    return 0;
}
//======================从I2C设备读取多个字节数据=====================================
uint8_t I2C_Read_BytesWithoutREG_Address(uint8_t SlaveAddress,
                                         uint8_t *REG_data, uint8_t data_len,
                                         i2c_ack_type_t ack)
{
    uint8_t i;
    I2C_Start();                        //起始信号
    if (I2C_SendByte((SlaveAddress << 1) | 0x01, 1))   //发送设备地址+读信号 检测ACK信号
    {
        I2C_Stop();
        return 1;
    }
    if (ack != I2C_MASTER_LAST_NACK)
    {
        for (i = 0; i < data_len; i++)
        {
            *(REG_data + i) = I2C_RecvByte((uint8_t) ack);            //读出寄存器数据
        }
    }
    else
    {
        if (data_len == 1)
        {
            *REG_data = I2C_RecvByte(1);
        }
        else
        {
            data_len -= 1;
            for (i = 0; i < data_len; i++)
            {
                *(REG_data + i) = I2C_RecvByte(0);            //读出寄存器数据
            }
            *(REG_data + data_len) = I2C_RecvByte(1);
        }
    }
    I2C_Stop();                         //停止信号

    return 0;
}
//======================从I2C设备读取多个字节数据=====================================
uint8_t I2C_Read_Bytes(uint8_t SlaveAddress, uint8_t REG_Address,
                       uint8_t *REG_data, uint8_t data_len, i2c_ack_type_t ack)
{
//    uint8_t i;
    I2C_Start();                        //起始信号
    if (I2C_SendByte(SlaveAddress << 1, 1))    //发送设备地址+写信号 检测ACK信号
    {
        I2C_Stop();
        return 1;
    }
    if (I2C_SendByte(REG_Address, 1))   //发送存储单元地址,从0开始 检测ACK信号
    {
        I2C_Stop();
        return 1;
    }
    return I2C_Read_BytesWithoutREG_Address(SlaveAddress,REG_data,data_len,ack);
}


uint8_t I2C_CHECK(uint8_t SlaveAddress)
{
    I2C_Start();                        //起始信号

    if (I2C_SendByte(SlaveAddress << 1, 1))  //发送设备地址+写信号 检测ACK信号
    {
        I2C_Stop();
        return 1;
    }
    I2C_Stop();                         //发送停止信号
    return 0;
}


