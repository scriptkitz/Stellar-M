#ifndef _COMMON_H_
#define _COMMON_H_
#include <stdarg.h>
#include <stdint.h>

#include <msp430.h>

#define _inline inline
typedef unsigned char       uint8;   //  8 bits

typedef unsigned short int  uint16;  // 16 bits

typedef unsigned long int   uint32;  // 32 bits

typedef unsigned long long  uint64;  // 64 bits


typedef char                int8;    //  8 bits
typedef short int           int16;   // 16 bits
typedef long  int           int32;   // 32 bits
typedef long  long          int64;   // 64 bits

typedef volatile int8       vint8;   //  8 bits
typedef volatile int16      vint16;  // 16 bits
typedef volatile int32      vint32;  // 32 bits
typedef volatile int64      vint64;  // 64 bits

typedef volatile uint8      vuint8;  //  8 bits
typedef volatile uint16     vuint16; // 16 bits
typedef volatile uint32     vuint32; // 32 bits
typedef volatile uint64     vuint64; // 64 bits
//

#ifndef NULL
#define NULL 0
#endif
#define  CPU_F ((double)1000000)
#define  delay_us(x) __delay_cycles((unsigned long)(CPU_F*(double)x/1000000.0))
#define  delay_ms(x) __delay_cycles((unsigned long)(CPU_F*(double)x/1000.0))

typedef struct FontData
{

	uint8_t StartChar;
	uint8_t Width;
	uint8_t Hight;
	uint8_t num;
	const uint8* fp;
} epdFONT_ascii, epdFONT_utf8;



//extern int putchar(int ch);

//static int print2(char **out, int ch);
int prints2(char **out, const char *str, int width, int pad);
int printi2(char **out, int val, int base, int sign, int width, int pad, int alphaBase);
int vprintf2(char **out, const char *fmt, va_list args);
int printf2(const char *fmt, ...);
int sprintf2(char *buf, const char *fmt, ...);

#define  sprintf(_string,format, ...)  sprintf2(_string,format,##__VA_ARGS__)
#endif // !_COMMON_H_
