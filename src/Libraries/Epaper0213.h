// e-ink paper display
#ifndef _EPD213_H
#define _EPD213_H
#include "common.h"
/************************************************************************************
* 墨水屏驱动库
* 平台：MSP430G2553
* 屏幕:HINK-E0213A04-G01(汉硕Hanshow Stellar-M价签)
* BY：Laomao(https://blog.yejiah.com)
*************************************************************************************/
//CS - P3.4                         //片选
//SDI - P2.4                        //SPI数据
//SCLK - P2.3                       //SPI时钟
//BUSY - P2.5                       //忙状态输出引脚
//D/C - P3.5                        //数据/命令控制引脚
//BS - P3.1                         //总线切换
//nc - P3.7                         //微雪说悬空就行 价签上接了mcu
//P2.6 = H power off；L = power on   //供电控制

#define EPD_SCLK_L         P2OUT &= ~BIT3
#define EPD_SCLK_H         P2OUT |= BIT3

#define EPD_SDI_L          P2OUT &= ~BIT4
#define EPD_SDI_H          P2OUT |= BIT4

#define EPD_CS_L           P3OUT &= ~BIT4
#define EPD_CS_H           P3OUT |= BIT4

#define EPD_DC_L		   P3OUT &= ~BIT5
#define EPD_DC_H		   P3OUT |= BIT5

#define EPD_RST_L          P3OUT &= ~BIT6
#define EPD_RST_H          P3OUT |= BIT6

#define EPD_BUSY            P2IN & BIT5
#define EPD_BUSY_OUT        P2DIR  |= BIT5
#define EPD_BUSY_H          P2OUT |= BIT5

#define EPD_BS_L           P3OUT &= ~BIT1

#define EPD_NC_H           P3OUT |= BIT7
#define EPD_NC_L           P3OUT &= ~BIT7
#define EPD_ON             P2OUT &= ~BIT6
#define EPD_OFF            P2OUT |= BIT6

#define EPD_2IN13_WIDTH       122
#define EPD_2IN13_HEIGHT      250

#define EPD_2IN13_FULL			0
#define EPD_2IN13_PART			1

/**
 * Display rotate
**/
#define ROTATE_0            0
#define ROTATE_90           90
#define ROTATE_180          180
#define ROTATE_270          270
#define DIS_DIR				ROTATE_270

extern uint16_t Vbat;
#define INVERSE
// EPD2IN13 commands
#define DRIVER_OUTPUT_CONTROL                       0x01
#define BOOSTER_SOFT_START_CONTROL                  0x0C
#define GATE_SCAN_START_POSITION                    0x0F
#define DEEP_SLEEP_MODE                             0x10
#define DATA_ENTRY_MODE_SETTING                     0x11
#define SW_RESET                                    0x12
#define TEMPERATURE_SENSOR_CONTROL                  0x1A
#define MASTER_ACTIVATION                           0x20
#define DISPLAY_UPDATE_CONTROL_1                    0x21
#define DISPLAY_UPDATE_CONTROL_2                    0x22
#define WRITE_RAM                                   0x24
#define WRITE_VCOM_REGISTER                         0x2C
#define WRITE_LUT_REGISTER                          0x32
#define SET_DUMMY_LINE_PERIOD                       0x3A
#define SET_GATE_TIME                               0x3B
#define BORDER_WAVEFORM_CONTROL                     0x3C
#define SET_RAM_X_ADDRESS_START_END_POSITION        0x44
#define SET_RAM_Y_ADDRESS_START_END_POSITION        0x45
#define SET_RAM_X_ADDRESS_COUNTER                   0x4E
#define SET_RAM_Y_ADDRESS_COUNTER                   0x4F
#define TERMINATE_FRAME_READ_WRITE                  0xFF
extern struct TH_Value th_value;

void EPD_2IN13_Init(uint8_t part);
void EPD_2IN13_Denit(void);
void EPD_DrawHLine(uint16 x, uint8 y, uint16 x_size, uint8 width);
void EPD_DrawVLine(uint16 x, uint8 y, uint8 y_size, uint16 width);
void EPD_DrawFonts(uint16 x, uint8 y_x8, uint8 gap, const char* str, const epdFONT_ascii* ascii_font, const epdFONT_utf8* utf8_font);//д��
void epd_distest(uint16_t part);
void EPD_2IN13_DeepSleep(void);
void EPD_2IN13urnOnDisplay(void);
void EPD_2IN13_GPIO_Init(void);
void EPD_draw(uint16 x, uint8 y_x8, uint16 x_size, uint8 y_size_x8, const uint8 *dat);
uint16_t ADC_getVbat();
void epd_RHtest(void);
#endif
