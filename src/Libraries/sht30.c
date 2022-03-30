#include "sht30.h"
#include "iic.h"

static short TemperatureOffset = 0;  //温度偏移
static short HumidityOffset = 0;     //湿度偏移

/**
 * @brief  设置温度偏移。
 * @param  offset 温度偏移。
 */
void TH_SetTemperatureOffset(short offset)
{
    TemperatureOffset = offset;
}

/**
 * @brief  设置湿度偏移。
 * @param  offset 湿度偏移。
 */
void TH_SetHumidityOffset(short offset)
{
    HumidityOffset = offset;
}

/**
 * @brief  读取温度偏移。
 * @return 温度偏移。
 */
short TH_GetTemperatureOffset(void)
{
    return TemperatureOffset;
}

/**
 * @brief  读取湿度偏移。
 * @return 湿度偏移。
 */
short TH_GetHumidityOffset(void)
{
    return HumidityOffset;
}

/**
 * @brief  计算CRC-8校验值。
 * @param  data 要计算的数据。
 * @param  data_size 数据大小。
 * @return 计算结果。
 */
uint8_t crc8(const uint8_t *data, uint8_t data_size)
{
    uint8_t i, j, crc, polynomial;

    polynomial = 0x31;
    crc = 0xFF;
    for (i = 0; i < data_size; i++)
    {
        crc ^= *data++;
        for (j = 0; j < 8; j++)
        {
            if ((crc & 0x80) != 0)
            {
                crc = (crc << 1) ^ polynomial;
            }
            else
            {
                crc = crc << 1;
            }
        }
    }
    return crc;
}

/**
 * @brief  sht30传感器原始数据转为实际数据。
 * @param  raw_data 原始数据。
 * @param  value 实际数据存储结构体。
 */
void SHT30_readout_data_conv(const uint8_t *raw_data, struct TH_Value *value)
{
    uint16_t conv_tmp;
    conv_tmp = (raw_data[0] << 8) | raw_data[1];
    value->CEL = -450 + (short) (((uint32_t) 1750 * conv_tmp) / 65535) + TemperatureOffset;

    conv_tmp = (raw_data[3] << 8) | raw_data[4];
    value->RH = (short) (((uint32_t) 1000 * conv_tmp) / 65535) + HumidityOffset;
    if (value->RH > 1000)
    {
        value->RH = 1000;
    }

}

/**
 * @brief  sht40传感器原始数据转为实际数据。
 * @param  raw_data 原始数据。
 * @param  value 实际数据存储结构体。
 */
void SHT40_readout_data_conv(const uint8_t *raw_data, struct TH_Value *value)
{
    uint16_t conv_tmp;
    conv_tmp = (raw_data[0] << 8) | raw_data[1];
    value->CEL = -450 + (short) (((uint32_t) 1750 * conv_tmp) / 65535) + TemperatureOffset;

    conv_tmp = (raw_data[3] << 8) | raw_data[4];
    value->RH = -60 + (short) (((uint32_t) 1250 * conv_tmp) / 65535) + HumidityOffset;
    if (value->RH > 1000)
    {
        value->RH = 1000;
    }
    if (value->RH < 0)
    {
        value->RH = 0;
    }

}
/**
 * @brief  以单次模式开始转换数据（发送开始转换命令后返回，后续使用其他命令读取返回值）。
 * @param  acc 数据精确等级。
 * @return 1：启动失败，0：启动成功。
 */
uint8_t SHT30_StartConv_SingleShotWithoutCS(uint8_t acc)
{
    uint16_t cmd;
    uint8_t dat[2] = { 0 };
    switch (acc)
    {
    case TH_ACC_HIGH:
        cmd = 0x2400;
        break;
    case TH_ACC_MID:
        cmd = 0x240B;
        break;
    case TH_ACC_LOW:
        cmd = 0x2416;
        break;
    default:
        cmd = 0x2400;
        break;
    }
    dat[0] = (cmd >> 8) & 0xff;
    dat[1] = cmd & 0xff;
    return I2C_Write_BytesWithoutREG_Address(SHT30_ADDR, &dat[0], 2);
}

//CRC校验后，读取SHT40的温度和湿度数据
uint8_t SHT40_GetValue_SingleShotWithoutCS(struct TH_Value *value)
{
    uint8_t res = 0;
    uint8_t ht_tmp[6] = { 0 };
    ht_tmp[0] = 0xFD;
    res = I2C_Write_BytesWithoutREG_Address(SHT40_ADDR, &ht_tmp[0], 1);
    if (res == 0)
    {
        delay_ms(10);
        I2C_Read_BytesWithoutREG_Address(SHT40_ADDR, &ht_tmp[0], 6, I2C_MASTER_LAST_NACK);
        if (crc8(ht_tmp, 2) != ht_tmp[2] || crc8(ht_tmp + 3, 2) != ht_tmp[5])
        {
            return 1;
        }
        SHT40_readout_data_conv(ht_tmp, value);
    }
    return res;
}
/**
 * @brief  读取单次模式转换完成的数据。
 * @param  value 数据存储结构体。
 * @return 1：读取失败，0：读取成功。
 */
uint8_t SHT30_GetValue_SingleShotWithCS(uint8_t acc, struct TH_Value *value)
{
    uint8_t ht_tmp[6] = { 0 };
    SHT30_StartConv_SingleShotWithoutCS(acc);
    delay_ms(20);
    I2C_Read_BytesWithoutREG_Address(SHT30_ADDR, &ht_tmp[0], 6, I2C_MASTER_LAST_NACK);

    if (crc8(ht_tmp, 2) != ht_tmp[2] || crc8(ht_tmp + 3, 2) != ht_tmp[5])
    {
        return 1;
    }
    SHT30_readout_data_conv(ht_tmp, value);
    return 0;

}

uint8_t SHT30_GetStatus(void)
{
    uint8_t i, status_tmp[3], ret;
    const uint8_t mask_map[7] = { 0x80, 0x20, 0x08, 0x04, 0x10, 0x02, 0x01 };
    status_tmp[0] = (0xF32D >> 8) & 0xff;
    status_tmp[1] = 0xF32D & 0xff;
    if (I2C_Write_BytesWithoutREG_Address(SHT30_ADDR, &status_tmp[0], 2))
    {
        return 0xFE;
    }
    if (I2C_Read_BytesWithoutREG_Address(SHT30_ADDR, &status_tmp[0], sizeof(status_tmp), I2C_MASTER_LAST_NACK))
    {
        return 0xFE;
    }
    if (crc8(status_tmp, 2) != status_tmp[2])
    {
        return 0xFF;
    }
    ret = 0;
    for (i = 0; i < sizeof(mask_map); i++)
    {
        if (i < 4)
        {
            if ((status_tmp[0] & mask_map[i]) != 0)
            {
                ret |= 0x40 >> i;
            }
        }
        else
        {
            if ((status_tmp[1] & mask_map[i]) != 0)
            {
                ret |= 0x40 >> i;
            }
        }
    }
    return ret;
}

/**
 * @brief  软复位。
 * @return 1：执行失败，0：执行成功。
 */
uint8_t SHT30_SoftReset(void)
{
    uint8_t th_ret;
    uint8_t mask_map[2] = { 0x30, 0xA2 };
    th_ret = I2C_Write_BytesWithoutREG_Address(SHT30_ADDR, &mask_map[0], 2);
    if (th_ret == 0)
    {
        delay_ms(1); /* 必要延时 */
    }
    return th_ret;
}

/**
 * @brief  获取最后一条命令状态。
 * @return 1：最后一条命令执行失败，0：最后一条命令执行成功。
 */
uint8_t SHT30_GetCmdExecute(void)
{
    return ((SHT30_GetStatus() & 0x02) != 0);
}

/**
 * @brief  AHT20传感器原始数据转为实际数据。
 * @param  raw_data 原始数据。
 * @param  value 实际数据存储结构体。
 */
void AHT20_readout_data_conv(const uint8_t *raw_data, struct TH_Value *value)
{

    uint32_t RetuData = 0;
    short temp = 0;
    RetuData = (RetuData | raw_data[1]) << 8;
    RetuData = (RetuData | raw_data[2]) << 8;
    RetuData = (RetuData | raw_data[3]);
    RetuData = RetuData >> 4;
    temp = (short) ((RetuData * 1000) >> 20);
    value->RH = temp + HumidityOffset;
    RetuData = 0;
    RetuData = (RetuData | raw_data[3]) << 8;
    RetuData = (RetuData | raw_data[4]) << 8;
    RetuData = (RetuData | raw_data[5]);
    RetuData = RetuData & 0xfffff;
    temp = (short) (((RetuData * 2000) >> 20) - 500);
    value->CEL = temp + TemperatureOffset;

    if (value->RH > 1000)
    {
        value->RH = 1000;
    }
}

uint8_t AHT20_Read_Status(void)                     //读取AHT20的状态寄存器
{
    uint8_t Byte_first;
    I2C_Start();
    I2C_SendByte((AHT20_ADDR << 1) | 1, 1);
    Byte_first = I2C_RecvByte(1);
    I2C_Stop();
    return Byte_first;
}

inline void AHT20_SendAC(void) //向AHT20发送AC命令
{

    I2C_Start();
    I2C_SendByte((AHT20_ADDR << 1), 1);
    I2C_SendByte(0xac, 1);
    I2C_SendByte(0x33, 1);
    I2C_SendByte(0x00, 1);
    I2C_Stop();
}

void JH_Reset_REG(uint8_t addr)
{

//    uint8_t buf[3]={0};
//    buf[0]=addr;
//    I2C_Write_BytesWithoutREG_Address(AHT20_ADDR,&buf[0],sizeof(buf));
//    delay_ms(5);
//    I2C_Read_BytesWithoutREG_Address(AHT20_ADDR,&buf[0],sizeof(buf),I2C_MASTER_LAST_NACK);
//    delay_ms(10);
//    buf[0]=0xB0 | addr;
//    I2C_Write_BytesWithoutREG_Address(AHT20_ADDR,&buf[0],sizeof(buf));
    uint8_t Byte_second, Byte_third;
    I2C_Start();
    I2C_SendByte((AHT20_ADDR << 1), 1);
    I2C_SendByte(addr, 1);
    I2C_SendByte(0x0, 1);
    I2C_SendByte(0x0, 1);
    I2C_Stop();

    delay_ms(5); //延时5ms左右
    I2C_Start();
    I2C_SendByte((AHT20_ADDR << 1) | 1, 1);
    I2C_RecvByte(0);
    Byte_second = I2C_RecvByte(0);
    Byte_third = I2C_RecvByte(1);
    I2C_Stop();

    delay_ms(10); //延时10ms左右
    I2C_Start();
    I2C_SendByte((AHT20_ADDR << 1), 1);
    I2C_SendByte(0xB0 | addr, 1);
    I2C_SendByte(Byte_second, 1);
    I2C_SendByte(Byte_third, 1);

    I2C_Stop();

}
void AHT20_Init()
{
    if ((AHT20_Read_Status() & 0x18) != 0x18)
    {
        JH_Reset_REG(0x1b);
        JH_Reset_REG(0x1c);
        JH_Reset_REG(0x1e);
        delay_ms(10);
    }
}

//CRC校验后，读取AHT20的温度和湿度数据
uint8_t AHT20_GetValue_SingleShotWithoutCS(struct TH_Value *value)
{

    uint16_t cnt = 0;
    uint8_t Byte[7] = { 0 };
    AHT20_Init();
    AHT20_SendAC();
    delay_ms(80);
    while (((AHT20_Read_Status() & 0x80) == 0x80)) //直到状态bit[7]为0，表示为空闲状态，若为1，表示忙状态
    {
        delay_ms(2);
        if (cnt++ >= 40)
        {
            break;
        }
    }

    uint8_t res = I2C_Read_BytesWithoutREG_Address(AHT20_ADDR, Byte, sizeof(Byte), I2C_MASTER_LAST_NACK);
    if (res == 0 && crc8(Byte, 6) == Byte[6])
    {
        AHT20_readout_data_conv(Byte, value);
    }
    return res;
}



