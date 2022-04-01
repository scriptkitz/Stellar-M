#include "Epaper0213.h"
#include "iic.h"
#include "sht30.h"
#include "Cat_Fonts.h"

/************************************************************************************
 * 墨水屏驱动库
 * 平台：MSP430G2553
 * 屏幕:HINK-E0213A04-G01(汉硕Hanshow Stellar-M价签)
 * BY：Laomao(https://blog.yejiah.com)
 *************************************************************************************/
uint16_t Vbat;
static char String[20];
//CS	- P3.4						//片选
//SDI	- P2.4						//SPI数据
//SCLK	- P2.3						//SPI时钟
//BUSY	- P2.5						//忙状态输出引脚
//D/C	- P3.5						//数据/命令控制引脚
//BS	- P3.1						//总线切换
//nc	- P3.7						//微雪说悬空就行 价签上接了mcu
//P2.6 = H power off；L = power on	//供电控制

//全刷寄存器
const uint8 EPD_2IN13_lut_full_update[] = { 0x22, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x11, 0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
                                            0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };
//局刷寄存器
//const uint8 EPD_2IN13_lut_partial_update[] = { 0x18, 0x00, 0x00, 0x00, 0x00,
//                                               0x00, 0x00, 0x00, 0x00, 0x00,
//                                               0x00, 0x00, 0x00, 0x00, 0x00,
//                                               0x00, 0x0F, 0x01, 0x00, 0x00,
//                                               0x00, 0x00, 0x00, 0x00, 0x00,
//                                               0x00, 0x00, 0x00, 0x00, 0x00 };

const uint8 EPD_2IN13_lut_partial_update[] = { 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

const uint8 hs_lut[] = { 0x66, 0x66, 0x26, 0x04, 0x55, 0xaa, 0x08, 0x91, 0x11, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x19, 0x19, 0x0a, 0x0a, 0x5e, 0x1e, 0x1e, 0x0a, 0x39, 0x14, 0x00, 0x00, 0x00, 0x02, };

//模拟软件SPI
void SendByte_softSPI(uint8 sdbyte)
{
    volatile unsigned int i;
    for (i = 0; i < 8; i++)
    {
        EPD_SCLK_L;
        if (sdbyte & 0x80)
        {
            EPD_SDI_H;
        }
        else
        {
            EPD_SDI_L;
        }
        sdbyte <<= 1;
        EPD_SCLK_H;
    }
    EPD_SCLK_L;
}
//墨水屏发送命令
void EPD_2IN13_SendCommand(uint8 cmd)
{
    EPD_DC_L;
    EPD_CS_L;
    SendByte_softSPI(cmd);
    EPD_CS_H;
    EPD_DC_H;
}
//墨水屏发送数据
void EPD_2IN13_SendData(uint8 data)
{
    EPD_DC_H;
    EPD_CS_L;
    SendByte_softSPI(data);
    EPD_CS_H;
}
void EPD_2IN13_SendData_Multi(const uint8 *data, uint16 data_size)
{
    EPD_DC_H;
    EPD_CS_L;
    while (data_size)
    {
        SendByte_softSPI((*data++));
        data_size--;
    }
    EPD_CS_H;
}

void EPD_2IN13_Reset(void)
{

    EPD_RST_L;
    delay_ms(1);
    EPD_RST_H;
    delay_ms(5);

}
#define adc_buf_len 20
uint16_t adc_buf[adc_buf_len];
uint16_t conv_float_avg(uint16_t *data, uint8_t data_size)
{
    uint8_t i, min_index, max_index;
    uint16_t avg;

    if (data_size < 3)
    {
        return 0;
    }
    min_index = 0;
    max_index = data_size - 1;
    for (i = 0; i < data_size; i++)
    {
        if (data[min_index] > data[i])
        {
            min_index = i;
        }
        else if (data[max_index] < data[i])
        {
            max_index = i;
        }
    }
    avg = 0;
    for (i = 0; i < data_size; i++)
    {
        if (i != min_index && i != max_index)
        {
            avg += data[i] / (data_size - 2);
        }
    }
    return ((uint32_t) avg * (uint32_t) 500 / (uint32_t) 1023);
//    return avg;
}

uint16_t ADC_getVbat()
{

    ADC10CTL0 |= REFON;
    ADC10CTL0 |= ENC;
    uint16_t res[3], i, timeout;
    for (i = 0; i < 3; i++)
    {
        ADC10CTL0 |= ADC10SC;
        while (!(ADC10CTL0 & ADC10IFG) && (timeout < 255))
        {
            timeout++;
            delay_us(1);
        }
        ADC10CTL0 &= ~ADC10IFG;
        res[i] = ADC10MEM;
    }
    ADC10CTL0 &= ~ADC10SC;
    ADC10CTL0 &= ~ENC;
    ADC10CTL0 &= ~REFON;
    if (res[0] > res[1])
    {
        res[0] ^= res[1];
        res[1] ^= res[0];
        res[0] ^= res[1];
    }
    if (res[1] > res[2])
    {
        res[1] ^= res[2];
        res[2] ^= res[1];
        res[1] ^= res[2];
    }
    if (res[0] > res[1])
    {
        return (uint16_t) ((uint32_t) res[0] * (uint32_t) 300 / (uint32_t) 1023);
    }
    else
    {
        return (uint16_t) ((uint32_t) res[1] * (uint32_t) 300 / (uint32_t) 1023);
    }
}

void EPD_2IN13_ReadBusy(void)
{
    uint16_t vbat;
    delay_ms(100);
    while (EPD_BUSY)
    {      //LOW: idle, HIGH: busy
        delay_ms(10);
        //在刷新的时候测试电压比较准确
        vbat = ADC_getVbat();
        if (vbat < Vbat)
        {

            Vbat = vbat;
        }
    }

}
void SetLut(const uint8 *lut)
{
    uint8_t i = 0;
//    EPD_2IN13_SendCommand(WRITE_LUT_REGISTER);
    /* 查询表的长度为30个字节 */
    for (i = 0; i < 30; i++)
    {
        EPD_2IN13_SendData(lut[i]);
    }
}
void EPD_2IN13urnOnDisplay(void)
{
    EPD_2IN13_SendCommand(DISPLAY_UPDATE_CONTROL_2);	//显示更新控制2
    EPD_2IN13_SendData(0xC7);							//微雪是C4 汉硕C7
    EPD_2IN13_SendCommand(MASTER_ACTIVATION);			// 主激活
    EPD_2IN13_SendCommand(TERMINATE_FRAME_READ_WRITE);	// 终止帧读写
    EPD_2IN13_ReadBusy();

}
/**
 * @brief  设置EPD显示窗口位置和大小。
 * @param  x 显示窗口起始X位置。
 * @param  y_x8 显示窗口起始Y位置，设置1等于8像素。
 * @param  x_size 显示窗口X方向大小。
 * @param  y_size_x8 显示窗口Y方向大小，设置1等于8像素。
 * @note   指针已被自动设置至窗口的左上角。
 */
void EPD_2IN13_SetWindows(uint16 x, uint8 y_x8, uint16 x_size, uint8 y_size_x8)
{
    x = EPD_2IN13_HEIGHT - 1 - x;
    x_size = x - x_size + 1; /* x_size已变为x结束地址 */
    y_size_x8 = y_size_x8 + y_x8 - 1; /* y_size已变为y结束地址 */

    EPD_2IN13_SendCommand(SET_RAM_X_ADDRESS_START_END_POSITION); /* 设置X（短边）起始地址和结束地址，根据扫描方式不同，地址设置也不同 */
    EPD_2IN13_SendData(y_x8);
    EPD_2IN13_SendData(y_size_x8 & 0x1F);
    EPD_2IN13_SendCommand(SET_RAM_Y_ADDRESS_START_END_POSITION); /* 设置Y（长边）起始地址和结束地址，根据扫描方式不同，地址设置也不同 */
    EPD_2IN13_SendData(x & 0xFF);
    EPD_2IN13_SendData((x >> 8) & 0x01);
    EPD_2IN13_SendData(x_size & 0xFF);
    EPD_2IN13_SendData((x_size >> 8) & 0x01);

    EPD_2IN13_SendCommand(SET_RAM_X_ADDRESS_COUNTER); /* 设置X（短边）地址计数器 */
    EPD_2IN13_SendData(y_x8);
    EPD_2IN13_SendCommand(SET_RAM_Y_ADDRESS_COUNTER); /* 设置Y（长边）地址计数器 */
    EPD_2IN13_SendData(x & 0xFF);
    EPD_2IN13_SendData((x >> 8) & 0x01);
}

/**
 * @brief  设置EPD显示数据指针位置。
 * @param  x 显示数据指针起始X位置。
 * @param  y_x8 显示数据指针起始Y位置，设置1等于8像素。
 */
static void EPD_2IN13_SetCursor(int x, int y)
{
    EPD_2IN13_SendCommand(SET_RAM_X_ADDRESS_COUNTER);/* 设置X（短边）地址计数器 */
    /* x point must be the multiple of 8 or the last 3 bits will be ignored */
    EPD_2IN13_SendData((x >> 3) & 0xFF);
    EPD_2IN13_SendCommand(SET_RAM_Y_ADDRESS_COUNTER);/* 设置Y（长边）地址计数器 */
    EPD_2IN13_SendData(y & 0xFF);
    EPD_2IN13_SendData((y >> 8) & 0xFF);
    //    EPD_2IN13_ReadBusy();
}
void EPD_2IN13_DeepSleep(void)
{

    EPD_2IN13_SendCommand(DEEP_SLEEP_MODE); //DEEP_SLEEP_MODE
    EPD_2IN13_SendData(0x01);
    //设置电平省电
    EPD_RST_H;
    EPD_NC_L;
    EPD_DC_L;
    EPD_CS_H;
    EPD_SDI_H;
    EPD_SCLK_H;
}
/**
 * @brief  清除EPD控制器内全部显示RAM。
 * @note   上次数据都不要就要先清屏 不然会由于寄存器没数据造成花屏或者残影
 */
void EPD_ClearRAM(void)
{
    uint8 Width = 0, Height = 0;
    Width = (EPD_2IN13_WIDTH % 8 == 0) ? (EPD_2IN13_WIDTH / 8) : (EPD_2IN13_WIDTH / 8 + 1);
    Height = EPD_2IN13_HEIGHT;

    EPD_2IN13_SetWindows(0, 0, EPD_2IN13_WIDTH, EPD_2IN13_HEIGHT);
    for (uint8 j = 0; j < Height; j++)
    {
        EPD_2IN13_SetCursor(0, j);
        EPD_2IN13_SendCommand(WRITE_RAM);
        for (uint8 i = 0; i < Width; i++)
        {
            EPD_2IN13_SendData(0xFF);
        }
    }

}
/**
 * @brief  向EPD控制器发送指定大小的显示数据。
 * @param  data 要发送数据的指针。
 * @param  data_size 要发送数据的大小。
 */
void EPD_SendRAM(const uint8 *data, uint16 data_size)
{
    EPD_2IN13_SendCommand(WRITE_RAM);
    EPD_2IN13_SendData_Multi(data, data_size);
}
//全屏填充
void EPD_2IN13_Fill(uint8 color)
{
    uint8 Width = 0, Height = 0;
    uint8 j, i;
    Width = (EPD_2IN13_WIDTH % 8 == 0) ? (EPD_2IN13_WIDTH / 8) : (EPD_2IN13_WIDTH / 8 + 1);
    Height = EPD_2IN13_HEIGHT;

    EPD_2IN13_SetWindows(0, 0, EPD_2IN13_WIDTH, EPD_2IN13_HEIGHT);
    for (j = 0; j < Height; j++)
    {
        EPD_2IN13_SetCursor(0, j);
        EPD_2IN13_SendCommand(WRITE_RAM);
        for (i = 0; i < Width; i++)
        {
            EPD_2IN13_SendData(color);
        }
    }

    EPD_2IN13urnOnDisplay();
}
//画图函数
void EPD_draw(uint16 x, uint8 y_x8, uint16 x_size, uint8 y_size_x8, const uint8 *dat)
{
    uint16 data_size = x_size * y_size_x8;
    EPD_2IN13_SetWindows(x, y_x8, x_size, y_size_x8);
    EPD_2IN13_SendCommand(WRITE_RAM);
    EPD_SendRAM(dat, data_size);
//    EPD_2IN13_SetWindows(0, 0, 250, 16);
}

/**
 * @brief  清除EPD控制器指定区域显示RAM。
 * @note   执行完成后窗口会恢复至全屏幕。
 */
void EPD_ClearArea(uint16 x, uint8 y_x8, uint16 x_size, uint8 y_size_x8, uint8 color)
{
    uint16 i, data_size;

    data_size = x_size * y_size_x8;
    EPD_2IN13_SetWindows(x, y_x8, x_size, y_size_x8);
    EPD_2IN13_SendCommand(WRITE_RAM);
    for (i = 0; i < data_size; i++)
    {
        EPD_2IN13_SendData(color);
    }
    EPD_2IN13_SetWindows(0, 0, 250, 16);
}
void EPD_2IN13_Display(const uint8 *Image)
{
    uint8 Width, Height;
    Width = (EPD_2IN13_WIDTH % 8 == 0) ? (EPD_2IN13_WIDTH / 8) : (EPD_2IN13_WIDTH / 8 + 1);
    Height = EPD_2IN13_HEIGHT;
    uint8 j, i;
    EPD_2IN13_SetWindows(0, 0, EPD_2IN13_WIDTH, EPD_2IN13_HEIGHT);
    for (j = 0; j < Height; j++)
    {
        EPD_2IN13_SetCursor(0, j);
        EPD_2IN13_SendCommand(WRITE_RAM);
        for (i = 0; i < Width; i++)
        {
            EPD_2IN13_SendData(Image[i + j * Width]);
        }
    }
    EPD_2IN13urnOnDisplay();
}
//初始化墨水用的IO
void EPD_2IN13_GPIO_Init(void)
{
    EPD_BS_L; //BS：VCC: 3线SPI / GND: 4线SPI
    EPD_ON;

    P2DIR &= ~BIT5;                             //P2.5设置为输入
    P2OUT |= BIT5;
    P2REN |= BIT5;

    P2DIR |= BIT3 | BIT4 | BIT6;
    P3DIR |= BIT1 | BIT4 | BIT5 | BIT6 | BIT7;

    P1SEL = 0X0;
    P1SEL2 = 0X0;
    P2SEL = 0X0;
    P2SEL2 = 0X0;
    P3SEL = 0X0;
    P3SEL2 = 0X0;
}
//墨水屏初始化 0代表全刷  1局刷
void EPD_2IN13_Init(uint8_t part)
{

    EPD_2IN13_Reset();
//    if (part == 0)
//    {
//        EPD_2IN13_SendCommand(0x12);                             //SWRESET
//        EPD_2IN13_ReadBusy();
//    }
    EPD_2IN13_SendCommand(0x01); // DRIVER_OUTPUT_CONTROL
    EPD_2IN13_SendData((EPD_2IN13_HEIGHT - 1) & 0xFF);
    EPD_2IN13_SendData(0x00);
    EPD_2IN13_SendData(0x00);           // GD = 0; SM = 0; TB = 0;

    EPD_2IN13_SendCommand(0x0C); // BOOSTER_SOFT_START_CONTROL
    EPD_2IN13_SendData(0xD7);
    EPD_2IN13_SendData(0xD6);
    EPD_2IN13_SendData(0x9D);

    EPD_2IN13_SendCommand(0x2C); // WRITE_VCOM_REGISTER
    if (part == 0)
    {
        EPD_2IN13_SendData(0xA5);     // VCOM 7C
    }
    else
    {
        EPD_2IN13_SendData(0x25);     // VCOM 7C
    }

    EPD_2IN13_SendCommand(0x3A);   // SET_DUMMY_LINE_PERIOD
    EPD_2IN13_SendData(0x06);           // 4 dummy lines per gate

    EPD_2IN13_SendCommand(0x3B);   // SET_GATEIME
    EPD_2IN13_SendData(0x0B);           // 2us per line

    EPD_2IN13_SendCommand(0x3C); // BORDER_WAVEFORM_CONTROL
    EPD_2IN13_SendData(0x33);

    EPD_2IN13_SendCommand(0x11); // DATA_ENTRY_MODE_SETTING
    EPD_2IN13_SendData(0x01);           // X increment; Y increment

    EPD_2IN13_SendCommand(0x32);

    if (part == 0)
    {
        SetLut(&hs_lut[0]);
    }
    else
    {
        SetLut(&EPD_2IN13_lut_partial_update[0]);
    }
}
void EPD_2IN13_Denit(void)
{
    EPD_2IN13_DeepSleep();
    P2DIR &= ~(BIT3 | BIT4 | BIT6);
    P3DIR &= ~(BIT1 | BIT4 | BIT5 | BIT6 | BIT7);
}

/**
 * @brief  绘制水平直线。
 * @param  x 绘制起始X位置。
 * @param  y 绘制起始Y位置。
 * @param  x_size 绘制长度。
 * @param  width 线宽度。
 * @note   Y方向始终会占用8的倍数的像素，例如在0,0位置绘制一条1像素宽高的线，会清除Y方向8像素内的显示数据。
 */
void EPD_DrawHLine(uint16 x, uint8 y, uint16 x_size, uint8 width)
{
    uint16 i;
    uint8 j, k, block, height, yemp, widthemp;

    height = (y % 8 + width - 1) / 8 + 1;
    EPD_2IN13_SetWindows(x, y / 8, x_size, height);
    for (i = 0; i < x_size; i++)
    {
        yemp = y;
        widthemp = width;
        for (j = 0; j < height; j++)
        {
            block = 0x00;
            for (k = (yemp % 8); k < 8; k++)
            {
                block |= 0x80 >> k;
                widthemp -= 1;
                if (widthemp <= 0)
                {
                    break;
                }
            }
            yemp = 0;
            block = ~block;
            EPD_SendRAM((uint8*) &block, 1);
        }
    }
}

/**
 * @brief  绘制垂直直线。
 * @param  x 绘制起始X位置。
 * @param  y 绘制起始Y位置。
 * @param  y_size 绘制长度。
 * @param  width 线宽度。
 * @note   Y方向始终会占用8的倍数的像素，例如在0,0位置绘制一条1像素宽高的线，会清除Y方向8像素内的显示数据。
 */
void EPD_DrawVLine(uint16 x, uint8 y, uint8 y_size, uint16 width)
{
    uint16 i;
    uint8 j, k, block, height, yemp, widthemp;

    height = (y % 8 + y_size - 1) / 8 + 1;
    EPD_2IN13_SetWindows(x, y / 8, width, height);
    for (i = 0; i < width; i++)
    {
        yemp = y;
        widthemp = y_size;
        for (j = 0; j < height; j++)
        {
            block = 0x00;
            for (k = (yemp % 8); k < 8; k++)
            {
                block |= 0x80 >> k;
                widthemp -= 1;
                if (widthemp <= 0)
                {
                    break;
                }
            }
            yemp = 0;
            block = ~block;
            EPD_SendRAM((uint8*) &block, 1);
        }
    }
}

/**
 * @brief  绘制UTF8字符串。
 * @param  x 绘制起始X位置。
 * @param  y_x8 绘制起始Y位置，设置1等于8像素。
 * @param  gap 字符间额外间距。
 * @param  str 要绘制的字符串指针。
 * @param  ascii_font ASCII字符字模指针。
 * @param  utf8_font UTF8字符字模指针。
 * @note   调用的文件必须是utf-8编码 否则中文会错误
 */
#define Interval_Ascii_Utf8 2			//ASCII字符与UTF8的间隔
void EPD_DrawFonts(uint16 x, uint8 y_x8, uint8 gap, const char *str, const epdFONT_ascii *ascii_font,
                   const epdFONT_utf8 *utf8_font)
{

    uint8 i = 0, utf8_size = 0;
    uint16 x_count = 0, font_size = 0;
    const uint8 *ascii_base_addr = NULL;
    uint32 unicode = 0, unicodeemp = 0;

    x_count = 0;
    while (*str != '\0')
    {
        if ((*str & 0x80) == 0x00) /* 普通ASCII字符 */
        {
            if (ascii_font != NULL)
            {
                font_size = ascii_font->Width * ascii_font->Hight / 8;
                ascii_base_addr = ascii_font->fp + (*str - ascii_font->StartChar) * font_size;
                if ((*str - ascii_font->StartChar) >= 0
                        && ascii_base_addr + font_size <= ascii_font->fp + font_size * ascii_font->num) /* 限制数组范围 */
                {
                    EPD_2IN13_SetWindows(x + x_count, y_x8, ascii_font->Width, ascii_font->Hight / 8);
                    EPD_SendRAM(ascii_base_addr, font_size);
                }
                x_count += ascii_font->Width + gap;
            }
            else if (*str == ' ' && utf8_font != NULL) /* 未指定ASCII字体时空格为UTF8字体宽度除2 */
            {
                font_size = (utf8_font->Width / 2) * (utf8_font->Hight / 8);
                EPD_2IN13_SetWindows(x + x_count, y_x8, utf8_font->Width / 2, utf8_font->Hight / 8);
                for (i = 0; i < font_size; i++)
                {
                    utf8_size = 0xFF; /* 借用变量 */
                    EPD_SendRAM(&utf8_size, 1);
                }
                x_count += utf8_font->Width / 2 + gap;
            }
        }
        else if (utf8_font != NULL) /* UTF8字符 */
        {
            unicode = 0x000000;
            utf8_size = 0;
            for (i = 0; i < 5; i++)
            {
                if (*str & (0x80 >> i))
                {
                    utf8_size += 1;
                }
                else
                {
                    break;
                }
            }
            switch (utf8_size)
            {
            case 2:
                if (*(str + 1) != '\0')
                {
                    unicode = ((uint32) (*str & 0x1F)) << 6;
                    str += 1;
                    unicode |= (uint32) *str & 0x3F;
                }
                break;
            case 3:
                if (*(str + 1) != '\0' && *(str + 2) != '\0')
                {
                    unicode = ((uint32) (*str & 0x0F)) << 12;
                    str += 1;
                    unicode |= ((uint32) (*str & 0x3F)) << 6;
                    str += 1;
                    unicode |= (uint32) *str & 0x3F;
                }
                break;
            case 4:
                if (*(str + 1) != '\0' && *(str + 2) != '\0' && *(str + 3) != '\0')
                {
                    unicode = ((uint32) (*str & 0x07)) << 18;
                    str += 1;
                    unicode |= ((uint32) (*str & 0x3F)) << 12;
                    str += 1;
                    unicode |= ((uint32) (*str & 0x3F)) << 6;
                    str += 1;
                    unicode |= (uint32) *str & 0x3F;
                }
                break;
            }
            if (unicode != 0)
            {
                font_size = utf8_font->Width * utf8_font->Hight / 8;
                for (i = 0; i < utf8_font->num; i++) /* 限制数组范围 */
                {
                    unicodeemp = (uint32) utf8_font->fp[0 + (font_size + 3) * i] << 16;
                    unicodeemp |= (uint32) utf8_font->fp[1 + (font_size + 3) * i] << 8;
                    unicodeemp |= (uint32) utf8_font->fp[2 + (font_size + 3) * i];
                    if (unicodeemp == unicode)
                    {
                        EPD_2IN13_SetWindows(x + x_count + Interval_Ascii_Utf8, y_x8, utf8_font->Width,
                                             utf8_font->Hight / 8);
                        EPD_SendRAM(utf8_font->fp + 3 + (font_size + 3) * i, font_size);
                        break;
                    }
                }
            }
            x_count += utf8_font->Width + gap;
        }
        str += 1;
    }
}

static uint8_t dis_ram[114];
void EPD_DrawBattery(uint16_t x, uint8_t y_x8, uint16_t max_voltage, uint16_t min_voltage, uint16_t voltage)
{

    uint8_t i, bar_size;
    const uint8_t bar_end_pos = (24 / 8) * (37 - 5) + 3;
    const uint8_t bar_size_max = 37 - 10;
    if ((voltage < min_voltage) || ((max_voltage - min_voltage) < 1))
    {
        EPD_draw(x, y_x8, 37, 3, &EPD_Image_BattWarn[0]);
        return;
    }
    memcpy(dis_ram, EPD_Image_BattWarn, sizeof(dis_ram));

    if (voltage < max_voltage)
    {
        voltage -= min_voltage;
        bar_size = (voltage * 10 / (((max_voltage - min_voltage) * 10) / bar_size_max));
    }
    else
    {
        bar_size = bar_size_max;
    }
    if (bar_size == 0)
    {
        bar_size = 1;
    }
    else if (bar_size > bar_size_max)
    {
        bar_size = bar_size_max;
    }
    for (i = 0; i < bar_size_max; i++) /* 清空电池图标内部 */
    {
        dis_ram[bar_end_pos - (i * 3) + 0] |= 0x07;
        dis_ram[bar_end_pos - (i * 3) + 1] |= 0xFF;
        dis_ram[bar_end_pos - (i * 3) + 2] |= 0xE0;
    }
    for (i = 0; i < bar_size; i++) /* 绘制填充 */
    {
        dis_ram[bar_end_pos - (i * 3) + 0] &= 0xF0;
        dis_ram[bar_end_pos - (i * 3) + 1] &= 0x00;
        dis_ram[bar_end_pos - (i * 3) + 2] &= 0x0F;
    }
    EPD_draw(x, y_x8, 37, 3, &dis_ram[0]);

}
static uint8_t RH_TYPE = 0;
void epd_RHtest(void)
{

    EPD_2IN13_Init(0);

    EPD_ClearRAM();
    EPD_draw(35, 4, 180, 4, &EPD_RH_INIT[0]);
    EPD_2IN13urnOnDisplay();
    EPD_2IN13_DeepSleep();
    delay_ms(500);
    EPD_2IN13_Init(1);
    EPD_ClearRAM();
    if (I2C_CHECK(SHT30_ADDR) == 0)
    {
        RH_TYPE = 1;
        EPD_DrawFonts(5, 0, 0, "SHT30 OK!", &Ascii_YouYuan_8x16, NULL);
    }
    else if (I2C_CHECK(AHT20_ADDR) == 0)
    {
        RH_TYPE = 2;
        EPD_DrawFonts(5, 0, 0, "AHT20 OK!", &Ascii_YouYuan_8x16, NULL);
    }
    else
    {

        EPD_draw(20, 6, 208, 2, &EPD_RH_F[0]);
    }

    EPD_2IN13urnOnDisplay();
    EPD_2IN13_DeepSleep();
    delay_ms(500);
    if (RH_TYPE == 0)
    {
        CCTL0 &= ~CCIE;
    }

}

uint16_t difference(short a, short b);
uint16_t difference(short a, short b)
{
    return a >= b ? a - b : b - a;
}

struct TH_Value th_value = { 0 };
struct TH_Value th_value_old = { 0 };
#define VBAT_MIN    220
#define VBAT_MAX    300
#define TEST_MAX    60   //最大测试稳定值
void epd_distest(uint16_t part)
{
    static uint8_t part_count = 0;
    static uint8_t flag = 0;
    if (RH_TYPE == 1)
    {
        SHT30_GetValue_SingleShotWithCS(TH_ACC_HIGH, &th_value);
    }
    else if (RH_TYPE == 2)
    {
        AHT20_GetValue_SingleShotWithoutCS(&th_value);
    }
    else
    {
        return;
    }

    if (part != 0 && difference(th_value.RH, th_value_old.RH) < 10 && difference(th_value.CEL, th_value.CEL) < 3)
    {

        if (flag < TEST_MAX)
        {
            flag++;
        }
        else if (flag == TEST_MAX)
        {
            flag++;
            goto START;
        }
        return;
    }
    else
    {
        flag = 0;
    }
    START: if (part_count > 30)
    {
        part_count = 0;
    }

    EPD_2IN13_Init(part_count);
    part_count++;
    EPD_ClearRAM();
    EPD_DrawBattery(210, 0, VBAT_MAX, VBAT_MIN, Vbat);
    EPD_DrawVLine(124, 24, 96, 3);
    EPD_draw(0 + 5, 0, 17, 3, &EPD_T_IMG[0]);
    EPD_draw(125 + 5, 0, 18, 3, &EPD_H_IMG[0]);
    sprintf(String, "%02d", th_value.CEL / 10);
    EPD_DrawFonts(0 + 5, 3, 0, String, &Ascii_DigitalDismay_28x56, NULL);
    sprintf(String, "%02d", th_value.RH / 10);
    EPD_DrawFonts(125 + 5, 3, 0, String, &Ascii_DigitalDismay_28x56, NULL);
    EPD_draw(95 + 5, 3, 20, 5, &EPD_FontAscii_RH[1][0]);
    EPD_draw(219 + 5, 3, 20, 5, &EPD_FontAscii_RH[0][0]);
    EPD_draw(95 + 5, 10, 20, 5, &EPD_FontAscii_RHL[(uint16_t) th_value.CEL % 10][0]);
    EPD_draw(219 + 5, 10, 20, 5, &EPD_FontAscii_RHL[(uint16_t) th_value.RH % 10][0]);
    EPD_2IN13urnOnDisplay();
    EPD_2IN13_DeepSleep();

    memcpy(&th_value_old, &th_value, sizeof(struct TH_Value));

}

