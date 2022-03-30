#include "main.h"
#include "iic.h"
#include "sht30.h"
#include "flash.h"
#define RF_PIN BIT7
#define RF_OUT        P2DIR |= RF_PIN;
//#define rfon               P2OUT &= ~RF_MOS_PIN
#define RF_OFF              P2OUT |= RF_PIN

inline void InitClk(void)
{
    DCOCTL = 0x0000;
    BCSCTL1 = CALBC1_1MHZ;                    // Set range
    DCOCTL = CALDCO_1MHZ;
    BCSCTL3 |= LFXT1S_2; /* Mode 2 for LFXT1 : VLO */
    IFG1 &= ~(OFIFG);
    BCSCTL1 |= DIVA_3;                //辅助系统8分频f=32768/8=4096
    TACTL = TASSEL_1 + MC_1;       // AMCLK，upmode
    CCTL0 = CCIE;                    //CCR0 interrupt enabled
    CCR0 = 4096 * 5;
}

int main(void)
{

    WDTCTL = WDTPW + WDTHOLD;
    EPD_2IN13_GPIO_Init();
    RF_OUT
    ;
    RF_OFF;
    InitClk();
    flash_init();
    flash_sleep();
    flash_deinit();
    P2DIR &= ~BIT0;
//    P2OUT |= BIT0;
//    P2REN |= BIT0;
//    /*打开P2.0口的中断*/
//    P2IE |= BIT0;
//    /*设定为下降沿触发*/
//    P2IES |= BIT0;
//    /*清除中断标志位*/
//    P2IFG &= ~BIT0;
//    _BIS_SR(GIE);// Enable CPU interrupts
    uint16_t ADC10CTL1_R = ADC10CTL1;
    uint16_t ADC10CTL0_R = ADC10CTL0;
    ADC10CTL1 = INCH_11;
    ADC10CTL0 = SREF_1 + ADC10SHT_3 + ADC10SR + REFON + ADC10ON + ENC;
    epd_RHtest();
    static uint16_t count = 0;
    Vbat = ADC_getVbat();
    count = 0;
    while (1)
    {

        epd_distest(count);
        if (count > 600)
        {
            count = 0;
        }
        count++;
//        ADC10CTL0 &= ~ENC;
//        ADC10CTL0 &= ~REFON;
        __bis_SR_register(LPM3_bits | GIE);
//        ADC10CTL0 |= REFON;
//        ADC10CTL0 |= ENC;
    }

}
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{

    TACTL |= TACLR;                    //计数器清零
    LPM3_EXIT;
}
//#pragma vector = PORT2_VECTOR
//__interrupt void Port2_ISR(void)    //Port1_ISR(void)   中断服务函数
//{
//    if (P2IFG & BIT0)   //判断是否P2.0产生中断
//    {
//
//        P2IFG &= ~ BIT0;   //清除标志位
//        LPM4_EXIT;
//    }
//}
